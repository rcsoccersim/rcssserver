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

#include <string>
#include <chrono>
#include <memory>

class Player;
class Coach;
class OnlineCoach;
class Stadium;

namespace rcss {
namespace clang {
class Msg;
}
}

class Logger {
private:

    struct Impl;
    std::unique_ptr< Impl > M_impl;

    Logger();

    Logger( const Logger & ) = delete;
    Logger & operator=( const Logger & ) = delete;

public:

    static Logger & instance();

    ~Logger();

    bool open( const Stadium & stadium );
    void close( const Stadium & stadium );

private:
    bool setSenders( const Stadium & stadium );

    bool openGameLog( const Stadium & stadium );
    bool openTextLog();
    bool openKawayLog();

    void renameLogs( const Stadium & stadium );
    void closeGameLog();
    void closeTextLog();
    void closeKawayLog();

public:

    void flush();

    void writeMsgToGameLog( const BoardType board_type,
                            const char * msg,
                            const bool force = false );

    /*!
      \brief write current status
     */
    void writeGameLog( const Stadium & stadium );
private:
    void writeGameLogImpl( const Stadium & stadium );

    // void writeToGameLog( const char * str,
    //                      const std::streamsize n );

    //void writeGameLogV1();
    //void writeGameLogV2();
    //void writeGameLogV3();
    //void writeGameLogV4();

public:

    void writeTeamGraphic( const Side side,
                           const unsigned int x,
                           const unsigned int y );

    void writeTextLog( const Stadium & stadium,
                       const char * message,
                       const TextLogFlag flag );

    void writePlayerLog( const Stadium & stadium,
                         const Player & player,
                         const char * message,
                         const TextLogFlag flag );
    void writeCoachLog( const Stadium & stadium,
                        const char * message,
                        const TextLogFlag flag );
    void writeOnlineCoachLog( const Stadium & stadium,
                              const OnlineCoach & coach,
                              const char * message,
                              const TextLogFlag flag );

    void writeRefereeAudio( const Stadium & stadium,
                            const char * msg );
    void writePlayerAudio( const Stadium & stadium,
                           const Player & player,
                           const char * msg );
    void writeCoachAudio( const Stadium & stadium,
                          const Coach & coach,
                          const char * msg );
    void writeCoachStdAudio( const Stadium & stadium,
                             const OnlineCoach & coach,
                             const rcss::clang::Msg & msg );

    void writeKeepawayHeader( const int keepers,
                              const int takers );
    void writeKeepawayLog( const Stadium & stadium,
                           const int episode,
                           const int time,
                           const char * end_cond );

    void writeTimes( const Stadium & stadium,
                     const std::chrono::system_clock::time_point & old_time,
                     const std::chrono::system_clock::time_point & new_time );
    void writeProfile( const Stadium & stadium,
                       const std::chrono::system_clock::time_point & start_time,
                       const std::chrono::system_clock::time_point & end_time,
                       const std::string & str );

};


#endif
