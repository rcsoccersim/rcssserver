// -*-c++-*-

/***************************************************************************
                                logger.h
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


#ifndef RCSSSERVER_LOGGER_H
#define RCSSSERVER_LOGGER_H

#include "types.h"

#include <rcssbase/gzip/gzfstream.hpp>

#include <iostream>
#include <fstream>
#include <string>


class Player;
class Coach;
class OnlineCoach;
class Stadium;
class XPMHolder;
struct timeval;

namespace rcss {
class InitObserverLogger;
class ObserverLogger;
namespace clang {
class Msg;
}
}

class Logger {
private:

    static const std::string DEF_TEXT_NAME;
    static const std::string DEF_TEXT_SUFFIX;
    static const std::string DEF_GAME_NAME;
    static const std::string DEF_GAME_SUFFIX;
    static const std::string DEF_KAWAY_NAME;
    static const std::string DEF_KAWAY_SUFFIX;

    rcss::InitObserverLogger * M_init_observer;
    rcss::ObserverLogger * M_observer;

    const Stadium & M_stadium;

    std::string M_game_log_name;
    std::string M_text_log_name;
    std::string M_kaway_log_name;

    std::ostream * M_game_log;
    std::ostream * M_text_log;
    std::ofstream M_kaway_log;  //!< file for keepaway log


    PlayMode M_playmode;
    std::string M_team_l_name;
    std::string M_team_r_name;
    int M_team_l_score;
    int M_team_r_score;
    int M_team_l_pen_taken;
    int M_team_r_pen_taken;

public:
    explicit
    Logger( Stadium & stadium );
    ~Logger();

    bool setSenders();

    bool open();
    void close();

private:
    bool openGameLog();
    bool openTextLog();
    bool openKawayLog();

    void closeGameLog();
    void closeTextLog();
    void closeKawayLog();

    void renameLogs();

public:

    bool isGameLogOpen() const
      {
          return ( M_game_log && M_game_log->good() );
      }

    bool isTextLogOpen() const
      {
          return ( M_text_log && M_text_log->good() );
      }

    std::ostream & kawayLog()
      {
          return M_kaway_log;
      }

    void flush()
      {
          if ( M_game_log ) M_game_log->flush();
          if ( M_text_log ) M_text_log->flush();
          M_kaway_log.flush();
      }

    void writeToGameLog( const char * str,
                         const std::streamsize n );

    void writeMsgToGameLog( const BoardType board_type,
                            const char * msg,
                            const bool force = false );

    /*!
      \brief write current status
     */
    void writeGameLog();
private:
    void writeGameLogImpl();

    //void writeGameLogV1();
    //void writeGameLogV2();
    //void writeGameLogV3();
    //void writeGameLogV4();

public:

    void writeTeamGraphic( const Side side,
                           const unsigned int x,
                           const unsigned int y );

    void writeTextLog( const char * message,
                       const TextLogFlag flag );

    void writePlayerLog( const Player & player,
                         const char * message,
                         const TextLogFlag flag );
    void writeCoachLog( const char * message,
                        const TextLogFlag flag );
    void writeOnlineCoachLog( const OnlineCoach & coach,
                              const char * message,
                              const TextLogFlag flag );

    void writeRefereeAudio( const char * msg );
    void writePlayerAudio( const Player & player,
                           const char * msg );
    void writeCoachAudio( const Coach & coach,
                          const char * msg );
    void writeCoachStdAudio( const OnlineCoach & coach,
                             const rcss::clang::Msg & msg );

    void writeTimes( const timeval &,
                     const timeval & );
    void writeProfile( const timeval &,
                       const timeval &,
                       const char * );
};


#endif
