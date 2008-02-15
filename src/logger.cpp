// -*-c++-*-

/***************************************************************************
                                logger.cpp
                         Classes for game logging
                             -------------------
    begin                : 2008-02-15
    copyright            : (C) 2008 by The RoboCup Soccer Simulator
                           Maintenance Group.
    email                : sserver-admin@lists.sourceforge.net
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LGPL as published by the Free Software  *
 *   Foundation; either version 2 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "logger.h"

#include "player.h"
#include "coach.h"
#include "monitor.h"
#include "field.h"
#include "xpmholder.h"
#include "serverparam.h"
#include "playerparam.h"
#include "heteroplayer.h"

#include "initsender.h"
#include "serializercommonstdv8.h"

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif

const std::string Logger::DEF_TEXT_NAME = "incomplete";
const std::string Logger::DEF_TEXT_SUFFIX = ".rcl";
const std::string Logger::DEF_GAME_NAME = "incomplete";
const std::string Logger::DEF_GAME_SUFFIX = ".rcg";
const std::string Logger::DEF_KAWAY_NAME = "incomplete";
const std::string Logger::DEF_KAWAY_SUFFIX = ".kwy";


Logger::Logger( Stadium & stadium )
    : M_stadium( stadium )
    , M_game_log( static_cast< std::ostream * >( 0 ) )
    , M_text_log( static_cast< std::ostream * >( 0 ) )
    , M_playmode( PM_Null )
    , M_team_l_score( 0 )
    , M_team_r_score( 0 )
    , M_team_l_pen_taken( 0 )
    , M_team_r_pen_taken( 0 )
{

}

Logger::~Logger()
{
    if ( M_game_log )
    {
        delete M_game_log;
        M_game_log = static_cast< std::ostream * >( 0 );
    }

    if ( M_text_log )
    {
        delete M_text_log;
        M_text_log = static_cast< std::ostream * >( 0 );
    }
}

bool
Logger::open()
{
    if ( ServerParam::instance().gameLogging() )
    {
				if ( ! openGameLog() )
        {
            return false;
        }
    }

    if ( ServerParam::instance().textLogging() )
		{
        if ( ! openTextLog() )
        {
            return false;
        }
		}

    return true;
}

bool
Logger::openGameLog()
{
    boost::filesystem::path dir( ServerParam::instance().gameLogDir(),
                                 boost::filesystem::portable_posix_name );
    if ( ! boost::filesystem::exists( dir )
         && ! boost::filesystem::create_directory( dir ) )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << ": can't create game log dir. game_log_dir = ["
                  << ServerParam::instance().gameLogDir() << ']'
                  << std::endl;
        return false;
    }

    M_game_log_name = ServerParam::instance().gameLogDir();
#if defined(_WIN32) || defined(__WIN32__) || defined (WIN32) || defined (__CYGWIN__)
    if ( *M_game_log_name.rbegin() != '\\' )
    {
        M_game_log_name += '\\';
    }
#else
    if ( *M_game_log_name.rbegin() != '/' )
    {
        M_game_log_name += '/';
    }
#endif

    if ( ServerParam::instance().gameLogFixed() )
    {
        M_game_log_name += ServerParam::instance().gameLogFixedName();
    }
    else
    {
        M_game_log_name += Logger::DEF_GAME_NAME;
    }
    M_game_log_name += Logger::DEF_GAME_SUFFIX;

    if ( ServerParam::instance().gameLogCompression() > 0 )
    {
        M_game_log_name += ".gz";
        rcss::gz::gzofstream * f
            = new rcss::gz::gzofstream( M_game_log_name.c_str(),
                                        ServerParam::instance().gameLogCompression() );
        M_game_log = f;
    }
    else
    {
        std::ofstream * f
            = new std::ofstream( M_game_log_name.c_str(),
                                 ( std::ofstream::binary
                                   | std::ofstream::out
                                   | std::ofstream::trunc ) );
        M_game_log = f;
    }

    if ( ! isGameLogOpen() )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << ": can't open record_file " << M_game_log_name << std::endl;
        return false;
    }

    if ( ServerParam::instance().gameLogVersion() != REC_OLD_VERSION )
    {
        // write version information
        char buf[6];
        buf[0] = 'U';
        buf[1] = 'L';
        buf[2] = 'G';
        if ( ServerParam::instance().gameLogVersion() == REC_VERSION_4 )
        {
            buf[3] = '4';
            buf[4] = '\n';
            writeToGameLog( buf, 5 );
        }
        else
        {
            buf[3] = ServerParam::instance().gameLogVersion();
            writeToGameLog( buf, 4 );
        }

        if ( ServerParam::instance().gameLogVersion() == REC_VERSION_4 )
        {
            rcss::InitSenderCommonV8 init_sender( *M_game_log,
                                                  rcss::SerializerCommonStdv8::instance(),
                                                  M_stadium,
                                                  999, // version number
                                                  true ); // new line
            init_sender.sendServerParams();
            init_sender.sendPlayerParams();
            init_sender.sendPlayerTypes();
        }
        else if ( ServerParam::instance().gameLogVersion() == REC_VERSION_3 )
        {
            Int16 mode = htons( PARAM_MODE );
            server_params_t stmp = ServerParam::instance().convertToStruct();
            writeToGameLog( reinterpret_cast< const char * >( &mode ), sizeof( mode ) );
            writeToGameLog( reinterpret_cast< const char * >( &stmp ), sizeof( stmp ) );

            mode = htons( PPARAM_MODE );
            player_params_t ptmp = PlayerParam::instance().convertToStruct();
            writeToGameLog( reinterpret_cast< const char * >( &mode ), sizeof( mode ) );
            writeToGameLog( reinterpret_cast< const char * >( &ptmp ), sizeof( ptmp ) );

            mode = htons( PT_MODE );
            for ( int i = 0; i < PlayerParam::instance().playerTypes(); ++i )
            {
                const HeteroPlayer * p = M_stadium.playerType( i );
                if ( p )
                {
                    player_type_t pt_tmp = p->convertToStruct( i );
                    writeToGameLog( reinterpret_cast< const char * >( &mode ), sizeof( mode ) );
                    writeToGameLog( reinterpret_cast< const char * >( &pt_tmp ), sizeof( pt_tmp ) );
                }
                else
                {
                    return false;
                }
            }
        }

        this->flush();
    }

    return true;
}

bool
Logger::openTextLog()
{
    boost::filesystem::path dir( ServerParam::instance().textLogDir(),
                                 boost::filesystem::portable_posix_name );
    if ( ! boost::filesystem::exists( dir )
         && ! boost::filesystem::create_directory( dir ) )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << ": can't create text log dir. text_log_dir = ["
                  << ServerParam::instance().textLogDir() << ']'
                  << std::endl;
        return false;
    }

    M_text_log_name = ServerParam::instance().textLogDir();
#if defined(_WIN32) || defined(__WIN32__) || defined (WIN32) || defined (__CYGWIN__)
    if ( *M_text_log_name.rbegin() != '\\' )
    {
        M_text_log_name += '\\';
    }
#else
    if ( *M_text_log_name.rbegin() != '/' )
    {
        M_text_log_name += '/';
    }
#endif

    if ( ServerParam::instance().textLogFixed() )
    {
        M_text_log_name += ServerParam::instance().textLogFixedName();
    }
    else
    {
        M_text_log_name += Logger::DEF_TEXT_NAME;
    }
    M_text_log_name += Logger::DEF_TEXT_SUFFIX;

    if ( ServerParam::instance().textLogCompression() > 0 )
    {
        M_text_log_name += std::string ( ".gz" );
        rcss::gz::gzofstream * f
            = new rcss::gz::gzofstream( M_text_log_name.c_str(),
                                        ServerParam::instance().textLogCompression() );
        M_text_log = f;
    }
    else
    {
        std::ofstream * f = new std::ofstream( M_text_log_name.c_str() );
        M_text_log = f;
    }

    if ( ! isTextLogOpen() )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << ": can't open log_file " << M_text_log_name << std::endl;
        return false;
    }

    return true;
}

void
Logger::writeToGameLog( const char * str,
                        const std::streamsize n )
{
    if ( M_game_log )
    {
        M_game_log->write( str, n );
    }
}

void
Logger::writeMsgToGameLog( const BoardType board_type,
                           const char * msg )
{
    if ( ! isGameLogOpen() )
    {
        return;
    }

    if ( ServerParam::instance().gameLogVersion() == REC_VERSION_4 )
    {
        if ( ServerParam::instance().recordMessages() )
        {
            *M_game_log << "(msg " << M_stadium.time()
                        << ' ' << board_type << ' ' << msg << ")\n";
        }
    }
    else if ( ServerParam::instance().gameLogVersion() != REC_OLD_VERSION )
    {
        if ( ServerParam::instance().recordMessages() )
        {
            Int16 mode = htons( MSG_MODE );
            writeToGameLog( reinterpret_cast< const char * >( &mode ),
                            sizeof( mode ) );
            Int16 board = htons( board_type );
            writeToGameLog( reinterpret_cast< const char * >( &board ),
                            sizeof( board ) );
            // calculate the string length and write it

            const Int16 max_len = std::min( Int16( std::strlen( msg ) ), Int16( 2048 ) );
            Int16 len = 1;
            while ( (msg[len-1] != '\0') && (len < max_len) )
            {
                ++len;
            }
            /* pfr 1/7/00 Need to put length in network byte order */
            Int16 nlen = htons( len );
            writeToGameLog( reinterpret_cast< const char* >( &nlen ),
                            sizeof( nlen ) );
            // write the message
            writeToGameLog( msg, len );
        }
    }
    else
    {
        dispinfo_t disp;
        disp.mode = htons( MSG_MODE );
        disp.body.msg.board = htons( board_type );
        std::strncpy( disp.body.msg.message,
                      msg,
                      std::min( sizeof( disp.body.msg.message ),
                                std::strlen( msg ) ) );
        writeToGameLog( reinterpret_cast< const char * >( &disp ),
                        sizeof( dispinfo_t ) );
    }
}

void
Logger::writeGameLog()
{
    /* TH - 2-NOV-2000 */
    static bool wrote_final_cycle = false;

    if ( ! isGameLogOpen() )
    {
        return;
    }

    if ( M_stadium.playmode() != PM_BeforeKickOff
         && M_stadium.playmode() != PM_TimeOver )
    {
        switch ( ServerParam::instance().gameLogVersion() ) {
        case REC_VERSION_4:
            writeGameLogV4();
            break;
        case REC_VERSION_3:
            writeGameLogV3();
            break;
        case REC_VERSION_2:
            writeGameLogV2();
            break;
        case REC_OLD_VERSION:
            writeGameLogV1();
            break;
        default:
            break;
        }
    }
    else if ( M_stadium.playmode() == PM_TimeOver
              && ! wrote_final_cycle )
    {
        switch ( ServerParam::instance().gameLogVersion() ) {
        case REC_VERSION_4:
            writeGameLogV4();
            break;
        case REC_VERSION_3:
            writeGameLogV3();
            break;
        case REC_VERSION_2:
            writeGameLogV2();
            break;
        case REC_OLD_VERSION:
            writeGameLogV1();
            break;
        default:
            break;
        }
        wrote_final_cycle = true;
    }
}

// TODO: replaced with DispSender
void
Logger::writeGameLogV1()
{
    dispinfo_t disp;

    disp.mode = htons( SHOW_MODE );

    disp.body.show.time = htons( M_stadium.time() );

    disp.body.show.pmode = static_cast< char >( M_stadium.playmode() );

    disp.body.show.pos[0].x = htons( (Int16)rint( ( M_stadium.ball().pos().x * SHOWINFO_SCALE ) ) );
    disp.body.show.pos[0].y = htons( (Int16)rint( ( M_stadium.ball().pos().y * SHOWINFO_SCALE ) ) );

    const Stadium::PlayerCont & players = M_stadium.players();
    for ( int i = 0; i < MAX_PLAYER * 2; ++i )
    {
        disp.body.show.pos[i+1].enable = htons( players[i]->state() );
        disp.body.show.pos[i+1].x = htons( (Int16)rint( players[i]->pos().x * SHOWINFO_SCALE ) );
        disp.body.show.pos[i+1].y = htons( (Int16)rint( players[i]->pos().y * SHOWINFO_SCALE ) );
        disp.body.show.pos[i+1].angle = htons( (Int16)Rad2IDegRound( players[i]->angleBodyCommitted() ) );
        disp.body.show.pos[i+1].side = htons( players[i]->team()->side() );
        disp.body.show.pos[i+1].unum = htons( players[i]->unum() );
    }

    if ( ! M_stadium.teamLeft().name().empty() )
    {
        std::strncpy( disp.body.show.team[0].name,
                      M_stadium.teamLeft().name().c_str(),
                      std::min( M_stadium.teamLeft().name().length() + 1, size_t( 16 ) ) );
        disp.body.show.team[0].score = htons( M_stadium.teamLeft().point() );
    }
    else
    {
        std::memset( &disp.body.show.team[0].name,
                     0,
                     sizeof( disp.body.show.team[0].name ) );
    }

    if ( ! M_stadium.teamRight().name().empty() )
    {
        std::strncpy( disp.body.show.team[1].name,
                      M_stadium.teamRight().name().c_str(),
                      std::min( M_stadium.teamRight().name().length() + 1, size_t( 16 ) ) );
        disp.body.show.team[1].score = htons( M_stadium.teamRight().point() );
    }
    else
    {
        std::memset( &disp.body.show.team[1].name,
                     0,
                     sizeof( disp.body.show.team[1].name ) );
    }

    writeToGameLog( reinterpret_cast< const char * >( &disp ),
                    sizeof( dispinfo_t ) );
}

// TODO: replaced with DispSender
void
Logger::writeGameLogV2()
{
    Int16 mode = htons( SHOW_MODE );

    showinfo_t show;

    show.time = htons( M_stadium.time() );

    show.pmode = static_cast< char >( M_stadium.playmode() );

    show.pos[0].x = htons( (Int16)rint( ( M_stadium.ball().pos().x * SHOWINFO_SCALE ) ) );
    show.pos[0].y = htons( (Int16)rint( ( M_stadium.ball().pos().y * SHOWINFO_SCALE ) ) );

    const Stadium::PlayerCont & players = M_stadium.players();
    for ( int i = 0; i < MAX_PLAYER * 2; ++i )
    {
        show.pos[i+1].enable = htons( players[i]->state() );
        show.pos[i+1].x = htons( (Int16)rint( players[i]->pos().x * SHOWINFO_SCALE ) );
        show.pos[i+1].y = htons( (Int16)rint( players[i]->pos().y * SHOWINFO_SCALE ) );
        show.pos[i+1].angle = htons( (Int16)Rad2IDegRound( players[i]->angleBodyCommitted() ) );
        show.pos[i+1].side = htons( players[i]->team()->side() );
        show.pos[i+1].unum = htons( players[i]->unum() );
    }

    if ( ! M_stadium.teamLeft().name().empty() )
    {
        std::strncpy( show.team[0].name,
                      M_stadium.teamLeft().name().c_str(),
                      std::min( M_stadium.teamLeft().name().length() + 1, size_t( 16 ) ) );
        show.team[0].score = htons( M_stadium.teamLeft().point() );
    }
    else
    {
        std::memset( &show.team[0].name,
                     0,
                     sizeof( show.team[0].name ) );
        show.team[0].score = htons( 0 );
    }

    if ( ! M_stadium.teamRight().name().empty() )
    {
        std::strncpy( show.team[1].name,
                      M_stadium.teamRight().name().c_str(),
                      std::min( M_stadium.teamRight().name().length() + 1, size_t( 16 ) ) );
        show.team[1].score = htons( M_stadium.teamRight().point() );
    }
    else
    {
        std::memset( &show.team[1].name,
                     0,
                     sizeof( show.team[1].name ) );
        show.team[1].score = htons( 0 );
    }

    // write a show struct
    writeToGameLog( reinterpret_cast< const char * >( &mode ),
                    sizeof( Int16 ) );
    writeToGameLog( reinterpret_cast< const char * >( &show ),
                    sizeof( showinfo_t ) );
}

// TODO: replaced with DispSender
void
Logger::writeGameLogV3()
{
    static PlayMode pmode = PM_Null;
    static team_t teams[2] = { { "", 0 },
                               { "", 0 } };
    static int score_l = 0;
    static int score_r = 0;

    Int16 mode;

    // if playmode has changed wirte playmode
    if ( pmode != M_stadium.playmode() )
    {
        pmode = M_stadium.playmode();

        char pm = static_cast< char >( pmode );

        mode = htons( PM_MODE );
        writeToGameLog( reinterpret_cast< const char * >( &mode ),
                        sizeof( mode ) );
        writeToGameLog( reinterpret_cast< const char * >( &pm ),
                        sizeof( char ) );
    }

    // if teams or score has changed, write teams and score
    if ( M_stadium.teamLeft().point() != score_l
         || M_stadium.teamRight().point() != score_r
         || M_stadium.teamLeft().name() != teams[ 0 ].name
         || M_stadium.teamRight().name() != teams[ 1 ].name )
    {
        score_l = M_stadium.teamLeft().point();
        score_r = M_stadium.teamLeft().point();

        std::strncpy( teams[ 0 ].name,
                      M_stadium.teamLeft().name().c_str(),
                      std::min( M_stadium.teamLeft().name().length() + 1, size_t( 16 ) ) );
        teams[ 0 ].score = htons( score_l );

        std::strncpy( teams[ 1 ].name,
                      M_stadium.teamRight().name().c_str(),
                      std::min( M_stadium.teamRight().name().length() + 1, size_t( 16 ) ) );
        teams[ 1 ].score = htons( score_r );

        mode = htons( TEAM_MODE );
        writeToGameLog( reinterpret_cast< const char * >( &mode ),
                        sizeof( mode ) );
        writeToGameLog( reinterpret_cast< const char * >( teams ),
                        sizeof( team_t ) * 2 );

    }

    // write positional data
    short_showinfo_t2 show;

    show.time = htons( M_stadium.time() );

    show.ball.x = htonl( (Int32)rint( ( M_stadium.ball().pos().x * SHOWINFO_SCALE2 ) ) );
    show.ball.y = htonl( (Int32)rint( ( M_stadium.ball().pos().y * SHOWINFO_SCALE2 ) ) );
    show.ball.deltax = htonl( (Int32)rint( ( M_stadium.ball().vel().x * SHOWINFO_SCALE2 ) ) );
    show.ball.deltay = htonl( (Int32)rint( ( M_stadium.ball().vel().y * SHOWINFO_SCALE2) ) );

    const Stadium::PlayerCont & players = M_stadium.players();
    for ( int i = 0; i < MAX_PLAYER * 2; ++i )
    {
        show.pos[i].mode = htons( players[i]->state() );
        show.pos[i].type = htons( players[i]->playerTypeId() );

        show.pos[i].x = htonl( (Int32)rint( players[i]->pos().x * SHOWINFO_SCALE2 ) );
        show.pos[i].y = htonl( (Int32)rint( players[i]->pos().y * SHOWINFO_SCALE2 ) );
        show.pos[i].deltax = htonl( (Int32)rint( players[i]->vel().x * SHOWINFO_SCALE2 ) );
        show.pos[i].deltay = htonl( (Int32)rint( players[i]->vel().y * SHOWINFO_SCALE2 ) );
        show.pos[i].body_angle = htonl( (Int32)rint( players[i]->angleBodyCommitted() * SHOWINFO_SCALE2 ) );
        show.pos[i].head_angle = htonl( (Int32)rint( players[i]->angleNeckCommitted() * SHOWINFO_SCALE2 ) );
        show.pos[i].view_width = htonl( (Int32)rint( players[i]->visibleAngle() * SHOWINFO_SCALE2 ) );
        show.pos[i].view_quality = htons( (Int16)rint( players[i]->highquality() ) );

        show.pos[i].stamina = htonl( (Int32)rint( players[i]->stamina() * SHOWINFO_SCALE2 ) );
        show.pos[i].effort = htonl( (Int32)rint( players[i]->effort() * SHOWINFO_SCALE2 ) );
        show.pos[i].recovery = htonl( (Int32)rint( players[i]->recovery() * SHOWINFO_SCALE2 ) );

        show.pos[i].kick_count = htons( players[i]->kickCount() );
        show.pos[i].dash_count = htons( players[i]->dashCount() );
        show.pos[i].turn_count = htons( players[i]->turnCount() );
        show.pos[i].say_count = htons( players[i]->sayCount() );
        show.pos[i].tneck_count = htons( players[i]->turnNeckCount() );
        show.pos[i].catch_count = htons( players[i]->catchCount() );
        show.pos[i].move_count = htons( players[i]->moveCount() );
        show.pos[i].chg_view_count = htons( players[i]->changeViewCount() );
    }

    mode = htons( SHOW_MODE );
    writeToGameLog( reinterpret_cast< const char * >( &mode ),
                    sizeof( mode ) );
    writeToGameLog( reinterpret_cast< const char * >( &show ),
                    sizeof( short_showinfo_t2 ) );
}

// TODO: replaced with DispSender
void
Logger::writeGameLogV4()
{
    static char * playmode_strings[] = PLAYMODE_STRINGS;

    static PlayMode pm = PM_Null;
    static std::string team_l_name, team_r_name;
    static int team_l_score = 0, team_r_score = 0;
    static int team_l_pen_score = 0, team_r_pen_score = 0;

    const double prec = 0.0001;
    const double dprec = 0.001;

    std::ostream & os = *M_game_log;

    // if playmode has changed wirte playmode
    if ( pm != M_stadium.playmode() )
    {
        pm = M_stadium.playmode();

        os << "(playmode " << M_stadium.time()
           << ' ' << playmode_strings[pm] << ")\n";
    }

    // if teams or score has changed, write teams and score
    if ( team_l_score != M_stadium.teamLeft().point()
         || team_l_pen_score != M_stadium.teamLeft().penaltyPoint()
         || team_r_score != M_stadium.teamRight().point()
         || team_r_pen_score != M_stadium.teamRight().penaltyPoint()
         || M_stadium.teamLeft().name() != team_l_name
         || M_stadium.teamRight().name() != team_r_name
         )
    {
        team_l_name = M_stadium.teamLeft().name();
        team_r_name = M_stadium.teamRight().name();
        team_l_score = M_stadium.teamLeft().point();
        team_r_score = M_stadium.teamRight().point();
        team_l_pen_score = M_stadium.teamLeft().penaltyPoint();
        team_r_pen_score = M_stadium.teamRight().penaltyPoint();

        os << "(team " << M_stadium.time()
           << ' ' << ( team_l_name.empty() ? "null" : team_l_name.c_str() )
           << ' ' << ( team_r_name.empty() ? "null" : team_r_name.c_str() )
           << ' ' << team_l_score
           << ' ' << team_r_score;
        if ( M_stadium.teamLeft().penaltyTaken() > 0
             || M_stadium.teamRight().penaltyTaken() > 0 )
        {
            os << ' ' << team_l_pen_score
               << ' ' << M_stadium.teamLeft().penaltyTaken() - team_l_pen_score
               << ' ' << team_r_pen_score
               << ' ' << M_stadium.teamLeft().penaltyTaken() - team_r_pen_score;
        }
        os << ")\n";
    }

    os << "(show " << M_stadium.time();
    os << " (" << BALL_NAME_SHORT
       << ' ' << Quantize( M_stadium.ball().pos().x, prec )
       << ' ' << Quantize( M_stadium.ball().pos().y, prec )
       << ' ' << Quantize( M_stadium.ball().vel().x, prec )
       << ' ' << Quantize( M_stadium.ball().vel().y, prec )
       << ')';

    const Stadium::PlayerCont::const_iterator end = M_stadium.players().end();
    for ( Stadium::PlayerCont::const_iterator p = M_stadium.players().begin();
          p != end;
          ++p )
    {
        os << " (";
        os << "(" << SideStr( (*p)->team()->side() )
           << ' ' << (*p)->unum()
           << ')';
        os << ' ' << (*p)->playerTypeId()
           << std::hex << std::showbase
           << (*p)->state() // include goalie flag
           << std::dec << std::noshowbase;

        os << ' ' << Quantize( (*p)->pos().x, prec )
           << ' ' << Quantize( (*p)->pos().y, prec )
           << ' ' << Quantize( (*p)->vel().x, prec )
           << ' ' << Quantize( (*p)->vel().y, prec )
           << ' ' << Quantize( Rad2Deg( (*p)->angleBodyCommitted() ), dprec )
           << ' ' << Quantize( Rad2Deg( (*p)->angleNeckCommitted() ), dprec );
        if ( (*p)->arm().isPointing() )
        {
            rcss::geom::Vector2D arm_dest;
            if ( (*p)->arm().getRelDest( rcss::geom::Vector2D( (*p)->pos().x,
                                                               (*p)->pos().y ),
                                         (*p)->angleBodyCommitted()
                                         + (*p)->angleNeckCommitted(),
                                         arm_dest ) )
            {
                os << ' ' << Quantize( arm_dest.getMag(), prec )
                   << ' ' << Quantize( Rad2Deg( arm_dest.getHead() ), dprec );
            }
        }

        os << " (v "
           << ( (*p)->highquality() ? "h " : "l " )
           << Quantize( Rad2Deg( (*p)->visibleAngle() ), dprec )
           << ')';
        os << " (s "
           << (*p)->stamina() << ' '
           << (*p)->effort() << ' '
           << (*p)->recovery() << ')';
        if ( (*p)->state() != DISABLE
             && (*p)->getFocusTarget() != NULL )
        {
            os << " (f " << SideStr( (*p)->getFocusTarget()->team()->side() )
               << ' ' << (*p)->getFocusTarget()->unum()
               << ')';
        }
        os << " (c "
           << (*p)->kickCount()
           << ' ' << (*p)->dashCount()
           << ' ' << (*p)->turnCount()
           << ' ' << (*p)->catchCount()
           << ' ' << (*p)->moveCount()
           << ' ' << (*p)->turnNeckCount()
           << ' ' << (*p)->changeViewCount()
           << ' ' << (*p)->sayCount()
           << ' ' << (*p)->tackleCount()
           << ' ' << (*p)->arm().getCounter()
           << ' ' << (*p)->attentiontoCount()
           << ')';
        os << ')'; // end of player
    }

    os << ')' << std::endl; // new line & flush
}

void
Logger::writeTextLog( const char * message,
                      const int flag )
{
    if ( flag == RECV )
    {
        if ( isTextLogOpen() )
        {
            *M_text_log << M_stadium.time() << "\t" << message;
        }
    }

    if ( ( flag == RECV || flag == SUBS )
         && ( ( isGameLogOpen()
                && ServerParam::instance().recordMessages() )
              || ServerParam::instance().sendComms() )
         )
    {
        char buf[max_message_length_for_display];
        std::strncpy( buf, message, std::min( max_message_length_for_display,
                                              (int)std::strlen( message ) ) );

        if ( ServerParam::instance().sendComms() )
        {
            for ( Stadium::MonitorCont::iterator i = M_stadium.monitors().begin();
                  i != M_stadium.monitors().end();
                  ++i )
            {
                (*i)->sendMsg( LOG_BOARD, buf );
            }
        }

        if ( isGameLogOpen()
             && ServerParam::instance().recordMessages()
             && M_stadium.playmode() != PM_TimeOver )
        {
            writeMsgToGameLog( LOG_BOARD, buf );
        }
    }
}

void
Logger::writePlayerLog( const Player & player,
                        const char * message,
                        const int flag )
{
    if ( isTextLogOpen()
         || ( isGameLogOpen() && flag == RECV )
         || ( ServerParam::instance().sendComms() && flag == RECV ) )
    {
        char tmp[MaxMesg];
        std::snprintf( tmp, MaxMesg,
                       "%s %s_%d: %s\n",
                       (flag == SEND) ? "Send" : "Recv",
                       player.team()->name().c_str(),
                       player.unum(),
                       message );
        writeTextLog( tmp, flag );
    }
}

void
Logger::writeCoachLog( const char * message,
                       const int flag )
{
    if ( isTextLogOpen()
         || ( isGameLogOpen() && flag == RECV )
         || ( ServerParam::instance().sendComms() && flag == RECV ) )
    {
        char tmp[MaxMesg];
        std::snprintf( tmp, MaxMesg,
                       "%s Coach: %s\n",
                       (flag == SEND) ? "Send" : "Recv",
                       message ) ;
        writeTextLog( tmp, flag );
    }
}

void
Logger::writeOnlineCoachLog( const OnlineCoach & coach,
                             const char * message,
                             const int flag )
{
    if ( isTextLogOpen()
         || ( isGameLogOpen() && flag == RECV )
         || ( ServerParam::instance().sendComms() && flag == RECV ) )
    {
        char tmp[MaxMesg];
        std::snprintf( tmp, MaxMesg,
                       "%s %s_Coach: %s\n",
                       (flag == SEND) ? "Send" : "Recv",
                       ( coach.side() == LEFT
                         ? M_stadium.teamLeft().name().c_str()
                         : M_stadium.teamRight().name().c_str() ),
                       message );
        writeTextLog( tmp, flag );
    }
}

void
Logger::writeTimes( const timeval & old_time,
                    const timeval & new_time )
{
    if ( isTextLogOpen()
         && ServerParam::instance().logTimes() )
    {
        double diff = (new_time.tv_sec - old_time.tv_sec) * 1000
            + (double)(new_time.tv_usec - old_time.tv_usec) / 1000;

        *M_text_log << M_stadium.time() << "\tCYCLE_TIMES: " << diff << std::endl;
    }
}

void
Logger::writeProfile( const timeval & start_time,
                      const timeval & end_time,
                      const char * str )
{
    if ( isTextLogOpen()
         && ServerParam::instance().profile() )
    {
        double diff = (end_time.tv_sec - start_time.tv_sec) * 1000
            + (double)(end_time.tv_usec - start_time.tv_usec) / 1000;

        *M_text_log << M_stadium.time() << "\t" << str << ": " << diff << std::endl;
    }
}
