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

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>

#include <rcss/clang/clangmsg.h>
#include <rcss/gzip/gzfstream.hpp>

#include <sstream>

namespace {
const std::string DEF_TEXT_NAME = "incomplete";
const std::string DEF_TEXT_SUFFIX = ".rcl";
const std::string DEF_GAME_NAME = "incomplete";
const std::string DEF_GAME_SUFFIX = ".rcg";
const std::string DEF_KAWAY_NAME = "incomplete";
const std::string DEF_KAWAY_SUFFIX = ".kwy";
}

struct Logger::Impl {

    std::unique_ptr< rcss::InitObserverLogger > init_observer_;
    std::unique_ptr< rcss::ObserverLogger > observer_;

    std::string game_log_filepath_;
    std::string text_log_filepath_;
    std::string kaway_log_filepath_;

    std::ostream * game_log_;
    std::ostream * text_log_;
    std::ofstream kaway_log_;  //!< file for keepaway log


    Impl()
        : init_observer_( new rcss::InitObserverLogger ),
          observer_( new rcss::ObserverLogger ),
          game_log_( nullptr ),
          text_log_( nullptr )
    {

    }

    void flush()
    {
        if ( game_log_ ) game_log_->flush();
        if ( text_log_ ) text_log_->flush();
        kaway_log_.flush();
    }

    void closeGameLog()
    {
         if ( game_log_ )
         {
             init_observer_->sendTail();

             game_log_->flush();
             delete game_log_;
             game_log_ = nullptr;
         }
    }

    void closeTextLog()
    {
        if ( text_log_ )
        {
            text_log_->flush();
            delete text_log_;
            text_log_ = nullptr;
        }
    }

    void closeKeepawayLog()
    {
        if ( kaway_log_.is_open() )
        {
            kaway_log_.flush();
            kaway_log_.close();
        }
    }

    bool isGameLogOpen() const
    {
        return ( game_log_
                 && game_log_->good() );
    }

    bool isTextLogOpen() const
    {
        return ( text_log_
                 && text_log_->good() );
    }

    bool isKeepawayLogOpen() const
    {
        return kaway_log_.is_open();
    }
};

Logger &
Logger::instance()
{
    static Logger s_instance;
    return s_instance;
}

Logger::Logger()
    : M_impl( new Impl() )
{

}

Logger::~Logger()
{

}

bool
Logger::setSenders( const Stadium & stadium )
{
    if ( ! M_impl->isGameLogOpen() )
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
        rcss::InitSenderLogger::Params init_params( *M_impl->game_log_,
                                                    *this,
                                                    ser,
                                                    stadium );
        rcss::InitSenderLogger::Creator init_cre;
        if ( ! rcss::InitSenderLogger::factory().getCreator( init_cre, log_version ) )
        {
            std::cerr << "No InitSenderLogger::Creator v" << log_version << std::endl;
            return false;
        }
        M_impl->init_observer_->setInitSender( init_cre( init_params ) );
    }

    // disp sender
    {
        rcss::DispSenderLogger::Params disp_params( *M_impl->game_log_,
                                                    *this,
                                                    ser,
                                                    stadium );
        rcss::DispSenderLogger::Creator disp_cre;
        if ( ! rcss::DispSenderLogger::factory().getCreator( disp_cre, log_version ) )
        {
            std::cerr << "No DispSenderLogger::Creator v" << log_version << std::endl;
            return false;
        }
        M_impl->observer_->setDispSender( disp_cre( disp_params ) );
    }

    return true;
}

bool
Logger::open( const Stadium & stadium )
{
    if ( ServerParam::instance().gameLogging() )
    {
        if ( ! openGameLog( stadium ) )
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
Logger::close( const Stadium & stadium )
{
    renameLogs( stadium );

    closeGameLog();
    closeTextLog();
    closeKawayLog();
}

bool
Logger::openGameLog( const Stadium & stadium )
{
    // create the log directory & file path string
    try
    {
        boost::filesystem::path game_log( ServerParam::instance().gameLogDir() );
        if ( ! boost::filesystem::exists( game_log )
             && ! boost::filesystem::create_directories( game_log ) )
        {
            std::cerr << __FILE__ << ": " << __LINE__
                      << ": can't create game log directory " << game_log << std::endl;
            return false;
        }

        if ( ServerParam::instance().gameLogFixed() )
        {
            game_log /= ServerParam::instance().gameLogFixedName() + DEF_GAME_SUFFIX;
        }
        else
        {
            game_log /= DEF_GAME_NAME + DEF_GAME_SUFFIX;
        }

        M_impl->game_log_filepath_ = game_log.string();
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
        M_impl->game_log_filepath_ += ".gz";
        rcss::gz::gzofstream * f
            = new rcss::gz::gzofstream( M_impl->game_log_filepath_.c_str(),
                                        ServerParam::instance().gameLogCompression() );
        M_impl->game_log_ = f;
    }
    else
#endif
    {
        std::ofstream * f
            = new std::ofstream( M_impl->game_log_filepath_.c_str(),
                                 ( std::ofstream::binary
                                   | std::ofstream::out
                                   | std::ofstream::trunc ) );
        M_impl->game_log_ = f;
    }

    if ( ! M_impl->isGameLogOpen() )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << ": can't open the game log file " << M_impl->game_log_filepath_ << std::endl;
        return false;
    }

    // write header and configration parameters
    if ( ! setSenders( stadium ) )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << ": can't set senders " << M_impl->game_log_filepath_ << std::endl;
        return false;
    }

    M_impl->init_observer_->sendHeader();
    M_impl->init_observer_->sendServerParams();
    M_impl->init_observer_->sendPlayerParams();
    M_impl->init_observer_->sendPlayerTypes();
    M_impl->game_log_->flush();

    return true;
}

bool
Logger::openTextLog()
{
    // create the log directory & file path string
    try
    {
        boost::filesystem::path text_log( ServerParam::instance().textLogDir() );
        if ( ! boost::filesystem::exists( text_log )
             && ! boost::filesystem::create_directories( text_log ) )
        {
            std::cerr << __FILE__ << ": " << __LINE__
                      << ": can't create text log directory " << text_log << std::endl;
            return false;
        }

        if ( ServerParam::instance().textLogFixed() )
        {
            text_log /= ServerParam::instance().textLogFixedName() + DEF_TEXT_SUFFIX;
        }
        else
        {
            text_log /= DEF_TEXT_NAME + DEF_TEXT_SUFFIX;
        }

        M_impl->text_log_filepath_ = text_log.string();
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
        M_impl->text_log_filepath_ += std::string ( ".gz" );
        rcss::gz::gzofstream * f
            = new rcss::gz::gzofstream( M_impl->text_log_filepath_.c_str(),
                                        ServerParam::instance().textLogCompression() );
        M_impl->text_log_ = f;
    }
    else
#endif
    {
        std::ofstream * f = new std::ofstream( M_impl->text_log_filepath_.c_str() );
        M_impl->text_log_ = f;
    }

    if ( ! M_impl->isTextLogOpen() )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << ": can't open the text log file '" << M_impl->text_log_filepath_
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
        boost::filesystem::path kaway_log( ServerParam::instance().kawayLogDir() );
        if ( ! boost::filesystem::exists( kaway_log )
             && ! boost::filesystem::create_directories( kaway_log ) )
        {
            std::cerr << __FILE__ << ": " << __LINE__
                      << ": can't create keepaway log directory " << kaway_log << std::endl;
            return false;
        }

        if ( ServerParam::instance().kawayLogFixed() )
        {
            kaway_log /= ServerParam::instance().kawayLogFixedName() + DEF_KAWAY_SUFFIX;
        }
        else
        {
            kaway_log /= DEF_KAWAY_NAME + DEF_KAWAY_SUFFIX;
        }

        M_impl->kaway_log_filepath_ = kaway_log.string();
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
    M_impl->kaway_log_.open( M_impl->kaway_log_filepath_.c_str() );

    if ( ! M_impl->kaway_log_.is_open() )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << ": can't open keepaway_log_file " << M_impl->kaway_log_filepath_
                  << std::endl;
        return false;
    }

    return true;
}


void
Logger::closeGameLog()
{
    M_impl->closeGameLog();
}

void
Logger::closeTextLog()
{
    M_impl->closeTextLog();
}

void
Logger::closeKawayLog()
{
    M_impl->closeKeepawayLog();
}

void
Logger::flush()
{
    M_impl->flush();
}

void
Logger::renameLogs( const Stadium & stadium )
{
    if ( ! M_impl->isGameLogOpen()
         && ! M_impl->isTextLogOpen()
         && ! M_impl->isKeepawayLogOpen() )
    {
        return;
    }

    // add penalty to logfile when penalties are score or was draw and one team won
    bool bAddPenaltyScore = ( stadium.teamRight().point() == stadium.teamLeft().point()
                              && stadium.teamLeft().penaltyTaken() > 0
                              && stadium.teamRight().penaltyTaken() > 0 );

    char time_str[32];
    const std::time_t time = stadium.getStartTime();
    const std::tm * lt = std::localtime( &time );
    std::strftime( time_str, 32,
                   ServerParam::instance().logDateFormat().c_str(),
                   lt );

    std::string team_name_score( "" );

    if ( stadium.teamLeft().enabled() )
    {
        team_name_score += stadium.teamLeft().name();
        team_name_score += "_";
        if ( ! stadium.teamLeft().olcoach()->name().empty() )
        {
            team_name_score += stadium.teamLeft().olcoach()->name();
            team_name_score += "_";
        }
        team_name_score += std::to_string( stadium.teamLeft().point() );
        if ( bAddPenaltyScore )
        {
            team_name_score += "_";
            team_name_score += std::to_string( stadium.teamLeft().penaltyPoint() );
            team_name_score += ( stadium.teamLeft().penaltyWon() ? "w" : "" );
        }
    }
    else
    {
        team_name_score += "null";
    }

    team_name_score += "-vs-";
    if ( stadium.teamRight().enabled() )
    {
        team_name_score += stadium.teamRight().name();
        team_name_score += "_";
        if ( ! stadium.teamRight().olcoach()->name().empty() )
        {
            team_name_score += stadium.teamRight().olcoach()->name();
            team_name_score += "_";
        }
        team_name_score += std::to_string( stadium.teamRight().point() );
        if ( bAddPenaltyScore )
        {
            team_name_score += "_";
            team_name_score += std::to_string( stadium.teamRight().penaltyPoint() );
            team_name_score += ( stadium.teamRight().penaltyWon() ? "w" : "" );
        }
    }
    else
    {
        team_name_score += "null";
    }

    //
    // write the result to the game log
    //
    if ( M_impl->isGameLogOpen() )
    {
        char time_stamp[32];
        const std::time_t time = stadium.getStartTime();
        const struct tm * lt = std::localtime( &time );
        std::strftime( time_stamp, 32,
                       "%Y%m%d%H%M",
                       lt );
        char msg[256];
        snprintf( msg, 256, "(result %s %s)", time_stamp, team_name_score.c_str() );
        writeMsgToGameLog( MSG_BOARD, msg, true );
    }

    //
    // rename text log
    //
    if ( M_impl->isTextLogOpen()
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
        newname += DEF_TEXT_SUFFIX;
        if ( ServerParam::instance().textLogCompression() > 0 )
        {
            newname += ".gz";
        }

        closeTextLog();

        if ( std::rename( M_impl->text_log_filepath_.c_str(),
                          newname.c_str() ) )
        {
            std::cerr << __FILE__ << ": " << __LINE__
                      << ": error renaming " << M_impl->text_log_filepath_ << std::endl;
        }
        M_impl->text_log_filepath_ = newname;
    }

    //
    // rename game log
    //
    if ( M_impl->isGameLogOpen()
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
        newname += DEF_GAME_SUFFIX;
        if ( ServerParam::instance().gameLogCompression() > 0 )
        {
            newname += ".gz";
        }

        closeGameLog();

        if ( std::rename( M_impl->game_log_filepath_.c_str(),
                          newname.c_str() ) )
        {
            std::cerr << __FILE__ << ": " << __LINE__
                      << ": error renaming " << M_impl->game_log_filepath_ << std::endl;
        }
        M_impl->game_log_filepath_ = newname;
    }

    //
    // rename keepaway log
    //
    if ( M_impl->kaway_log_.is_open()
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
        newname += DEF_KAWAY_SUFFIX;

        closeKawayLog();

        if( std::rename( M_impl->kaway_log_filepath_.c_str(),
                         newname.c_str() ) )
        {
            std::cerr << __FILE__ << ": " << __LINE__
                      << ": error renaming " << M_impl->kaway_log_filepath_ << std::endl;
        }
    }

}

// void
// Logger::writeToGameLog( const char * str,
//                         const std::streamsize n )
// {
//     if ( M_impl->game_log_ )
//     {
//         M_impl->game_log_->write( str, n );
//     }
// }

void
Logger::writeMsgToGameLog( const BoardType board_type,
                           const char * msg,
                           const bool force )
{
    if ( ! M_impl->isGameLogOpen() )
    {
        return;
    }

    if ( force
         || ServerParam::instance().recordMessages()
         || ServerParam::instance().gameLogVersion() == REC_OLD_VERSION )
    {
        M_impl->observer_->sendMsg( board_type, msg );
    }

//     if ( ServerParam::instance().gameLogVersion() >= REC_VERSION_4 )
//     {
//         if ( force
//              || ServerParam::instance().recordMessages() )
//         {
//             std::string str( msg );
//             std::replace( str.begin(), str.end(), '\n', ' ' );

//             *M_game_log << "(msg " << stadium.time()
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
Logger::writeGameLog( const Stadium & stadium )
{
    /* TH - 2-NOV-2000 */
    static bool wrote_final_cycle = false;

    if ( ! M_impl->isGameLogOpen() )
    {
        return;
    }


    if ( stadium.playmode() != PM_BeforeKickOff
         && stadium.playmode() != PM_TimeOver )
    {
        writeGameLogImpl( stadium );
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
    else if ( stadium.playmode() == PM_TimeOver
              && ! wrote_final_cycle )
    {
        writeGameLogImpl( stadium );
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
Logger::writeGameLogImpl( const Stadium & stadium )
{
    static PlayMode pm = PM_Null;
    static std::string team_l_name, team_r_name;
    static int team_l_score = 0, team_r_score = 0;
    static int team_l_pen_taken = 0, team_r_pen_taken = 0;

   // if playmode has changed wirte playmode
    if ( pm != stadium.playmode() )
    {
        pm = stadium.playmode();
        M_impl->init_observer_->sendPlayMode();
    }

    // if teams or score has changed, write teams and score
    if ( team_l_score != stadium.teamLeft().point()
         || team_r_score != stadium.teamRight().point()
         || team_l_pen_taken != stadium.teamLeft().penaltyTaken()
         || team_r_pen_taken != stadium.teamRight().penaltyTaken()
         || stadium.teamLeft().name() != team_l_name
         || stadium.teamRight().name() != team_r_name
         )
    {
        team_l_name = stadium.teamLeft().name();
        team_r_name = stadium.teamRight().name();
        team_l_score = stadium.teamLeft().point();
        team_r_score = stadium.teamRight().point();
        team_l_pen_taken = stadium.teamLeft().penaltyTaken();
        team_r_pen_taken = stadium.teamRight().penaltyTaken();

        M_impl->init_observer_->sendTeam();
    }


    M_impl->observer_->sendShow();
}

#if 0
// This method has no longer be used.
void
Logger::writeGameLogV1()
{
    dispinfo_t disp;

    disp.mode = htons( SHOW_MODE );

    disp.body.show.time = htons( stadium.time() );

    disp.body.show.pmode = static_cast< char >( stadium.playmode() );

    disp.body.show.pos[0].x = htons( (Int16)rint( ( stadium.ball().pos().x * SHOWINFO_SCALE ) ) );
    disp.body.show.pos[0].y = htons( (Int16)rint( ( stadium.ball().pos().y * SHOWINFO_SCALE ) ) );

    const Stadium::PlayerCont & players = stadium.players();
    for ( int i = 0; i < MAX_PLAYER * 2; ++i )
    {
        disp.body.show.pos[i+1].enable = htons( players[i]->state() );
        disp.body.show.pos[i+1].x = htons( (Int16)rint( players[i]->pos().x * SHOWINFO_SCALE ) );
        disp.body.show.pos[i+1].y = htons( (Int16)rint( players[i]->pos().y * SHOWINFO_SCALE ) );
        disp.body.show.pos[i+1].angle = htons( (Int16)Rad2IDegRound( players[i]->angleBodyCommitted() ) );
        disp.body.show.pos[i+1].side = htons( players[i]->side() );
        disp.body.show.pos[i+1].unum = htons( players[i]->unum() );
    }

    if ( ! stadium.teamLeft().name().empty() )
    {
        std::strncpy( disp.body.show.team[0].name,
                      stadium.teamLeft().name().c_str(),
                      std::min( stadium.teamLeft().name().length() + 1, size_t( 16 ) ) );
        disp.body.show.team[0].score = htons( stadium.teamLeft().point() );
    }
    else
    {
        std::memset( &disp.body.show.team[0].name,
                     0,
                     sizeof( disp.body.show.team[0].name ) );
    }

    if ( ! stadium.teamRight().name().empty() )
    {
        std::strncpy( disp.body.show.team[1].name,
                      stadium.teamRight().name().c_str(),
                      std::min( stadium.teamRight().name().length() + 1, size_t( 16 ) ) );
        disp.body.show.team[1].score = htons( stadium.teamRight().point() );
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

    show.time = htons( stadium.time() );

    show.pmode = static_cast< char >( stadium.playmode() );

    show.pos[0].x = htons( (Int16)rint( ( stadium.ball().pos().x * SHOWINFO_SCALE ) ) );
    show.pos[0].y = htons( (Int16)rint( ( stadium.ball().pos().y * SHOWINFO_SCALE ) ) );

    const Stadium::PlayerCont & players = stadium.players();
    for ( int i = 0; i < MAX_PLAYER * 2; ++i )
    {
        show.pos[i+1].enable = htons( players[i]->state() );
        show.pos[i+1].x = htons( (Int16)rint( players[i]->pos().x * SHOWINFO_SCALE ) );
        show.pos[i+1].y = htons( (Int16)rint( players[i]->pos().y * SHOWINFO_SCALE ) );
        show.pos[i+1].angle = htons( (Int16)Rad2IDegRound( players[i]->angleBodyCommitted() ) );
        show.pos[i+1].side = htons( players[i]->side() );
        show.pos[i+1].unum = htons( players[i]->unum() );
    }

    if ( ! stadium.teamLeft().name().empty() )
    {
        std::strncpy( show.team[0].name,
                      stadium.teamLeft().name().c_str(),
                      std::min( stadium.teamLeft().name().length() + 1, size_t( 16 ) ) );
        show.team[0].score = htons( stadium.teamLeft().point() );
    }
    else
    {
        std::memset( &show.team[0].name,
                     0,
                     sizeof( show.team[0].name ) );
        show.team[0].score = htons( 0 );
    }

    if ( ! stadium.teamRight().name().empty() )
    {
        std::strncpy( show.team[1].name,
                      stadium.teamRight().name().c_str(),
                      std::min( stadium.teamRight().name().length() + 1, size_t( 16 ) ) );
        show.team[1].score = htons( stadium.teamRight().point() );
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
    if ( pmode != stadium.playmode() )
    {
        pmode = stadium.playmode();

        char pm = static_cast< char >( pmode );

        mode = htons( PM_MODE );
        writeToGameLog( reinterpret_cast< const char * >( &mode ),
                        sizeof( mode ) );
        writeToGameLog( reinterpret_cast< const char * >( &pm ),
                        sizeof( char ) );
    }

    // if teams or score has changed, write teams and score
    if ( stadium.teamLeft().point() != score_l
         || stadium.teamRight().point() != score_r
         || stadium.teamLeft().name() != teams[ 0 ].name
         || stadium.teamRight().name() != teams[ 1 ].name )
    {
        score_l = stadium.teamLeft().point();
        score_r = stadium.teamLeft().point();

        std::strncpy( teams[ 0 ].name,
                      stadium.teamLeft().name().c_str(),
                      std::min( stadium.teamLeft().name().length() + 1, size_t( 16 ) ) );
        teams[ 0 ].score = htons( score_l );

        std::strncpy( teams[ 1 ].name,
                      stadium.teamRight().name().c_str(),
                      std::min( stadium.teamRight().name().length() + 1, size_t( 16 ) ) );
        teams[ 1 ].score = htons( score_r );

        mode = htons( TEAM_MODE );
        writeToGameLog( reinterpret_cast< const char * >( &mode ),
                        sizeof( mode ) );
        writeToGameLog( reinterpret_cast< const char * >( teams ),
                        sizeof( team_t ) * 2 );

    }

    // write positional data
    short_showinfo_t2 show;

    show.time = htons( stadium.time() );

    show.ball.x = htonl( (Int32)rint( ( stadium.ball().pos().x * SHOWINFO_SCALE2 ) ) );
    show.ball.y = htonl( (Int32)rint( ( stadium.ball().pos().y * SHOWINFO_SCALE2 ) ) );
    show.ball.deltax = htonl( (Int32)rint( ( stadium.ball().vel().x * SHOWINFO_SCALE2 ) ) );
    show.ball.deltay = htonl( (Int32)rint( ( stadium.ball().vel().y * SHOWINFO_SCALE2) ) );

    const Stadium::PlayerCont & players = stadium.players();
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
    if ( pm != stadium.playmode() )
    {
        pm = stadium.playmode();

        os << "(playmode " << stadium.time()
           << ' ' << playmode_strings[pm] << ")\n";
    }

    // if teams or score has changed, write teams and score
    if ( team_l_score != stadium.teamLeft().point()
         || team_r_score != stadium.teamRight().point()
         || team_l_pen_taken != stadium.teamLeft().penaltyTaken()
         || team_r_pen_taken != stadium.teamRight().penaltyTaken()
         || stadium.teamLeft().name() != team_l_name
         || stadium.teamRight().name() != team_r_name
         )
    {
        team_l_name = stadium.teamLeft().name();
        team_r_name = stadium.teamRight().name();
        team_l_score = stadium.teamLeft().point();
        team_r_score = stadium.teamRight().point();
        team_l_pen_taken = stadium.teamLeft().penaltyTaken();
        team_r_pen_taken = stadium.teamRight().penaltyTaken();

        os << "(team " << stadium.time()
           << ' ' << ( team_l_name.empty() ? "null" : team_l_name.c_str() )
           << ' ' << ( team_r_name.empty() ? "null" : team_r_name.c_str() )
           << ' ' << team_l_score
           << ' ' << team_r_score;
        if ( team_l_pen_taken > 0 || team_r_pen_taken > 0 )
        {
            os << ' ' << stadium.teamLeft().penaltyPoint()
               << ' ' << team_l_pen_taken - stadium.teamLeft().penaltyPoint()
               << ' ' << stadium.teamRight().penaltyPoint()
               << ' ' << team_r_pen_taken - stadium.teamRight().penaltyPoint();
        }
        os << ")\n";
    }

    os << "(show " << stadium.time();
    os << " (" << BALL_NAME_SHORT
       << ' ' << Quantize( stadium.ball().pos().x, prec )
       << ' ' << Quantize( stadium.ball().pos().y, prec )
       << ' ' << Quantize( stadium.ball().vel().x, prec )
       << ' ' << Quantize( stadium.ball().vel().y, prec )
       << ')';

    for ( Stadium::PlayerCont::const_reference p : stadium.players() )
    {
        os << " (";
        os << "(" << SideStr( p->side() )
           << ' ' << p->unum()
           << ')';
        os << ' ' << p->playerTypeId()
           << ' ' << std::hex << std::showbase
           << p->state() // include goalie flag
           << std::dec << std::noshowbase;

        os << ' ' << Quantize( p->pos().x, prec )
           << ' ' << Quantize( p->pos().y, prec )
           << ' ' << Quantize( p->vel().x, prec )
           << ' ' << Quantize( p->vel().y, prec )
           << ' ' << Quantize( Rad2Deg( p->angleBodyCommitted() ), dprec )
           << ' ' << Quantize( Rad2Deg( p->angleNeckCommitted() ), dprec );
        if ( p->arm().isPointing() )
        {
            os << ' ' << Quantize( p->arm().dest().getX(), prec )
               << ' ' << Quantize( p->arm().dest().getY(), prec );
//             rcss::geom::Vector2D arm_dest;
//             if ( p->arm().getRelDest( rcss::geom::Vector2D( p->pos().x,
//                                                                p->pos().y ),
//                                          0.0,
//                                          arm_dest ) )
//             {
//                 os << ' ' << Quantize( arm_dest.getMag(), prec )
//                    << ' ' << Quantize( Rad2Deg( arm_dest.getHead() ), dprec );
//             }
        }

        os << " (v "
           << ( p->highquality() ? "h " : "l " )
           << Quantize( Rad2Deg( p->visibleAngle() ), dprec )
           << ')';
        os << " (s "
           << p->stamina() << ' '
           << p->effort() << ' '
           << p->recovery() << ')';
        if ( p->isEnabled()
             && p->getFocusTarget() )
        {
            os << " (f " << SideStr( p->getFocusTarget()->side() )
               << ' ' << p->getFocusTarget()->unum()
               << ')';
        }
        os << " (c "
           << p->kickCount()
           << ' ' << p->dashCount()
           << ' ' << p->turnCount()
           << ' ' << p->catchCount()
           << ' ' << p->moveCount()
           << ' ' << p->turnNeckCount()
           << ' ' << p->changeViewCount()
           << ' ' << p->sayCount()
           << ' ' << p->tackleCount()
           << ' ' << p->arm().getCounter()
           << ' ' << p->attentiontoCount()
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
    if ( ! M_impl->isGameLogOpen() )
    {
        return;
    }

    M_impl->observer_->sendTeamGraphic( side, x, y );

//     const XPMHolder * xpm = ( side == LEFT
//                               ? stadium.teamLeft().teamGraphic( x, y )
//                               : side == RIGHT
//                               ? stadium.teamRight().teamGraphic( x, y )
//                               : static_cast< const XPMHolder * >( 0 ) );
//     if ( ! xpm )
//     {
//         return;
//     }

//     std::ostringstream data;
//     data << "(team_graphic_"
//          << ( side == LEFT ? "l" : "r" )
//          << " (" << x << " " << y << " "
//          << *xpm << ")";

//     std::string msg = data.str();

//     writeMsgToGameLog( MSG_BOARD, msg.c_str(), true );
}


void
Logger::writeTextLog( const Stadium & stadium,
                      const char * message,
                      const TextLogFlag flag )
{
    if ( flag == RECV || flag == LOG_TEXT )
    {
        if ( M_impl->isTextLogOpen() )
        {
            *M_impl->text_log_ << stadium.time()
                               << ',' << stadium.stoppageTime()
                               << "\t" << message << '\n';
        }
    }

    if ( ( flag == RECV || flag == SUBS )
         && M_impl->isGameLogOpen()
         && ServerParam::instance().recordMessages()
         && stadium.playmode() != PM_TimeOver )
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

//         for ( Stadium::MonitorCont::iterator i = stadium.monitors().begin();
//               i != stadium.monitors().end();
//               ++i )
//         {
//             (*i)->sendMsg( LOG_BOARD, buf );
//         }
//     }

}

void
Logger::writePlayerLog( const Stadium & stadium,
                        const Player & player,
                        const char * message,
                        const TextLogFlag flag )
{
    if ( M_impl->isTextLogOpen()
         || ( flag == RECV && M_impl->isGameLogOpen() )
         || ( flag == RECV && ServerParam::instance().sendComms() ) )
    {
        char tmp[MaxMesg];
        snprintf( tmp, MaxMesg,
                  "%s %s_%d: %s",
                  ( flag == SEND ? "Send" : "Recv" ),
                  player.team()->name().c_str(),
                  player.unum(),
                  message );
        writeTextLog( stadium, tmp, flag );
    }
}

void
Logger::writeCoachLog( const Stadium & stadium,
                       const char * message,
                       const TextLogFlag flag )
{
    if ( M_impl->isTextLogOpen()
         || ( flag == RECV && M_impl->isGameLogOpen() )
         || ( flag == RECV && ServerParam::instance().sendComms() ) )
    {
        char tmp[MaxMesg];
        snprintf( tmp, MaxMesg,
                  "%s Coach: %s",
                  ( flag == SEND ? "Send" : "Recv" ),
                  message ) ;
        writeTextLog( stadium, tmp, flag );
    }
}

void
Logger::writeOnlineCoachLog( const Stadium & stadium,
                             const OnlineCoach & coach,
                             const char * message,
                             const TextLogFlag flag )
{
    if ( M_impl->isTextLogOpen()
         || ( flag == RECV && M_impl->isGameLogOpen() )
         || ( flag == RECV && ServerParam::instance().sendComms() ) )
    {
        char tmp[MaxMesg];
        snprintf( tmp, MaxMesg,
                  "%s %s_Coach: %s",
                  ( flag == SEND ? "Send" : "Recv" ),
                  coach.team().name().c_str(),
                  message );
        writeTextLog( stadium, tmp, flag );
    }
}

void
Logger::writeRefereeAudio( const Stadium & stadium,
                           const char * msg )
{
    if ( M_impl->isTextLogOpen()
         || ( M_impl->isGameLogOpen()
              && stadium.playmode() != PM_BeforeKickOff
              && stadium.playmode() != PM_TimeOver ) )
    {
        char buf[max_message_length_for_display];
        snprintf( buf,
                  max_message_length_for_display,
                  "(%s %s)",
                  REFEREE_NAME, msg );

        if ( M_impl->isTextLogOpen() )
        {
            writeTextLog( stadium, buf, LOG_TEXT );
        }

        if ( M_impl->isGameLogOpen() )
        {
            writeMsgToGameLog( MSG_BOARD, buf );
        }
    }
}

void
Logger::writePlayerAudio( const Stadium & stadium,
                          const Player & player,
                          const char * msg )
{

    if ( M_impl->isGameLogOpen()
         && ServerParam::instance().recordMessages()
         && stadium.playmode() != PM_BeforeKickOff
         && stadium.playmode() != PM_TimeOver )
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
Logger::writeCoachAudio( const Stadium & stadium,
                         const Coach & coach,
                         const char * msg )
{
    if ( M_impl->isGameLogOpen()
         && ServerParam::instance().recordMessages()
         && stadium.playmode() != PM_BeforeKickOff
         && stadium.playmode() != PM_TimeOver )
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
Logger::writeCoachStdAudio( const Stadium & stadium,
                            const OnlineCoach & coach,
                            const rcss::clang::Msg & msg )
{
    if ( M_impl->isGameLogOpen()
         && ServerParam::instance().recordMessages()
         && stadium.playmode() != PM_BeforeKickOff
         && stadium.playmode() != PM_TimeOver )
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
Logger::writeKeepawayHeader( const int keepers,
                             const int takers )
{
    if ( M_impl->kaway_log_ )
    {
        M_impl->kaway_log_ << "# Keepers: " << keepers << '\n'
                           << "# Takers:  " << takers << '\n'
                           << "# Region:  " << ServerParam::instance().keepAwayLength()
                           << " x " << ServerParam::instance().keepAwayWidth()
                           << '\n'
                           << "#\n"
                           << "# Description of Fields:\n"
                           << "# 1) Episode number\n"
                           << "# 2) Start time in simulator steps (100ms)\n"
                           << "# 3) End time in simulator steps (100ms)\n"
                           << "# 4) Duration in simulator steps (100ms)\n"
                           << "# 5) (o)ut of bounds / (t)aken away\n"
                           << "#\n"
                           << std::flush;
    }
}

void
Logger::writeKeepawayLog( const Stadium & stadium,
                          const int episode,
                          const int time,
                          const char * end_cond )
{
    if ( M_impl->kaway_log_ )
    {
        M_impl->kaway_log_ << episode << "\t"
                           << time << "\t"
                           << stadium.time() << "\t"
                           << stadium.time() - time << "\t"
                           << end_cond
                           << std::endl;
    }
}

void
Logger::writeTimes( const Stadium & stadium,
                    const std::chrono::system_clock::time_point & old_time,
                    const std::chrono::system_clock::time_point & new_time )
{
    if ( M_impl->isTextLogOpen()
         && ServerParam::instance().logTimes() )
    {
        const std::chrono::nanoseconds nano_diff = std::chrono::duration_cast< std::chrono::nanoseconds >( new_time - old_time );
        const double diff = nano_diff.count() * 0.001 * 0.001;

        *M_impl->text_log_ << stadium.time()
                           << ',' << stadium.stoppageTime()
                           << "\tCYCLE_TIMES: " << diff << '\n';
    }
}

void
Logger::writeProfile( const Stadium & stadium,
                      const std::chrono::system_clock::time_point & start_time,
                      const std::chrono::system_clock::time_point & end_time,
                      const std::string & str )
{
    if ( M_impl->isTextLogOpen()
         && ServerParam::instance().profile() )
    {
        const std::chrono::nanoseconds nano_diff = std::chrono::duration_cast< std::chrono::nanoseconds >( end_time - start_time );
        const double diff = nano_diff.count() * 0.001 * 0.001;

        *M_impl->text_log_ << stadium.time()
                           << ',' << stadium.stoppageTime()
                           << "\t" << str << ": " << diff << '\n';
    }
}
