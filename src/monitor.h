// -*-c++-*-

/***************************************************************************
                                 monitor.h
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

#ifndef RCSSSERVER_MONITOR_H
#define RCSSSERVER_MONITOR_H

#include "remoteclient.h"
#include "types.h"

#include <set>
#include <string>

class Stadium;
class XPMHolder;

namespace rcss {
class InitObserverMonitor;
class ObserverMonitor;
}

class Monitor
    : public RemoteClient {
protected:

    rcss::InitObserverMonitor * M_init_observer;
    rcss::ObserverMonitor * M_observer;

    Stadium & M_stadium;
    double M_version;

    PlayMode M_playmode;
    std::string M_team_l_name;
    std::string M_team_r_name;
    int M_team_l_score;
    int M_team_r_score;
    int M_team_l_pen_taken;
    int M_team_r_pen_taken;

    typedef std::pair< unsigned int, unsigned int > GraphKey;
    std::set< GraphKey > M_left_sent_graphics;
    std::set< GraphKey > M_right_sent_graphics;

public:
    Monitor( Stadium & stadium,
             const double & version );
    ~Monitor();

    void disable()
      {
          RemoteClient::close();
      }

    void parseMsg( char * msg,
                   const size_t & len );
    bool parseCommand( const char * message );

    bool setSenders();

    void sendInit();
    void sendPlayMode();
    void sendScore();

    void sendTeamGraphics();

    void sendShow();
    int sendMsg( const BoardType board,
                 const char * msg );

    const
    double & version() const
      {
          return M_version;
      }

private:

    bool dispfoul( const char * command );
    bool dispplayer( const char * command );
    bool dispdiscard( const char * command );
    bool dispcard( const char * command );

    bool compression( const char * command );

    bool coach_change_mode( const char * command );
    bool coach_move( const char * command );
    bool coach_recover();
    bool coach_change_player_type( const char * command );
    bool coach_check_ball();
};


#endif
