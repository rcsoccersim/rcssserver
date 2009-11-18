// -*-c++-*-

/***************************************************************************
                            serializercoachstdv8.cpp
               Class for serializing data to std v8 offline coaches
                             -------------------
    begin                : 27-JAN-2003
    copyright            : (C) 2003 by The RoboCup Soccer Server
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

#include "serializercoachstdv8.h"

#include "clangmsg.h"
#include "object.h"
#include "player.h"

namespace rcss {

SerializerCoachStdv8::SerializerCoachStdv8( const SerializerCommon::Ptr common )
    : SerializerCoachStdv7( common )
{

}

SerializerCoachStdv8::~SerializerCoachStdv8()
{

}

void
SerializerCoachStdv8::serializeVisualObject( std::ostream & strm,
                                             const std::string & name,
                                             const PVector & pos,
                                             const PVector & vel,
                                             const int body,
                                             const int neck,
                                             const bool tackling ) const
{
    strm << " (" << name
         << ' ' << pos.x << ' ' << pos.y
         << ' ' << vel.x << ' ' << vel.y
         << ' ' << body
         << ' ' << neck;
    if ( tackling )
    {
        strm << " t";
    }
    strm << ')';
}

void
SerializerCoachStdv8::serializeVisualObject( std::ostream & strm,
                                             const std::string & name,
                                             const PVector & pos,
                                             const PVector & vel,
                                             const int body,
                                             const int neck,
                                             const int point_dir,
                                             const bool tackling ) const
{
    strm << " (" << name
         << ' ' << pos.x << ' ' << pos.y
         << ' ' << vel.x << ' ' << vel.y
         << ' ' << body
         << ' ' << neck
         << ' ' << point_dir;
    if ( tackling )
    {
        strm << " t";
    }
    strm << ')';
}

void
SerializerCoachStdv8::serializeVisualPlayer( std::ostream & strm,
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
    strm << ')';
}

void
SerializerCoachStdv8::serializeVisualPlayer( std::ostream & strm,
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
    strm << ')';
}


const
SerializerCoach::Ptr
SerializerCoachStdv8::create()
{
    SerializerCommon::Creator cre;
    if ( ! SerializerCommon::factory().getCreator( cre, 8 ) )
    {
        return SerializerCoach::Ptr();
    }

    SerializerCoach::Ptr ptr( new SerializerCoachStdv8( cre() ) );
    return ptr;
}

namespace {
RegHolder v8 = SerializerCoach::factory().autoReg( &SerializerCoachStdv8::create, 8 );
RegHolder v9 = SerializerCoach::factory().autoReg( &SerializerCoachStdv8::create, 9 );
RegHolder v10 = SerializerCoach::factory().autoReg( &SerializerCoachStdv8::create, 10 );
RegHolder v11 = SerializerCoach::factory().autoReg( &SerializerCoachStdv8::create, 11 );
RegHolder v12 = SerializerCoach::factory().autoReg( &SerializerCoachStdv8::create, 12 );
}

}
