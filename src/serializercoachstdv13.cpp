// -*-c++-*-

/***************************************************************************
                            serializercoachstdv13.cpp
               Class for serializing data to std v13 offline coaches
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

#include "serializercoachstdv13.h"

#include "player.h"

namespace rcss {

SerializerCoachStdv13::SerializerCoachStdv13( const SerializerCommon::Ptr common )
    : SerializerCoachStdv8( common )
{

}


SerializerCoachStdv13::~SerializerCoachStdv13()
{

}

void
SerializerCoachStdv13::serializeVisualPlayer( std::ostream & strm,
                                             const Player & player,
                                             const std::string & name,
                                             const PVector & pos,
                                             const PVector & vel,
                                             const int body,
                                             const int neck ) const
{
    strm << " (" << name
         << ' ' << pos.x << ' ' << pos.y
         << ' ' << vel.x << ' ' << vel.y
         << ' ' << body
         << ' ' << neck;
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
SerializerCoachStdv13::serializeVisualPlayer( std::ostream & strm,
                                             const Player & player,
                                             const std::string & name,
                                             const PVector & pos,
                                             const PVector & vel,
                                             const int body,
                                             const int neck,
                                             const int point_dir ) const
{
    strm << " (" << name
         << ' ' << pos.x << ' ' << pos.y
         << ' ' << vel.x << ' ' << vel.y
         << ' ' << body
         << ' ' << neck
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


const
SerializerCoach::Ptr
SerializerCoachStdv13::create()
{
    SerializerCommon::Creator cre;
    if ( ! SerializerCommon::factory().getCreator( cre, 13 ) )
    {
        return SerializerCoach::Ptr();
    }

    SerializerCoach::Ptr ptr( new SerializerCoachStdv13( cre() ) );
    return ptr;
}


namespace {
RegHolder v13 = SerializerCoach::factory().autoReg( &SerializerCoachStdv13::create, 13 );
}

}
