// -*-c++-*-

/***************************************************************************
                            serializercoachstdv14.cpp
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

#include "serializercoachstdv14.h"

#include "player.h"

namespace rcss {

SerializerCoachStdv14::SerializerCoachStdv14( const SerializerCommon::Ptr common )
    : SerializerCoachStdv13( common )
{

}


SerializerCoachStdv14::~SerializerCoachStdv14()
{

}

void
SerializerCoachStdv14::serializeVisualPlayer( std::ostream & strm,
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
SerializerCoachStdv14::serializeVisualPlayer( std::ostream & strm,
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
SerializerCoachStdv14::create()
{
    SerializerCommon::Creator cre;
    if ( ! SerializerCommon::factory().getCreator( cre, 14 ) )
    {
        return SerializerCoach::Ptr();
    }

    SerializerCoach::Ptr ptr( new SerializerCoachStdv14( cre() ) );
    return ptr;
}


namespace {
RegHolder v14 = SerializerCoach::factory().autoReg( &SerializerCoachStdv14::create, 14 );
RegHolder v15 = SerializerCoach::factory().autoReg( &SerializerCoachStdv14::create, 15 );
//RegHolder v16 = SerializerCoach::factory().autoReg( &SerializerCoachStdv14::create, 16 );
}

}
