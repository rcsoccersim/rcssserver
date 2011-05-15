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
 *   Foundation; either version 3 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "logger.h"

#include "player.h"
#include "clangmsg.h"
#include "coach.h"
#include "stadium.h"
#include "heteroplayer.h"
#include "monitor.h"
#include "playerparam.h"
#include "serverparam.h"
#include "team.h"
#include "xpmholder.h"

#include "dispsender.h"
#include "initsenderlogger.h"
#include "serializermonitor.h"

#include "serializercommonstdv8.h"

#include <boost/lexical_cast.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>

#include <sstream>

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
// #ifdef HAVE_NETINET_IN_H
// #include <netinet/in.h>
// #endif
// #ifdef HAVE_WINSOCK2_H
// #include <winsock2.h>
// #endif


#if defined(BOOST_FILESYSTEM_VERSION) && BOOST_FILESYSTEM_VERSION > 2

#define BOOST_FS_ABSOLUTE absolute
#define BOOST_FS_FILE_STRING string
#define BOOST_FS_DIRECTORY_STRING string
#define BOOST_FS_PARENT_PATH parent_path

#else

#define BOOST_FS_ABSOLUTE complete

#  ifdef BOOST_FILESYSTEM_NO_DEPRECATED
#    define BOOST_FS_FILE_STRING file_string
#    define BOOST_FS_DIRECTORY_STRING directory_string
#    define BOOST_FS_PARENT_PATH parent_path
#  else
#    define BOOST_FS_FILE_STRING native_file_string
#    define BOOST_FS_DIRECTORY_STRING native_directory_string
#    define BOOST_FS_PARENT_PATH branch_path
#  endif

#endif

const std::string Logger::DEF_TEXT_NAME = "incomplete";
const std::string Logger::DEF_TEXT_SUFFIX = ".rcl";
const std::string Logger::DEF_GAME_NAME = "incomplete";
const std::string Logger::DEF_GAME_SUFFIX = ".rcg";
const std::string Logger::DEF_KAWAY_NAME = "incomplete";
const std::string Logger::DEF_KAWAY_SUFFIX = ".kwy";


Logger::Logger( Stadium & stadium )
    : M_init_observer( new rcss::InitObserverLogger )
    , M_observer( new rcss::ObserverLogger )
    , M_stadium( stadium )
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
    this->close();
}

bool
Logger::setSenders()
{
    if ( ! isGameLogOpen() )
    {
        return true;
    }

    int log_version = ServerParam::instance().gameLogVersion();
    int monitor_version = log_version - 1;
    if ( log_version == REC_OLD_VERSION )
    {
        monitor_version = 1;
    }

    rcss::SerializerMonitor::Creator ser_cre;
    if ( ! rcss::SerializerMonitor::factory().getCreator( ser_cre, monitor_version ) )
    {
        std::cerr << "No SerializerMonitor::Creator v" << monitor_version << std::endl;
        return false;
    }

    const rcss::SerializerMonitor::Ptr ser = ser_cre();
    if ( ! ser )
    {
        std::cerr << "No SerializerMonitor v" << monitor_version << std::endl;
        return false;
    }

    //
    // init sender
    //
    {
        rcss::InitSenderLogger::Params init_params( *M_game_log,
                                                    *this,
                                                    ser,
                                                    M_stadium );
        rcss::InitSenderLogger::Creator init_cre;
        if ( ! rcss::InitSenderLogger::factory().getCreator( init_cre, log_version ) )
        {
            std::cerr << "No InitSenderLogger::Creator v" << log_version << std::endl;
            return false;
        }
        M_init_observer->setInitSender( init_cre( init_params ) );
    }

    // disp sender
    {
        rcss::DispSenderLogger::Params disp_params( *M_game_log,
                                                    *this,
                                                    ser,
                                                    M_stadium );
        rcss::DispSenderLogger::Creator disp_cre;
        if ( ! rcss::DispSenderLogger::factory().getCreator( disp_cre, log_version ) )
        {
            std::cerr << "No DispSenderLogger::Creator v" << log_version << std::endl;
            return false;
        }
        M_observer->setDispSender( disp_cre( disp_params ) );
    }

    return true;
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

    if ( ServerParam::instance().keepAwayMode()
         && ServerParam::instance().kawayLogging() )
    {
        if ( ! openKawayLog() )
        {
            return false;
        }
    }

    return true;
}

void
Logger::close()
{
    renameLogs();

    closeGameLog();
    closeTextLog();
    closeKawayLog();
}

bool
Logger::openGameLog()
{
    // create the log directory & file path string
    try
    {
        boost::filesystem::path game_log( ServerParam::instance().gameLogDir()
#if defined(BOOST_FILESYSTEM_VERSION) && BOOST_FILESYSTEM_VERSION == 2
#  ifndef BOOST_FILESYSTEM_NO_DEPRECATED
                                          , &boost::filesystem::native
#  endif
#endif
                                          );
        if ( ! boost::filesystem::exists( game_log )
             && ! boost::filesystem::create_directories( game_log ) )
        {
            std::cerr << __FILE__ << ": " << __LINE__
                      << ": can't create game log directory '"
                      << game_log.BOOST_FS_DIRECTORY_STRING() << "'" << std::endl;
            return false;
        }

        if ( ServerParam::instance().gameLogFixed() )
        {
            game_log /= ServerParam::instance().gameLogFixedName() + Logger::DEF_GAME_SUFFIX;
        }
        else
        {
            game_log /= Logger::DEF_GAME_NAME + Logger::DEF_GAME_SUFFIX;
        }

        M_game_log_name = game_log.BOOST_FS_FILE_STRING();
    }
    catch ( std::exception & e )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << " Exception caught! " << e.what()
                  << "\nCould not create game log directory '"
                  << ServerParam::instance().gameLogDir()
                  << "'" << std::endl;
        return false;
    }

    // open the output file stream
#ifndef HAVE_LIBZ
    if ( ServerParam::instance().gameLogCompression() > 0 )
    {
        std::cerr << "No zlib. Ignored game log compression." << std::endl;
    }
#endif

#ifdef HAVE_LIBZ
    if ( ServerParam::instance().gameLogCompression() > 0 )
    {
        M_game_log_name += ".gz";
        rcss::gz::gzofstream * f
            = new rcss::gz::gzofstream( M_game_log_name.c_str(),
                                        ServerParam::instance().gameLogCompression() );
        M_game_log = f;
    }
    else
#endif
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
                  << ": can't open the game log file " << M_game_log_name << std::endl;
        return false;
    }

    // write header and configration parameters
    if ( ! setSenders() )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << ": can't set senders " << M_game_log_name << std::endl;
        return false;
    }

    M_init_observer->sendHeader();
    M_init_observer->sendServerParams();
    M_init_observer->sendPlayerParams();
    M_init_observer->sendPlayerTypes();
    M_game_log->flush();

    return true;
}

bool
Logger::openTextLog()
{
    // create the log directory & file path string
    try
    {
        boost::filesystem::path text_log( ServerParam::instance().textLogDir()
#if defined(BOOST_FILESYSTEM_VERSION) && BOOST_FILESYSTEM_VERSION == 2
#  ifndef BOOST_FILESYSTEM_NO_DEPRECATED
                                          , &boost::filesystem::native
#  endif
#endif
                                          );
        if ( ! boost::filesystem::exists( text_log )
             && ! boost::filesystem::create_directories( text_log ) )
        {
            std::cerr << __FILE__ << ": " << __LINE__
                      << ": can't create text log directory '"
                      << text_log.BOOST_FS_DIRECTORY_STRING() << "'" << std::endl;
            return false;
        }

        if ( ServerParam::instance().textLogFixed() )
        {
            text_log /= ServerParam::instance().textLogFixedName() + Logger::DEF_TEXT_SUFFIX;
        }
        else
        {
            text_log /= Logger::DEF_TEXT_NAME + Logger::DEF_TEXT_SUFFIX;
        }

        M_text_log_name = text_log.BOOST_FS_FILE_STRING();
    }
    catch ( std::exception & e )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << " Exception caught! " << e.what()
                  << "\nCould not create text log directory '"
                  << ServerParam::instance().textLogDir()
                  << "'" << std::endl;
        return false;
    }

    // open the output file stream
#ifndef HAVE_LIBZ
    if ( ServerParam::instance().textLogCompression() > 0 )
    {
        std::cerr << "No zlib. Ignored text log compression." << std::endl;
    }
#endif

#ifdef HAVE_LIBZ
    if ( ServerParam::instance().textLogCompression() > 0 )
    {
        M_text_log_name += std::string ( ".gz" );
        rcss::gz::gzofstream * f
            = new rcss::gz::gzofstream( M_text_log_name.c_str(),
                                        ServerParam::instance().textLogCompression() );
        M_text_log = f;
    }
    else
#endif
    {
        std::ofstream * f = new std::ofstream( M_text_log_name.c_str() );
        M_text_log = f;
    }

    if ( ! isTextLogOpen() )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << ": can't open the text log file '" << M_text_log_name
                  << "'" << std::endl;
        return false;
    }

    return true;
}


bool
Logger::openKawayLog()
{
    // create the log directory & file path string
    try
    {
        boost::filesystem::path kaway_log( ServerParam::instance().kawayLogDir()
#if defined(BOOST_FILESYSTEM_VERSION) && BOOST_FILESYSTEM_VERSION == 2
#  ifndef BOOST_FILESYSTEM_NO_DEPRECATED
                                           , &boost::filesystem::native
#  endif
#endif
                                           );
        if ( ! boost::filesystem::exists( kaway_log )
             && ! boost::filesystem::create_directories( kaway_log ) )
        {
            std::cerr << __FILE__ << ": " << __LINE__
                      << ": can't create keepaway log directory '"
                      << kaway_log.BOOST_FS_DIRECTORY_STRING() << "'" << std::endl;
            return false;
        }

        if ( ServerParam::instance().kawayLogFixed() )
        {
            kaway_log /= ServerParam::instance().kawayLogFixedName() + Logger::DEF_KAWAY_SUFFIX;
        }
        else
        {
            kaway_log /= Logger::DEF_KAWAY_NAME + Logger::DEF_KAWAY_SUFFIX;
        }

        M_kaway_log_name = kaway_log.BOOST_FS_FILE_STRING();
    }
    catch ( std::exception & e )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << " Exception caught! " << e.what()
                  << "\nCould not create keepaway log directory '"
                  << ServerParam::instance().kawayLogDir()
                  << "'" << std::endl;
        return false;
    }

    // open the output file stream
    M_kaway_log.open( M_kaway_log_name.c_str() );

    if ( ! M_kaway_log.is_open() )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << ": can't open keepaway_log_file " << M_kaway_log_name
                  << std::endl;
        return false;
    }

    return true;
}


void
Logger::closeGameLog()
{
    if ( M_game_log )
    {
        M_game_log->flush();
        delete M_game_log;
        M_game_log =  static_cast< std::ostream * >( 0 );
    }
}

void
Logger::closeTextLog()
{
    if ( M_text_log )
    {
        M_text_log->flush();
        delete M_text_log;
        M_text_log =  static_cast< std::ostream * >( 0 );
    }
}

void
Logger::closeKawayLog()
{
    if ( M_kaway_log.is_open() )
    {
        M_kaway_log.flush();
        M_kaway_log.close();
    }
}

void
Logger::renameLogs()
{
    if ( ! isGameLogOpen()
         && ! isTextLogOpen()
         && ! M_kaway_log.is_open() )
    {
        return;
    }

    // add penalty to logfile when penalties are score or was draw and one team won
    bool bAddPenaltyScore = ( M_stadium.teamRight().point() == M_stadium.teamLeft().point()
                              && M_stadium.teamLeft().penaltyTaken() > 0
                              && M_stadium.teamRight().penaltyTaken() > 0 );

    char time_str[32];
    std::strftime( time_str, 32,
                   ServerParam::instance().logDateFormat().c_str(),
                   &( M_stadium.realTime() ) );

    std::string team_name_score( "" );

    if ( M_stadium.teamLeft().enabled() )
    {
        team_name_score += M_stadium.teamLeft().name();
        team_name_score += "_";
        if ( ! M_stadium.teamLeft().olcoach()->name().empty() )
        {
            team_name_score += M_stadium.teamLeft().olcoach()->name();
            team_name_score += "_";
        }
        team_name_score += boost::lexical_cast< std::string >( M_stadium.teamLeft().point() );
        if ( bAddPenaltyScore )
        {
            team_name_score += "_";
            team_name_score += boost::lexical_cast< std::string >( M_stadium.teamLeft().penaltyPoint() );
            team_name_score += ( M_stadium.teamLeft().penaltyWon() ? "w" : "" );
        }
    }
    else
    {
        team_name_score += "null";
    }

    team_name_score += "-vs-";
    if ( M_stadium.teamRight().enabled() )
    {
        team_name_score += M_stadium.teamRight().name();
        team_name_score += "_";
        if ( ! M_stadium.teamRight().olcoach()->name().empty() )
        {
            team_name_score += M_stadium.teamRight().olcoach()->name();
            team_name_score += "_";
        }
        team_name_score += boost::lexical_cast< std::string >( M_stadium.teamRight().point() );
        if ( bAddPenaltyScore )
        {
            team_name_score += "_";
            team_name_score += boost::lexical_cast< std::string >( M_stadium.teamRight().penaltyPoint() );
            team_name_score += ( M_stadium.teamRight().penaltyWon() ? "w" : "" );
        }
    }
    else
    {
        team_name_score += "null";
    }

    //
    // write result
    //
    if ( isGameLogOpen() )
    {
        char time_stamp[32];
        std::strftime( time_stamp, 32,
                       "%Y%m%d%H%M",
                       &( M_stadium.realTime() ) );
        char msg[256];
        snprintf( msg, 256, "(result %s %s)", time_stamp, team_name_score.c_str() );
        writeMsgToGameLog( MSG_BOARD, msg, true );
    }

    //
    // rename text log
    //
    if ( isTextLogOpen()
         && ! ServerParam::instance().textLogFixed() )
    {
        std::string newname = ServerParam::instance().textLogDir();
        if ( *newname.rbegin() != '/' )
        {
            newname += '/';
        }
        if ( ServerParam::instance().textLogDated() )
        {
            newname += time_str;
        }
        newname += team_name_score;
        newname += Logger::DEF_TEXT_SUFFIX;
        if ( ServerParam::instance().textLogCompression() > 0 )
        {
            newname += ".gz";
        }

        closeTextLog();

        if ( std::rename( M_text_log_name.c_str(),
                          newname.c_str() ) )
        {
            std::cerr << __FILE__ << ": " << __LINE__
                      << ": error renaming " << M_text_log_name << std::endl;
        }
        M_text_log_name = newname;
    }

    //
    // rename game log
    //
    if ( isGameLogOpen()
         && ! ServerParam::instance().gameLogFixed() )
    {
        std::string newname = ServerParam::instance().gameLogDir();
        if ( *newname.rbegin() != '/' )
        {
            newname += '/';
        }
        if ( ServerParam::instance().gameLogDated() )
        {
            newname += time_str;
        }
        newname += team_name_score;
        newname += Logger::DEF_GAME_SUFFIX;
        if ( ServerParam::instance().gameLogCompression() > 0 )
        {
            newname += ".gz";
        }

        closeGameLog();

        if ( std::rename( M_game_log_name.c_str(),
                          newname.c_str() ) )
        {
            std::cerr << __FILE__ << ": " << __LINE__
                      << ": error renaming " << M_game_log_name << std::endl;
        }
        M_game_log_name = newname;
    }

    //
    // rename keepaway log
    //
    if ( M_kaway_log.is_open()
         && ! ServerParam::instance().kawayLogFixed() )
    {
        std::string newname = ServerParam::instance().kawayLogDir();
        if ( *newname.rbegin() != '/' )
        {
            newname += '/';
        }
        if ( ServerParam::instance().kawayLogDated() )
        {
            newname += time_str;
        }
        newname += team_name_score;
        newname += Logger::DEF_KAWAY_SUFFIX;

        closeKawayLog();

        if( std::rename( M_kaway_log_name.c_str(),
                         newname.c_str() ) )
        {
            std::cerr << __FILE__ << ": " << __LINE__
                      << ": error renaming " << M_kaway_log_name << std::endl;
        }
    }

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
                           const char * msg,
                           const bool force )
{
    if ( ! isGameLogOpen() )
    {
        return;
    }

    if ( force
         || ServerParam::instance().recordMessages()
         || ServerParam::instance().gameLogVersion() == REC_OLD_VERSION )
    {
        M_observer->sendMsg( board_type, msg );
    }

//     if ( ServerParam::instance().gameLogVersion() >= REC_VERSION_4 )
//     {
//         if ( force
//              || ServerParam::instance().recordMessages() )
//         {
//             std::string str( msg );
//             std::replace( str.begin(), str.end(), '\n', ' ' );

//             *M_game_log << "(msg " << M_stadium.time()
//                         << ' ' << board_type << " \"" << str << "\")\n";
//         }
//     }
//     else if ( ServerParam::instance().gameLogVersion() != REC_OLD_VERSION )
//     {
//         if ( force
//              || ServerParam::instance().recordMessages() )
//         {
//             Int16 mode = htons( MSG_MODE );
//             writeToGameLog( reinterpret_cast< const char * >( &mode ),
//                             sizeof( mode ) );
//             Int16 board = htons( board_type );
//             writeToGameLog( reinterpret_cast< const char * >( &board ),
//                             sizeof( board ) );
//             // calculate the string length and write it

//             const Int16 max_len = std::min( Int16( std::strlen( msg ) ), Int16( 2048 ) );
//             Int16 len = 1;
//             while ( (msg[len-1] != '\0') && (len < max_len) )
//             {
//                 ++len;
//             }
//             /* pfr 1/7/00 Need to put length in network byte order */
//             Int16 nlen = htons( len );
//             writeToGameLog( reinterpret_cast< const char* >( &nlen ),
//                             sizeof( nlen ) );
//             // write the message
//             writeToGameLog( msg, len );
//         }
//     }
//     else
//     {
//         dispinfo_t disp;
//         disp.mode = htons( MSG_MODE );
//         disp.body.msg.board = htons( board_type );
//         std::strncpy( disp.body.msg.message,
//                       msg,
//                       std::min( sizeof( disp.body.msg.message ),
//                                 std::strlen( msg ) ) );
//         writeToGameLog( reinterpret_cast< const char * >( &disp ),
//                         sizeof( dispinfo_t ) );
//     }
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
        writeGameLogImpl();
//         switch ( ServerParam::instance().gameLogVersion() ) {
//         case REC_VERSION_4:
//             writeGameLogV4();
//             break;
//         case REC_VERSION_3:
//             writeGameLogV3();
//             break;
//         case REC_VERSION_2:
//             writeGameLogV2();
//             break;
//         case REC_OLD_VERSION:
//             writeGameLogV1();
//             break;
//         default:
//             break;
//         }
    }
    else if ( M_stadium.playmode() == PM_TimeOver
              && ! wrote_final_cycle )
    {
        writeGameLogImpl();
//         switch ( ServerParam::instance().gameLogVersion() ) {
//         case REC_VERSION_4:
//             writeGameLogV4();
//             break;
//         case REC_VERSION_3:
//             writeGameLogV3();
//             break;
//         case REC_VERSION_2:
//             writeGameLogV2();
//             break;
//         case REC_OLD_VERSION:
//             writeGameLogV1();
//             break;
//         default:
//             break;
//         }
        wrote_final_cycle = true;
    }
}

void
Logger::writeGameLogImpl()
{
    static PlayMode pm = PM_Null;
    static std::string team_l_name, team_r_name;
    static int team_l_score = 0, team_r_score = 0;
    static int team_l_pen_taken = 0, team_r_pen_taken = 0;

   // if playmode has changed wirte playmode
    if ( pm != M_stadium.playmode() )
    {
        pm = M_stadium.playmode();
        M_init_observer->sendPlayMode();
    }

    // if teams or score has changed, write teams and score
    if ( team_l_score != M_stadium.teamLeft().point()
         || team_r_score != M_stadium.teamRight().point()
         || team_l_pen_taken != M_stadium.teamLeft().penaltyTaken()
         || team_r_pen_taken != M_stadium.teamRight().penaltyTaken()
         || M_stadium.teamLeft().name() != team_l_name
         || M_stadium.teamRight().name() != team_r_name
         )
    {
        team_l_name = M_stadium.teamLeft().name();
        team_r_name = M_stadium.teamRight().name();
        team_l_score = M_stadium.teamLeft().point();
        team_r_score = M_stadium.teamRight().point();
        team_l_pen_taken = M_stadium.teamLeft().penaltyTaken();
        team_r_pen_taken = M_stadium.teamRight().penaltyTaken();

        M_init_observer->sendTeam();
    }


    M_observer->sendShow();
}

#if 0
// This method has no longer be used.
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
        disp.body.show.pos[i+1].side = htons( players[i]->side() );
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

// This method has no longer be used.
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
        show.pos[i+1].side = htons( players[i]->side() );
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

// This method has no longer be used.
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

// This method has no longer be used.
void
Logger::writeGameLogV4()
{
    static const char * playmode_strings[] = PLAYMODE_STRINGS;

    static PlayMode pm = PM_Null;
    static std::string team_l_name, team_r_name;
    static int team_l_score = 0, team_r_score = 0;
    static int team_l_pen_taken = 0, team_r_pen_taken = 0;

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
         || team_r_score != M_stadium.teamRight().point()
         || team_l_pen_taken != M_stadium.teamLeft().penaltyTaken()
         || team_r_pen_taken != M_stadium.teamRight().penaltyTaken()
         || M_stadium.teamLeft().name() != team_l_name
         || M_stadium.teamRight().name() != team_r_name
         )
    {
        team_l_name = M_stadium.teamLeft().name();
        team_r_name = M_stadium.teamRight().name();
        team_l_score = M_stadium.teamLeft().point();
        team_r_score = M_stadium.teamRight().point();
        team_l_pen_taken = M_stadium.teamLeft().penaltyTaken();
        team_r_pen_taken = M_stadium.teamRight().penaltyTaken();

        os << "(team " << M_stadium.time()
           << ' ' << ( team_l_name.empty() ? "null" : team_l_name.c_str() )
           << ' ' << ( team_r_name.empty() ? "null" : team_r_name.c_str() )
           << ' ' << team_l_score
           << ' ' << team_r_score;
        if ( team_l_pen_taken > 0 || team_r_pen_taken > 0 )
        {
            os << ' ' << M_stadium.teamLeft().penaltyPoint()
               << ' ' << team_l_pen_taken - M_stadium.teamLeft().penaltyPoint()
               << ' ' << M_stadium.teamRight().penaltyPoint()
               << ' ' << team_r_pen_taken - M_stadium.teamRight().penaltyPoint();
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
        os << "(" << SideStr( (*p)->side() )
           << ' ' << (*p)->unum()
           << ')';
        os << ' ' << (*p)->playerTypeId()
           << ' ' << std::hex << std::showbase
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
            os << ' ' << Quantize( (*p)->arm().dest().getX(), prec )
               << ' ' << Quantize( (*p)->arm().dest().getY(), prec );
//             rcss::geom::Vector2D arm_dest;
//             if ( (*p)->arm().getRelDest( rcss::geom::Vector2D( (*p)->pos().x,
//                                                                (*p)->pos().y ),
//                                          0.0,
//                                          arm_dest ) )
//             {
//                 os << ' ' << Quantize( arm_dest.getMag(), prec )
//                    << ' ' << Quantize( Rad2Deg( arm_dest.getHead() ), dprec );
//             }
        }

        os << " (v "
           << ( (*p)->highquality() ? "h " : "l " )
           << Quantize( Rad2Deg( (*p)->visibleAngle() ), dprec )
           << ')';
        os << " (s "
           << (*p)->stamina() << ' '
           << (*p)->effort() << ' '
           << (*p)->recovery() << ')';
        if ( (*p)->isEnabled()
             && (*p)->getFocusTarget() != NULL )
        {
            os << " (f " << SideStr( (*p)->getFocusTarget()->side() )
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

    os << ")\n";
}
#endif

void
Logger::writeTeamGraphic( const Side side,
                          const unsigned int x,
                          const unsigned int y )
{
    if ( ! isGameLogOpen() )
    {
        return;
    }

    M_observer->sendTeamGraphic( side, x, y );

//     const XPMHolder * xpm = ( side == LEFT
//                               ? M_stadium.teamLeft().teamGraphic( x, y )
//                               : side == RIGHT
//                               ? M_stadium.teamRight().teamGraphic( x, y )
//                               : static_cast< const XPMHolder * >( 0 ) );
//     if ( ! xpm )
//     {
//         return;
//     }

// #ifdef HAVE_SSTREAM
//     std::ostringstream data;
// #else
//     std::ostrstream data;
// #endif
//     data << "(team_graphic_"
//          << ( side == LEFT ? "l" : "r" )
//          << " (" << x << " " << y << " "
//          << *xpm << ")";

// #ifndef HAVE_SSTREAM
//     data << std::ends;
// #endif

//     std::string msg = data.str();

// #ifndef HAVE_SSTREAM
//     data.freeze( false );
// #endif

//     writeMsgToGameLog( MSG_BOARD, msg.c_str(), true );
}


void
Logger::writeTextLog( const char * message,
                      const TextLogFlag flag )
{
    if ( flag == RECV || flag == LOG_TEXT )
    {
        if ( isTextLogOpen() )
        {
            *M_text_log << M_stadium.time()
                        << ',' << M_stadium.stoppageTime()
                        << "\t" << message << '\n';
        }
    }

    if ( ( flag == RECV || flag == SUBS )
         && isGameLogOpen()
         && ServerParam::instance().recordMessages()
         && M_stadium.playmode() != PM_TimeOver )
    {
        char buf[max_message_length_for_display];
        std::strncpy( buf, message, std::min( max_message_length_for_display,
                                              (int)std::strlen( message ) ) );

        writeMsgToGameLog( LOG_BOARD, buf );
    }

//     if ( ( flag == RECV || flag == SUBS )
//          && ServerParam::instance().sendComms() )
//     {
//         char buf[max_message_length_for_display];
//         std::strncpy( buf, message, std::min( max_message_length_for_display,
//                                               (int)std::strlen( message ) ) );

//         for ( Stadium::MonitorCont::iterator i = M_stadium.monitors().begin();
//               i != M_stadium.monitors().end();
//               ++i )
//         {
//             (*i)->sendMsg( LOG_BOARD, buf );
//         }
//     }

}

void
Logger::writePlayerLog( const Player & player,
                        const char * message,
                        const TextLogFlag flag )
{
    if ( isTextLogOpen()
         || ( flag == RECV && isGameLogOpen() )
         || ( flag == RECV && ServerParam::instance().sendComms() ) )
    {
        char tmp[MaxMesg];
        snprintf( tmp, MaxMesg,
                  "%s %s_%d: %s",
                  ( flag == SEND ? "Send" : "Recv" ),
                  player.team()->name().c_str(),
                  player.unum(),
                  message );
        writeTextLog( tmp, flag );
    }
}

void
Logger::writeCoachLog( const char * message,
                       const TextLogFlag flag )
{
    if ( isTextLogOpen()
         || ( flag == RECV && isGameLogOpen() )
         || ( flag == RECV && ServerParam::instance().sendComms() ) )
    {
        char tmp[MaxMesg];
        snprintf( tmp, MaxMesg,
                  "%s Coach: %s",
                  ( flag == SEND ? "Send" : "Recv" ),
                  message ) ;
        writeTextLog( tmp, flag );
    }
}

void
Logger::writeOnlineCoachLog( const OnlineCoach & coach,
                             const char * message,
                             const TextLogFlag flag )
{
    if ( isTextLogOpen()
         || ( flag == RECV && isGameLogOpen() )
         || ( flag == RECV && ServerParam::instance().sendComms() ) )
    {
        char tmp[MaxMesg];
        snprintf( tmp, MaxMesg,
                  "%s %s_Coach: %s",
                  ( flag == SEND ? "Send" : "Recv" ),
                  ( coach.side() == LEFT
                    ? M_stadium.teamLeft().name().c_str()
                    : M_stadium.teamRight().name().c_str() ),
                  message );
        writeTextLog( tmp, flag );
    }
}

void
Logger::writeRefereeAudio( const char * msg )
{
    if ( isTextLogOpen()
         || ( isGameLogOpen()
              && M_stadium.playmode() != PM_BeforeKickOff
              && M_stadium.playmode() != PM_TimeOver ) )
    {
        char buf[max_message_length_for_display];
        snprintf( buf,
                  max_message_length_for_display,
                  "(%s %s)",
                  REFEREE_NAME, msg );

        if ( isTextLogOpen() )
        {
            writeTextLog( buf, LOG_TEXT );
        }

        if ( isGameLogOpen() )
        {
            writeMsgToGameLog( MSG_BOARD, buf );
        }
    }
}

void
Logger::writePlayerAudio( const Player & player,
                          const char * msg )
{

    if ( isGameLogOpen()
         && ServerParam::instance().recordMessages()
         && M_stadium.playmode() != PM_BeforeKickOff
         && M_stadium.playmode() != PM_TimeOver )
    {
        char buf[max_message_length_for_display];
        snprintf( buf,
                  max_message_length_for_display,
                  "(%s %s)",
                  player.name().c_str(), msg );

        writeMsgToGameLog( MSG_BOARD, buf );
    }
}

void
Logger::writeCoachAudio( const Coach & coach,
                         const char * msg )
{
    if ( isGameLogOpen()
         && ServerParam::instance().recordMessages()
         && M_stadium.playmode() != PM_BeforeKickOff
         && M_stadium.playmode() != PM_TimeOver )
    {
        char buf[max_message_length_for_display];
        char format[40];
        snprintf( format, 40, "(%%s %%.%ds)",
                  max_message_length_for_display - (MaxMesg - MaxCoachMesg) );
        snprintf( buf,
                  max_message_length_for_display,
                  format,
                  ( coach.side() == RIGHT
                    ? OLCOACH_NAME_R
                    : coach.side() == LEFT
                    ? OLCOACH_NAME_L
                    : REFEREE_NAME ),
                  msg );

        writeMsgToGameLog( MSG_BOARD, buf );
    }
}

void
Logger::writeCoachStdAudio( const OnlineCoach & coach,
                            const rcss::clang::Msg & msg )
{
    if ( isGameLogOpen()
         && ServerParam::instance().recordMessages()
         && M_stadium.playmode() != PM_BeforeKickOff
         && M_stadium.playmode() != PM_TimeOver )
    {
        std::ostringstream coach_mess;

        coach_mess << msg.getTimeRecv() << " ";
        msg.print( coach_mess );

        char buf[max_message_length_for_display];
        char format[40];
        snprintf( format, 40, "(%%s %%.%ds)",
                  max_message_length_for_display - (MaxMesg - MaxCoachMesg));
        snprintf( buf,
                  max_message_length_for_display,
                  format,
                  (coach.side() == RIGHT) ? OLCOACH_NAME_R : OLCOACH_NAME_L,
                  coach_mess.str().c_str() );

        writeMsgToGameLog( MSG_BOARD, buf );
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

        *M_text_log << M_stadium.time()
                    << ',' << M_stadium.stoppageTime()
                    << "\tCYCLE_TIMES: " << diff << '\n';
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

        *M_text_log << M_stadium.time()
                    << ',' << M_stadium.stoppageTime()
                    << "\t" << str << ": " << diff << '\n';
    }
}
