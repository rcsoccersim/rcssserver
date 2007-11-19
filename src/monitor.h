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

#ifndef _MONITOR_H_
#define _MONITOR_H_

#include "remoteclient.h"
#include "types.h"
#include <netinet/in.h>

class Stadium;

class Monitor
    : public RemoteClient
{
protected:
    Stadium & M_stadium;
    double M_version;
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
                  send( "(warning message_not_null_terminated)" );
              }
              str[ len ] = 0;
          }
          parseCommand( str );
      }

    void disable()
      {
          RemoteClient::close();
      }

    void sendInit();

    bool parseCommand( const char * message );


    int send( const char * msg )
      {
          if ( version() >= 2.0 )
          {
              dispinfo_t2 minfo;
              minfo.mode = htons( MSG_MODE );
              minfo.body.msg.board = htons( MSG_BOARD );
              std::strncpy( minfo.body.msg.message, msg, max_message_length_for_display );
              return RemoteClient::send( reinterpret_cast< char * >( &minfo ),
                                         sizeof( dispinfo_t2 ) );
          }
          else if ( version() >= 1.0 )
          {
              dispinfo_t minfo;
              minfo.mode = htons( MSG_MODE );
              minfo.body.msg.board = htons(MSG_BOARD);
              std::strncpy( minfo.body.msg.message, msg, max_message_length_for_display );
              return RemoteClient::send( reinterpret_cast< const char * >( &minfo ),
                                         sizeof( dispinfo_t ) );
          }

          return 0;
      }

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
