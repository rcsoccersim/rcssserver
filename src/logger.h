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

#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <memory>

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

    struct Impl;
    std::unique_ptr< Impl > M_impl;

    std::unique_ptr< rcss::InitObserverLogger > M_init_observer;
    std::unique_ptr< rcss::ObserverLogger > M_observer;

    const Stadium & M_stadium;

public:
    explicit
    Logger( Stadium & stadium );
    ~Logger();

    bool setSenders();

    bool open();
    void close();

    std::ostream & kawayLog();

private:
    bool openGameLog();
    bool openTextLog();
    bool openKawayLog();

    void closeGameLog();
    void closeTextLog();
    void closeKawayLog();

    void renameLogs();

public:

    void flush();

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

    void writeTimes( const std::chrono::system_clock::time_point & old_time,
                     const std::chrono::system_clock::time_point & new_time );
    void writeProfile( const std::chrono::system_clock::time_point & start_time,
                       const std::chrono::system_clock::time_point & end_time,
                       const std::string & str );
};


#endif
