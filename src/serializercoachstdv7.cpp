// -*-c++-*-

/***************************************************************************
                            serializercoachstdv7.cc
               Class for serializing data to std v7 offline coaches
                             -------------------
    begin                : 27-MAY-2002
    copyright            : (C) 2002 by The RoboCup Soccer Server
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

#include "serializercoachstdv7.h"

#include "clangmsg.h"
#include "object.h"

namespace rcss {

SerializerCoachStdv7::SerializerCoachStdv7( const SerializerCommon::Ptr common )
    : SerializerCoachStdv1( common )
{

}

SerializerCoachStdv7::~SerializerCoachStdv7()
{

}

void
SerializerCoachStdv7::serializeCoachAudio( std::ostream & strm,
                                           const int time,
                                           const std::string & name,
                                           const char * msg ) const
{
    strm << "(hear " << time << ' ' << name
         << " \"" << msg << "\")";
}

void
SerializerCoachStdv7::serializeCoachStdAudio( std::ostream & strm,
                                              const int time,
                                              const std::string & name,
                                              const rcss::clang::Msg & msg ) const
{
    strm << "(hear " << time << ' ' << name
         << " \"" << msg << "\")";
}

void
SerializerCoachStdv7::serializePlayerAudio( std::ostream & strm,
                                            const int time,
                                            const std::string & name,
                                            const char * msg ) const
{
    strm << "(hear " << time << ' ' << name
         << " \"" << msg << "\")";
}


void
SerializerCoachStdv7::serializeVisualBegin( std::ostream & strm,
                                            const int time ) const
{
    strm << "(see_global " << time;
}


void
SerializerCoachStdv7::serializeVisualObject( std::ostream & strm,
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
         << ' ' << neck
         << ')';
}

void
SerializerCoachStdv7::serializeVisualPlayer( std::ostream & strm,
                                             const Player & /*player*/ ,
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
         << ' ' << neck
         << ')';
}

void
SerializerCoachStdv7::serializeVisualPlayer( std::ostream & strm,
                                             const Player & /*player*/ ,
                                             const std::string & name,
                                             const PVector & pos,
                                             const PVector & vel,
                                             const int body,
                                             const int neck,
                                             const int /*point_dir*/ ) const
{
    strm << " (" << name
         << ' ' << pos.x << ' ' << pos.y
         << ' ' << vel.x << ' ' << vel.y
         << ' ' << body
         << ' ' << neck
         << ')';
}


const
SerializerCoach::Ptr
SerializerCoachStdv7::create()
{
    SerializerCommon::Creator cre;
    if ( ! SerializerCommon::factory().getCreator( cre, 7 ) )
    {
        return SerializerCoach::Ptr();
    }

    SerializerCoach::Ptr ptr( new SerializerCoachStdv7( cre() ) );
    return ptr;
}


namespace {
RegHolder v7 = SerializerCoach::factory().autoReg( &SerializerCoachStdv7::create, 7 );
}

}
