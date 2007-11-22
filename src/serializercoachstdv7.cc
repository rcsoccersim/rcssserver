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
 *   Foundation; either version 2 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#include "serializercoachstdv7.h"
#include "clangmsg.h"
#include "object.h"

namespace rcss
{
SerializerCoachStdv7::SerializerCoachStdv7( const SerializerCommon & common )
    : SerializerCoachStdv1( common )
{

}

SerializerCoachStdv7::~SerializerCoachStdv7()
{

}

const
 SerializerCoachStdv7*
SerializerCoachStdv7::instance ()
{
    rcss::SerializerCommon::Creator cre;
    if( !rcss::SerializerCommon::factory().getCreator( cre, 7 ) )
        return NULL;
    static SerializerCoachStdv7 ser( cre() );
    return &ser;
}

void
SerializerCoachStdv7::serializeCoachAudio( std::ostream& strm,
                                           const int& time,
                                           const std::string& name,
                                           const char* msg ) const
{
    strm << "(hear " << time << " "  << name
         << " \"" << msg << "\")";
}

void
SerializerCoachStdv7::serializeCoachStdAudio( std::ostream& strm,
                                              const int& time,
                                              const std::string& name,
                                              const rcss::clang::Msg& msg ) const
{
    strm << "(hear " << time << " " << name
         << " \"" << msg << "\")";
}

void
SerializerCoachStdv7::serializePlayerAudio( std::ostream& strm,
                                            const int& time,
                                            const std::string& name,
                                            const char* msg ) const
{
    strm << "(hear " << time << " " << name
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
         << " " << pos.x << " " << pos.y
         << " " << vel.x << " " << vel.y
         << " " << body
         << " " << neck
         << ")";
}

namespace
{
const SerializerCoach*
create()
{ return SerializerCoachStdv7::instance(); }

lib::RegHolder v7 = SerializerCoach::factory().autoReg( &create, 7 );
}
}
