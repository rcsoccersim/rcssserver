// -*-c++-*-

/***************************************************************************
                                 monitor.cc
    A class providing the communication interface for remote monitors that
    connect to the server
                             -------------------
    begin                : 27-DEC-2001
    copyright            : (C) 2001 by The RoboCup Soccer Server
                           Maintainance Group.
    email                : sserver-admin@lists.sourceforge.net
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LGPL as published by the Free Software  *
 *   Foundation; either version 3 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "monitor.h"

#include "dispsender.h"
#include "initsendermonitor.h"
#include "serializermonitor.h"

#include "stadium.h"
#include "player.h"
#include "team.h"
#include "types.h"
#include "xpmholder.h"

#ifdef HAVE_SSTREAM
#include <sstream>
#else
#include <strstream>
#endif

// #ifdef HAVE_NETINET_IN_H
// #include <netinet/in.h>
// #endif
// #ifdef HAVE_WINDOWS_H
// #include <windows.h>
// #endif

namespace {

PlayMode
play_mode_id( const char * mode )
{
    static const char * playmode_strings[] = PLAYMODE_STRINGS;

    for ( int n = 0; n < PM_MAX; ++n )
    {
        if ( ! std::strcmp( playmode_strings[n], mode ) )
        {
            return static_cast< PlayMode >( n );
        }
    }
    return PM_Null;
}

}


Monitor::Monitor( Stadium & stadium,
                  const double & version )
    : M_init_observer( new rcss::InitObserverMonitor ),
      M_observer( new rcss::ObserverMonitor ),
      M_stadium( stadium ),
      M_version ( version ),
      M_playmode( PM_Null ),
      M_team_l_name( "" ),
      M_team_r_name( "" ),
      M_team_l_score( 0 ),
      M_team_r_score( 0 ),
      M_team_l_pen_taken( 0 ),
      M_team_r_pen_taken( 0 )
{

}

Monitor::~Monitor()
{
    if ( M_init_observer )
    {
        delete M_init_observer;
        M_init_observer = static_cast< rcss::InitObserverMonitor * >( 0 );
    }

    if ( M_observer )
    {
        delete M_observer;
        M_observer = static_cast< rcss::ObserverMonitor * >( 0 );
    }
}

void
Monitor::parseMsg( char * msg,
                   const size_t & len )
{
    char * str = msg;
    if ( str[ len - 1 ] != 0 )
    {
        if ( version() >= 2.0 )
        {
            sendMsg( MSG_BOARD,
                     "(warning message_not_null_terminated)" );
        }
        str[ len ] = 0;
    }
    parseCommand( str );
}


bool
Monitor::setSenders()
{
    //std::cerr << "Monitor::setSenders() version=" << version() << std::endl;
    rcss::SerializerMonitor::Creator ser_cre;
    if ( ! rcss::SerializerMonitor::factory().getCreator( ser_cre,
                                                          (int)version() ) )
    {
        std::cout << "Unsupported monitor protocol version. version=" << version()
                  << std::endl;
        return false;
    }

    const rcss::SerializerMonitor::Ptr ser = ser_cre();
    if ( ! ser )
    {
        std::cout << "Could not create monitor serializer. " << std::endl;
        return false;
    }

    //
    // disp sender
    //
    {
        rcss::DispSenderMonitor::Params disp_params( getTransport(),
                                                     *this,
                                                     ser,
                                                     M_stadium );
        rcss::DispSenderMonitor::Creator disp_cre;
        if ( ! rcss::DispSenderMonitor::factory().getCreator( disp_cre,
                                                              (int)version() ) )
        {
            std::cerr << "failed to create DispSenderMonitor v" << version()
                      << std::endl;
            return false;
        }
        M_observer->setDispSender( disp_cre( disp_params ) );
    }

    //
    // init sender
    //
    {
        rcss::InitSenderMonitor::Params init_params( getTransport(),
                                                     *this,
                                                     ser,
                                                     M_stadium );
        rcss::InitSenderMonitor::Creator init_cre;
        if ( ! rcss::InitSenderMonitor::factory().getCreator( init_cre,
                                                              (int)version() ) )
        {
            std::cerr << "failed to create InitSenderMonitor v" << version()
                      << std::endl;
            return false;
        }
        M_init_observer->setInitSender( init_cre( init_params ) );
    }

    return true;
}

void
Monitor::sendInit()
{
    M_init_observer->sendServerParams();
    M_init_observer->sendPlayerParams();
    M_init_observer->sendPlayerTypes();
}


void
Monitor::sendPlayMode()
{
    if ( M_playmode == M_stadium.playmode() )
    {
        return;
    }

    M_playmode = M_stadium.playmode();

    M_init_observer->sendPlayMode();
}

void
Monitor::sendScore()
{
    if ( M_team_l_score != M_stadium.teamLeft().point()
         || M_team_l_pen_taken != M_stadium.teamLeft().penaltyTaken()
         || M_team_r_score != M_stadium.teamRight().point()
         || M_team_r_pen_taken != M_stadium.teamRight().penaltyTaken()
         || M_team_l_name != M_stadium.teamLeft().name()
         || M_team_r_name != M_stadium.teamRight().name()
         )
    {
        M_team_l_name = M_stadium.teamLeft().name();
        M_team_r_name = M_stadium.teamRight().name();
        M_team_l_score = M_stadium.teamLeft().point();
        M_team_r_score = M_stadium.teamRight().point();
        M_team_l_pen_taken = M_stadium.teamLeft().penaltyTaken();
        M_team_r_pen_taken = M_stadium.teamRight().penaltyTaken();

        M_init_observer->sendScore();
    }
}


void
Monitor::sendTeamGraphics()
{
    const int MAX_SEND = ( M_stadium.playmode() == PM_BeforeKickOff
                           ? 32
                           : 8 );

    int send_count = 0;
//     int left_send_count = 0;
//     int right_send_count = 0;

    if ( M_left_sent_graphics.size() < M_stadium.teamLeft().teamGraphics().size() )
    {
        const Team::GraphCont::const_reverse_iterator end = M_stadium.teamLeft().teamGraphics().rend();
        for ( Team::GraphCont::const_reverse_iterator it = M_stadium.teamLeft().teamGraphics().rbegin();
              it != end;
              ++it )
        {
            if ( send_count >= MAX_SEND ) break;

            if ( M_left_sent_graphics.find( it->first ) != M_left_sent_graphics.end() )
            {
                continue;
            }

//             std::cerr << "Monitor::sendTeamGraphics() send left "
//                       << it->first.first << ','
//                       << it->first.second
//                       << '\n';
            M_observer->sendTeamGraphic( LEFT,
                                         it->first.first, it->first.second );
            M_left_sent_graphics.insert( it->first );
            ++send_count;
//             ++left_send_count;
        }
    }

    if ( M_right_sent_graphics.size() < M_stadium.teamRight().teamGraphics().size() )
    {
        const Team::GraphCont::const_reverse_iterator end = M_stadium.teamRight().teamGraphics().rend();
        for ( Team::GraphCont::const_reverse_iterator it = M_stadium.teamRight().teamGraphics().rbegin();
              it != end;
              ++it )
        {
            if ( send_count >= MAX_SEND ) break;

            if ( M_right_sent_graphics.find( it->first ) != M_right_sent_graphics.end() )
            {
                continue;
            }

//             std::cerr << "Monitor::sendTeamGraphics() send right "
//                       << it->first.first << ','
//                       << it->first.second
//                       << '\n';
            M_observer->sendTeamGraphic( RIGHT,
                                         it->first.first, it->first.second );
            M_right_sent_graphics.insert( it->first );
            ++send_count;
//             ++right_send_count;
        }
    }

//     if ( send_count > 0 )
//     {
//         std::cerr << "Monitor::sendTeamGraphics() send_count=" << send_count << '\n'
//                   << "                            left  send_count=" << left_send_count << '\n'
//                   << "                            right send_count=" << right_send_count << '\n'
//                   << std::flush;
//     }
}


void
Monitor::sendShow()
{
    M_observer->sendShow();
    sendTeamGraphics();
}

int
Monitor::sendMsg( const BoardType board,
                  const char * msg )
{
    M_observer->sendMsg( board, msg );

//     if ( version() >= 3.0 )
//     {
//         char buf[MaxMesg];
//         snprintf( buf, MaxMesg,
//                   "(msg %d %d \"%s\")",
//                   M_stadium.time(), board, msg );
//         return RemoteClient::send( buf, std::strlen( buf ) + 1 );
//     }
//     else if ( version() >= 2.0 )
//     {
//         dispinfo_t2 minfo;
//         minfo.mode = htons( MSG_MODE );
//         minfo.body.msg.board = htons( board );
//         std::strncpy( minfo.body.msg.message, msg, max_message_length_for_display );
//         return RemoteClient::send( reinterpret_cast< char * >( &minfo ),
//                                    sizeof( dispinfo_t2 ) );
//     }
//     else if ( version() >= 1.0 )
//     {
//         dispinfo_t minfo;
//         minfo.mode = htons( MSG_MODE );
//         minfo.body.msg.board = htons( board );
//         std::strncpy( minfo.body.msg.message, msg, max_message_length_for_display );
//         return RemoteClient::send( reinterpret_cast< const char * >( &minfo ),
//                                    sizeof( dispinfo_t ) );
//     }
    return 0;
}

bool
Monitor::parseCommand( const char * message )
{
    if ( ! std::strcmp( message, "(dispbye)" ) )
    {
        disable();
        return true;
    }
    else if ( ! std::strcmp( message, "(dispstart)" ) )
    {
        M_stadium.kickOff();
        return true;
    }
    else if ( ! std::strncmp( message, "(dispplayer", 11 ) )
    {
        return dispplayer( message );
    }
    else if ( ! std::strncmp( message, "(dispdiscard", 12 ) )
    {
        return dispdiscard( message );
    }
    else if ( ! std::strncmp( message, "(compression", 12 ) )
    {
        return compression( message );
    }
    else if ( ! std::strncmp( message, "(dispfoul", 9 ) )
    {
        return dispfoul( message );
    }
    else if ( ! std::strncmp( message, "(dispcard", 9 ) )
    {
        return dispcard( message );
    }
    else if ( ServerParam::instance().coachMode()
              || ServerParam::instance().coachWithRefereeMode() )
    {
        if ( ! std::strncmp( message, "(start)", 7 ) )
        {
            M_stadium.kickOff();
            sendMsg( MSG_BOARD, "(ok start)" );
            return true;
        }
        else if ( ! std::strncmp( message, "(change_mode", 12 ) )
        {
            return coach_change_mode( message );
        }
        else if ( ! std::strncmp( message, "(move", 5 ) )
        {
            return coach_move( message );
        }
        else if ( ! std::strncmp( message, "(recover", 8 ) )
        {
            return coach_recover();
        }
        else if ( ! std::strcmp( message, "change_player_type" ) )
        {
            return coach_change_player_type( message );
        }
        else if ( ! std::strncmp( message, "(check_ball", 11 ) )
        {
            return coach_check_ball();
        }
        else
        {
            sendMsg( MSG_BOARD, "(error illegal_command_form)" );
            return false;
        }
    }
    else
    {
        sendMsg( MSG_BOARD, "(error illegal_command_form)" );
        return false;
    }

    return true;
}

bool
Monitor::dispfoul( const char * command )
{
    // foul or drop_ball
    int x, y, side;
    if ( std::sscanf( command,
                      " ( dispfoul %d %d %d ) ",
                      &x, &y, &side ) != 3 )
    {
        sendMsg( MSG_BOARD, "(error illegal_command_form)" );
        return false;
    }

    double real_x = x / SHOWINFO_SCALE;
    double real_y = y / SHOWINFO_SCALE;
    if ( static_cast< Side >( side ) == NEUTRAL )
    {
        M_stadium.dropBall( PVector( real_x, real_y ) );
    }
    else
    {
        M_stadium.callFoul( static_cast< Side >( side ),
                            PVector( real_x, real_y ) );

    }

    return true;
}

bool
Monitor::dispplayer( const char * command )
{
    // a player is given new position by the monitor
    int side, unum;
    int x, y, a;
    if ( std::sscanf( command,
                      " ( dispplayer %d %d %d %d %d ) ",
                      &side, &unum, &x, &y, &a ) != 5 )
    {
        sendMsg( MSG_BOARD, "(error illegal_command_form)" );
        return false;
    }

    double real_x = x / SHOWINFO_SCALE;
    double real_y = y / SHOWINFO_SCALE;
    double angle = Deg2Rad( a );
    PVector vel( 0.0, 0.0 );

    return M_stadium.movePlayer( static_cast< Side >( side ),
                                 unum,
                                 PVector( real_x, real_y ),
                                 &angle,
                                 &vel );
}

bool
Monitor::dispdiscard( const char * command )
{
    // a player is discarded by the monitor
    int side = 0, unum = 0;
    if ( std::sscanf( command,
                      " ( dispdiscard %d %d ) ",
                      &side, &unum ) != 2 )
    {
        sendMsg( MSG_BOARD, "(error illegal_command_form)" );
        return false;
    }

    M_stadium.discardPlayer( static_cast< Side >( side ), unum );
    return true;
}


bool
Monitor::dispcard( const char * command )
{
    // a player is punished (given the yellow/red card) by the monitor
    int side = 0, unum = 0;
    char card[8];
    if ( std::sscanf( command,
                      " ( dispcard %d %d %7[^)] ) ",
                      &side, &unum, card ) != 3 )
    {
        sendMsg( MSG_BOARD, "(error illegal_command_form)" );
        return false;
    }

    if ( ! std::strcmp( "yellow", card ) )
    {
        M_stadium.yellowCard( static_cast< Side >( side ), unum );
    }
    else if ( ! std::strcmp( "red", card ) )
    {
        M_stadium.redCard( static_cast< Side >( side ), unum );
    }
    else
    {
        sendMsg( MSG_BOARD, "(error illegal_command_form)" );
        return false;
    }

    return true;
}

bool
Monitor::compression( const char * command )
{
    int level = 0;
    if ( std::sscanf( command, " ( compression %d ) ",
                      &level ) != 1 )
    {
        sendMsg( MSG_BOARD, "(error illegal_command_form)" );
        return false;
    }
#ifdef HAVE_LIBZ
    if ( level > 9 )
    {
        sendMsg( MSG_BOARD, "(error illegal_command_form)" );
        return false;
    }

#ifdef HAVE_SSTREAM
    std::ostringstream reply;
    reply << "(ok compression " << level << ")";
    sendMsg( MSG_BOARD, reply.str().c_str() );
#else
    std::ostrstream reply;
    reply << "(ok compression " << level << ")" << std::ends;
    sendMsg( MSG_BOARD, reply.str() );
    reply.freeze( false );
#endif
    setCompressionLevel( level );
    return true;
#else
    sendMsg( MSG_BOARD, "(warning compression_unsupported)" );
    return false;
#endif
}

bool
Monitor::coach_change_mode( const char * command )
{
    char new_mode[128];
    if ( std::sscanf( command,
                      " ( change_mode %127[-0-9a-zA-Z.+*/?<>_] ) ",
                      new_mode ) != 1 )
    {
        sendMsg( MSG_BOARD, "(error illegal_command_form)" );
        return false;
    }

    PlayMode mode_id = play_mode_id( new_mode );

    if ( mode_id == PM_Null )
    {
        sendMsg( MSG_BOARD, "(error illegal_mode)" );
        return false;
    }

    M_stadium.changePlayMode( mode_id );
    sendMsg( MSG_BOARD, "(ok change_mode)" );
    return true;
}

bool
Monitor::coach_move( const char * command )
{
    char obj[128];
    double x = 0.0, y = 0.0, ang = 0.0, velx = 0.0, vely = 0.0;

    int n = std::sscanf( command,
                         " ( move ( %127[^)] ) %lf %lf %lf %lf %lf ) ",
                         obj, &x, &y, &ang, &velx, &vely );

    if ( n < 3
         || std::isnan( x ) != 0
         || std::isnan( y ) != 0
         || std::isnan( ang ) != 0
         || std::isnan( velx ) != 0
         || std::isnan( vely ) != 0 )
    {
        sendMsg( MSG_BOARD, "(error illegal_object_form)" );
        return false;
    }

    std::string obj_name = "(";
    obj_name += obj;
    obj_name += ')';

    if ( obj_name == BALL_NAME )
    {
        M_stadium.clearBallCatcher();

        if ( n == 3 || n == 4 )
        {
            M_stadium.moveBall( PVector( x, y ), PVector( 0.0, 0.0 ) );
        }
        else if ( n == 6 )
        {
            M_stadium.moveBall( PVector( x, y ), PVector( velx, vely ) );
        }
        else
        {
            sendMsg( MSG_BOARD, "(error illegal_command_form)" );
            return false;
        }
    }
    else
    {
        char teamname[128];
        int unum = 0;

        if ( std::sscanf( obj_name.c_str(),
                          PLAYER_NAME_FORMAT,
                          teamname, &unum ) != 2
             || unum < 1
             || MAX_PLAYER < unum )
        {
            sendMsg( MSG_BOARD, "(error illegal_object_form)" );
            return false;
        }

        Side side = ( M_stadium.teamLeft().name() == teamname
                      ? LEFT
                      : M_stadium.teamRight().name() == teamname
                      ? RIGHT
                      : NEUTRAL );

        PVector pos( x, y );
        PVector vel( velx, vely );
        ang = Deg2Rad( rcss::bound( ServerParam::instance().minMoment(),
                                    ang,
                                    ServerParam::instance().maxMoment() ) );
        if ( n == 3 )
        {
            M_stadium.movePlayer( side, unum, pos, NULL, NULL );
        }
        else if ( n == 4 )
        {
            M_stadium.movePlayer( side, unum, pos, &ang, NULL );
        }
        else if ( n == 6 )
        {
            M_stadium.movePlayer( side, unum, pos, &ang, &vel );
        }
        else
        {
            sendMsg( MSG_BOARD, "(error illegal_command_form)" );
            return false;
        }
    }

    sendMsg( MSG_BOARD, "(ok move)" );
    return true;
}

bool
Monitor::coach_recover()
{
    M_stadium.recoveryPlayers();

    sendMsg( MSG_BOARD, "(ok recover)" );
    return true;
}

bool
Monitor::coach_check_ball()
{
#ifdef HAVE_SSTREAM
    std::ostringstream ost;
#else
    std::ostrstream ost;
#endif

    static const char * s_ball_pos_info_str[] = BALL_POS_INFO_STRINGS;
    BallPosInfo info = M_stadium.ballPosInfo();

    ost << "(ok check_ball " << M_stadium.time() << " " ;

    ost << s_ball_pos_info_str[info] << ")";

    ost << std::ends;

#ifdef HAVE_SSTREAM
    sendMsg( MSG_BOARD, ost.str().c_str() );
#else
    ost << std::ends;
    sendMsg( MSG_BOARD, ost.str() );
    ost.freeze( false );
#endif

    return true;
}

bool
Monitor::coach_change_player_type( const char * command )
{
    char teamname[128];
    int unum, player_type;
    if ( std::sscanf( command,
                      " ( change_player_type %127s %d %d ) ",
                      teamname, &unum, &player_type ) != 3 )
    {
        sendMsg( MSG_BOARD, "(error illegal_command_form)" );
        return false;
    }

    const Team * team = NULL;
    if ( M_stadium.teamLeft().name() == teamname )
    {
        team = &( M_stadium.teamLeft() );
    }
    else if ( M_stadium.teamRight().name() == teamname )
    {
        team = &( M_stadium.teamRight() );
    }

    if ( team == NULL )
    {
        sendMsg( MSG_BOARD, "(warning no_team_found)" );
        return false;
    }

    if ( player_type < 0
         || player_type >= PlayerParam::instance().playerTypes() )
    {
        sendMsg( MSG_BOARD, "(error out_of_range_player_type)" );
        return false;
    }

    const Player * player = NULL;
    for ( int i = 0; i < team->size(); ++i )
    {
        const Player * p = team->player( i );
        if ( p && p->unum() == unum )
        {
            player = p;
            break;
        }
    }

    if ( player == NULL )
    {
        sendMsg( MSG_BOARD, "(warning no_such_player)" );
        return false;
    }

    M_stadium.substitute( player, player_type );

    char buf[64];
    snprintf( buf, 64,
              "(ok change_player_type %s %d %d)",
              teamname, unum, player_type );

    sendMsg( MSG_BOARD, buf );

    return true;
}
