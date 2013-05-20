// -*-c++-*-

/***************************************************************************
                            serializerplayerstdv14.cpp
                  Class for serializing data to std v14 players
                             -------------------
    begin                : 2009-10-26
    copyright            : (C) 2009 by The RoboCup Soccer Server
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

#include "serializerplayerstdv14.h"

#include "player.h"

namespace rcss {

SerializerPlayerStdv14::SerializerPlayerStdv14( const SerializerCommon::Ptr common )
    : SerializerPlayerStdv13( common )
{

}

SerializerPlayerStdv14::~SerializerPlayerStdv14()
{

}

void
SerializerPlayerStdv14::serializeFoul( std::ostream & strm,
                                       const Player & self ) const
{
    strm << " (foul "
         << " (charged " << self.foulCycles() << ')';

    if ( self.hasRedCard() )
    {
        strm << " (card red)";
    }
    else if ( self.hasYellowCard() )
    {
        strm << " (card yellow)";

    }
    else
    {
        strm << " (card none)";
    }

    strm << ')';
}

void
SerializerPlayerStdv14::serializeFSPlayerBegin( std::ostream & strm,
                                                const char side,
                                                const int unum,
                                                const bool goalie,
                                                const int type,
                                                const double & x,
                                                const double & y,
                                                const double & vel_x,
                                                const double & vel_y,
                                                const double & body_dir,
                                                const double & neck_dir ) const

{
    strm << " ((p " << side
         << ' ' << unum;

    if ( goalie )
    {
        strm << " g";
    }

    strm << ' ' << type // hetro player type
         << ") " << x
         << ' ' << y
         << ' ' << vel_x
         << ' ' << vel_y
         << ' ' << body_dir
         << ' ' << neck_dir;
}

void
SerializerPlayerStdv14::serializeFSPlayerState( std::ostream & strm,
                                                const Player & player ) const
{
    if ( player.isTackling() )
    {
        strm << " t";
    }
    else if ( player.kicked() )
    {
        strm << " k";
    }
    else if ( player.foulCycles() > 0 )
    {
        strm << " f";
    }

    if ( player.hasRedCard() )
    {
        strm << " r";
    }
    else if ( player.hasYellowCard() )
    {
        strm << " y";
    }
}


const
SerializerPlayer::Ptr
SerializerPlayerStdv14::create()
{
    SerializerCommon::Creator cre;
    if ( ! SerializerCommon::factory().getCreator( cre, 14 ) )
    {
        return SerializerPlayer::Ptr();
    }

    SerializerPlayer::Ptr ptr( new SerializerPlayerStdv14( cre() ) );
    return ptr;
}

namespace {
RegHolder v14 = SerializerPlayer::factory().autoReg( &SerializerPlayerStdv14::create, 14 );
RegHolder v15 = SerializerPlayer::factory().autoReg( &SerializerPlayerStdv14::create, 15 );
//RegHolder v16 = SerializerPlayer::factory().autoReg( &SerializerPlayerStdv14::create, 16 );
}

}
