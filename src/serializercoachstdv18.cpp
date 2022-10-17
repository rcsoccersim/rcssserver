// -*-c++-*-

/***************************************************************************
                            SerializerCoachStdv18.cpp
               Class for serializing data to std v14 offline coaches
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

#include "serializercoachstdv18.h"

#include "player.h"

namespace rcss {

SerializerCoachStdv18::SerializerCoachStdv18( const SerializerCommon::Ptr common )
    : SerializerCoachStdv14( common )
{
}


SerializerCoachStdv18::~SerializerCoachStdv18()
{

}

void
SerializerCoachStdv18::serializeVisualPlayer( std::ostream & strm,
                                             const Player & player,
                                             const std::string & name,
                                             const PVector & pos,
                                             const PVector & vel,
                                             const int body,
                                             const int neck,
                                             const int focus_point_dir,
                                             const double focus_point_dist) const
{
    strm << " (" << name
         << ' ' << pos.x << ' ' << pos.y
         << ' ' << vel.x << ' ' << vel.y
         << ' ' << body
         << ' ' << neck
         << ' ' << focus_point_dir
         << ' ' << focus_point_dist;
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

    strm << ')';
}

void
SerializerCoachStdv18::serializeVisualPlayer( std::ostream & strm,
                                             const Player & player,
                                             const std::string & name,
                                             const PVector & pos,
                                             const PVector & vel,
                                             const int body,
                                             const int neck,
                                             const int focus_point_dir,
                                             const double focus_point_dist,
                                             const int point_dir ) const
{
    strm << " (" << name
         << ' ' << pos.x << ' ' << pos.y
         << ' ' << vel.x << ' ' << vel.y
         << ' ' << body
         << ' ' << neck
         << ' ' << focus_point_dir
         << ' ' << focus_point_dist
         << ' ' << point_dir;
    if ( player.isTackling() )
    {
        strm << " t";
    }
    else if ( player.kicked() )
    {
        strm << " k";
    }

    if ( player.hasRedCard() )
    {
        strm << " r";
    }
    else if ( player.hasYellowCard() )
    {
        strm << " y";
    }

    strm << ')';
}


const
SerializerCoach::Ptr
SerializerCoachStdv18::create()
{
    SerializerCommon::Creator cre;
    if ( ! SerializerCommon::factory().getCreator( cre, 18 ) )
    {
        return SerializerCoach::Ptr();
    }

    SerializerCoach::Ptr ptr( new SerializerCoachStdv18( cre() ) );
    return ptr;
}


namespace {
RegHolder v18 = SerializerCoach::factory().autoReg( &SerializerCoachStdv18::create, 18 );
}

}
