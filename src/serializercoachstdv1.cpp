// -*-c++-*-

/***************************************************************************
                            serializercoachstdv1.cc
               Class for serializing data to std v1 offline coaches
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
 *   Foundation; either version 2 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#include "serializercoachstdv1.h"
#include "clangmsg.h"
#include "param.h"
#include "object.h"

namespace rcss {

SerializerCoachStdv1::SerializerCoachStdv1( const SerializerCommon & common )
    : SerializerCoach( common )
{

}

SerializerCoachStdv1::~SerializerCoachStdv1()
{

}

const
SerializerCoachStdv1 *
SerializerCoachStdv1::instance()
{
    rcss::SerializerCommon::Creator cre;
    if ( ! rcss::SerializerCommon::factory().getCreator( cre, 1 ) )
    {
        return NULL;
    }

    static SerializerCoachStdv1 ser( cre() );
    return &ser;
}

void
SerializerCoachStdv1::serializeRefereeAudio( std::ostream & strm,
                                             const int time,
                                             const char * msg ) const
{
    strm << "(hear " << REFEREE_NAME << ' ' << time
         << ' ' << msg << ')';
}

void
SerializerCoachStdv1::serializeCoachAudio( std::ostream & strm,
                                           const int time,
                                           const std::string & name,
                                           const char * msg ) const
{
    strm << "(hear " << name << ' ' << time
         << ' ' << msg << ')';
}

void
SerializerCoachStdv1::serializeCoachStdAudio( std::ostream & strm,
                                              const int time,
                                              const std::string & name,
                                              const rcss::clang::Msg & msg ) const
{
    strm << "(hear " << name << ' ' << time
         << ' ' << msg << ')';
}

void
SerializerCoachStdv1::serializePlayerAudio( std::ostream & strm,
                                            const int time,
                                            const std::string & name,
                                            const char * msg ) const
{
    strm << "(hear " << name << ' ' << time
         << ' ' << msg << ')';
}

void
SerializerCoachStdv1::serializeInit( std::ostream & strm ) const
{
    strm << "(init ok)";
}


void
SerializerCoachStdv1::serializeVisualBegin( std::ostream & strm,
                                            const int time ) const
{
    strm << "(see " << time;
}


void
SerializerCoachStdv1::serializeVisualEnd( std::ostream & strm ) const
{
    strm << ')';
}


void
SerializerCoachStdv1::serializeLookBegin( std::ostream & strm,
                                          const int time ) const
{
    strm << "(ok look " << time;
}

void
SerializerCoachStdv1::serializeLookEnd( std::ostream & strm ) const
{
    strm << ')';
}

void
SerializerCoachStdv1::serializeVisualObject( std::ostream & strm,
                                             const std::string & name,
                                             const PVector & pos ) const
{
    strm << " (" << name << ' ' << pos.x << ' ' << pos.y << ')';
}

void
SerializerCoachStdv1::serializeVisualObject( std::ostream & strm,
                                             const std::string & name,
                                             const PVector & pos,
                                             const PVector & vel ) const
{
    strm << " (" << name
         << ' ' << pos.x << ' ' << pos.y
         << ' ' << vel.x << ' ' << vel.y
         << ')';
}


void
SerializerCoachStdv1::serializeVisualObject( std::ostream & strm,
                                             const std::string & name,
                                             const PVector & pos,
                                             const PVector & vel,
                                             const int body,
                                             const int neck ) const
{
    strm << " (" << name
         << ' ' << pos.x << ' ' << pos.y
         << ' ' << body
         << ' ' << neck
         << ' ' << vel.x << ' ' << vel.y
         << ')';
}


void
SerializerCoachStdv1::serializeVisualPlayer( std::ostream & strm,
                                             const Player & /*player*/ ,
                                             const std::string & name,
                                             const PVector & pos,
                                             const PVector & vel,
                                             const int body,
                                             const int neck ) const
{
    strm << " (" << name
         << ' ' << pos.x << ' ' << pos.y
         << ' ' << body
         << ' ' << neck
         << ' ' << vel.x << ' ' << vel.y
         << ')';
}

 void
 SerializerCoachStdv1::serializeVisualPlayer( std::ostream & strm,
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
          << ' ' << body
          << ' ' << neck
          << ' ' << vel.x << ' ' << vel.y
          << ')';
 }


void
SerializerCoachStdv1::serializeOKEye( std::ostream & strm,
                                      const bool on ) const
{
    strm << "(ok eye "
         << ( on ? "on" : "off" )
         << ')';
}


namespace {

const
SerializerCoach *
create()
{
    return SerializerCoachStdv1::instance();
}

RegHolder v1 = SerializerCoach::factory().autoReg( &create, 1 );
RegHolder v2 = SerializerCoach::factory().autoReg( &create, 2 );
RegHolder v3 = SerializerCoach::factory().autoReg( &create, 3 );
RegHolder v4 = SerializerCoach::factory().autoReg( &create, 4 );
RegHolder v5 = SerializerCoach::factory().autoReg( &create, 5 );
RegHolder v6 = SerializerCoach::factory().autoReg( &create, 6 );
}

}
