// -*-c++-*-

/***************************************************************************
                            serializerplayerstdv13.cpp
                  Class for serializing data to std v13 players
                             -------------------
    begin                : 2008-10-28
    copyright            : (C) 2008 by The RoboCup Soccer Server
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

#include "serializerplayerstdv13.h"

#include "player.h"

namespace rcss {

SerializerPlayerStdv13::SerializerPlayerStdv13( const SerializerCommon::Ptr common )
    : SerializerPlayerStdv8( common )
{

}

SerializerPlayerStdv13::~SerializerPlayerStdv13()
{

}


void
SerializerPlayerStdv13::serializeVisualPlayer( std::ostream & strm,
                                               const Player & player,
                                               const std::string & name,
                                               const double & dist,
                                               const int dir ) const
{
    strm << " (" << name << ' ' << dist << ' ' << dir;
    if ( player.isTackling() )
    {
        strm << " t";
    }
    else if ( player.kicked() )
    {
        strm << " k";
    }
    strm << ')';
}


void
SerializerPlayerStdv13::serializeVisualPlayer( std::ostream & strm,
                                               const Player & player,
                                               const std::string & name,
                                               const double & dist,
                                               const int dir,
                                               const int point_dir ) const
{
    strm << " (" << name << ' ' << dist << ' ' << dir
         << ' ' << point_dir;
    if ( player.isTackling() )
    {
        strm << " t";
    }
    else if ( player.kicked() )
    {
        strm << " k";
    }
    strm << ')';
}


void
SerializerPlayerStdv13::serializeVisualPlayer( std::ostream & strm,
                                               const Player & player,
                                               const std::string & name,
                                               const double & dist,
                                               const int dir,
                                               const double & dist_chg,
                                               const double & dir_chg,
                                               const int body_dir,
                                               const int head_dir ) const
{
    strm << " (" << name << ' ' << dist << ' ' << dir
         << ' ' << dist_chg << ' ' << dir_chg
         << ' ' << body_dir << ' ' << head_dir;
    if ( player.isTackling() )
    {
        strm << " t";
    }
    else if ( player.kicked() )
    {
        strm << " k";
    }
    strm << ')';
}


void
SerializerPlayerStdv13::serializeVisualPlayer( std::ostream & strm,
                                               const Player & player,
                                               const std::string & name,
                                               const double & dist,
                                               const int dir,
                                               const double & dist_chg,
                                               const double & dir_chg,
                                               const int body_dir,
                                               const int head_dir,
                                               const int point_dir ) const
{
    strm << " (" << name << ' ' << dist << ' ' << dir
         << ' ' << dist_chg << ' ' << dir_chg
         << ' ' << body_dir << ' ' << head_dir
         << ' ' << point_dir;
    if ( player.isTackling() )
    {
        strm << " t";
    }
    else if ( player.kicked() )
    {
        strm << " k";
    }
    strm << ')';
}


void
SerializerPlayerStdv13::serializeBodyStamina( std::ostream & strm,
                                              const double & stamina,
                                              const double & effort,
                                              const double & stamina_capacity ) const
{
    strm << " (stamina "
         << stamina << ' '
         << effort << ' '
         << stamina_capacity << ')';
}

void
SerializerPlayerStdv13::serializeFSPlayerStamina( std::ostream & strm,
                                                  const double & stamina,
                                                  const double & effort,
                                                  const double & recovery,
                                                  const double & stamina_capacity ) const
{
    strm << " (stamina " << stamina
         << ' ' << effort
         << ' ' << recovery
         << ' ' << stamina_capacity
         << ')';
}

void
SerializerPlayerStdv13::serializeFSPlayerState( std::ostream & strm,
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
}


const
SerializerPlayer::Ptr
SerializerPlayerStdv13::create()
{
    SerializerCommon::Creator cre;
    if ( ! SerializerCommon::factory().getCreator( cre, 13 ) )
    {
        return SerializerPlayer::Ptr();
    }

    SerializerPlayer::Ptr ptr( new SerializerPlayerStdv13( cre() ) );
    return ptr;
}

namespace {
RegHolder v13 = SerializerPlayer::factory().autoReg( &SerializerPlayerStdv13::create, 13 );
}

}
