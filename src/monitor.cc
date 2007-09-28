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
 *   Foundation; either version 2 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "monitor.h"

#include "field.h"
#include "player.h"

Monitor::Monitor( Stadium & stadium,
                  const double & version )
    : M_stadium( stadium ),
      M_version ( version )
{
}

Monitor::~Monitor()
{
}

bool
Monitor::parseCommand( const char *message )
{
    if ( ! std::strcmp( message, "(dispbye)" ) )
	  {
        disable();
        return true;
    }
    else if ( ! std::strcmp( message, "(dispstart)" ) )
    {
        // kick off message
        Stadium::_Start( M_stadium );
    }
    else if ( ! std::strncmp( message, "(dispplayer", 11 ) )
    {
        // a player is given new position by the monitor
        int side, unum;
        int x, y, a;
        int n = std::sscanf( message, "(dispplayer %d %d %d %d %d)",
                             &side, &unum, &x, &y, &a );
        if ( n == 5 )
        {
            double real_x = x / SHOWINFO_SCALE;
            double real_y = y / SHOWINFO_SCALE;
            double angle = Deg2Rad( a );
            PVector vel( 0.0, 0.0 );
            M_stadium.movePlayer( (Side)side, unum,
                                  PVector( real_x, real_y ),
                                  &angle,
                                  &vel );
        }
    }
    else if ( ! std::strncmp( message, "(dispdiscard", 12 ) )
    {
        // a player is discarded by the monitor
        int side, unum;
        int n = std::sscanf( message,"(dispdiscard %d %d)", &side, &unum );
        if ( n == 2 )
        {
            M_stadium.discard_player( (Side)side, unum );
        }
    }
    else if ( ! std::strncmp( message, "(compression", 12 ) )
    {
        int level;
        int n = std::sscanf( message, "(compression %d)", &level );
        if ( n < 1 )
        {
            send( "(error illegal_command_form)" );
            return false;
        }
#ifdef HAVE_LIBZ
        if ( level > 9 )
        {
            send( "(error illegal_command_form)" );
            return false;
        }

#ifdef HAVE_SSTREAM
        std::ostringstream reply;
        reply << "(ok compression " << level << ")";
        send( reply.str().c_str() );
#else
        std::ostrstream reply;
        reply << "(ok compression " << level << ")" << std::ends;
        send( reply.str() );
        reply.freeze( false );
#endif
        setCompressionLevel( level );

#else
        send( "(warning compression_unsupported)" );
#endif
    }
    else
    {
        // foul or drop_ball
        int x, y, side ;
        int n = std::sscanf( message, "(dispfoul %d %d %d)", &x, &y, &side );

        if ( n == 3 )
        {
            double real_x = x / SHOWINFO_SCALE ;
            double real_y = y / SHOWINFO_SCALE ;
            if ( side == NEUTRAL )
            {
                M_stadium.referee_drop_ball( real_x, real_y, (Side)side );
            }
            else
            {
                M_stadium.referee_get_foul( real_x, real_y, (Side)side );
            }
        }
        else
        {
            send( "(error illegal_command_form)" );
            return false;
        }
    }
    return true;
}
