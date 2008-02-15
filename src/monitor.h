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
 *   Foundation; either version 2 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#ifndef MONITOR_H
#define MONITOR_H

#include "remoteclient.h"
#include "types.h"

#include <string>
#include <netinet/in.h>

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

public:
    Monitor( Stadium & stadium,
             const double & version );
    ~Monitor();

    void parseMsg( const char * msg,
                   const size_t & len )
      {
          char * str = const_cast< char * >( msg );
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

    void disable()
      {
          RemoteClient::close();
      }

    bool setSenders();

    void sendInit();
    void sendPlayMode();
    void sendScore();
    void sendTeamGraphic( const Side side,
                          const unsigned int x,
                          const unsigned int y,
                          const XPMHolder * holder );

    void sendShow();
    int sendMsg( const BoardType board,
                 const char * msg );

    bool parseCommand( const char * message );

    const
    double & version() const
      {
          return M_version;
      }

private:

    bool dispfoul( const char * command );
    bool dispplayer( const char * command );
    bool dispdiscard( const char * command );
    bool compression( const char * command );

    bool coach_change_mode( const char * command );
    bool coach_move( const char * command );
    bool coach_recover();
    bool coach_change_player_type( const char * command );
    bool coach_check_ball();
};


#endif
