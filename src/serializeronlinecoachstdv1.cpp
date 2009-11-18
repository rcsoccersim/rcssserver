// -*-c++-*-

/***************************************************************************
                            serializeronlinecoachstdv1.cc
               Class for serializing data to std v1 online coaches
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

#include "serializeronlinecoachstdv1.h"

namespace rcss {

SerializerOnlineCoachStdv1::SerializerOnlineCoachStdv1( const SerializerCommon::Ptr common,
                                                        const SerializerCoach::Ptr coach )
    : SerializerOnlineCoach( common, coach )
{

}

SerializerOnlineCoachStdv1::~SerializerOnlineCoachStdv1()
{

}

void
SerializerOnlineCoachStdv1::serializeRefereeAudio( std::ostream & strm,
                                                   const int time,
                                                   const std::string & name,
                                                   const char* msg ) const
{
    strm << "(hear " << time << ' ' << name
         << ' ' << msg << ')';
}

void
SerializerOnlineCoachStdv1::serializePlayerAudio( std::ostream & strm,
                                                  const int time,
                                                  const std::string & name,
                                                  const char * msg ) const
{
    strm << "(hear " << time << ' ' << name << ' '
         << msg << ')';
}

void
SerializerOnlineCoachStdv1::serializePlayerClangVer( std::ostream & ,
                                                     const std::string &,
                                                     const unsigned int,
                                                     const unsigned int ) const
{

}

void
SerializerOnlineCoachStdv1::serializeInit( std::ostream & strm,
                                           const int ) const
{
    strm << "(init ok)";
}

void
SerializerOnlineCoachStdv1::serializeChangedPlayer( std::ostream &,
                                                    const int,
                                                    const int ) const
{

}

const
SerializerOnlineCoach::Ptr
SerializerOnlineCoachStdv1::create()
{
    SerializerCommon::Creator cre_common;
    if ( ! SerializerCommon::factory().getCreator( cre_common, 1 ) )
    {
        return SerializerOnlineCoach::Ptr();
    }

    SerializerCoach::Creator cre_coach;
    if ( ! SerializerCoach::factory().getCreator( cre_coach, 1 ) )
    {
        return SerializerOnlineCoach::Ptr();
    }

    SerializerOnlineCoach::Ptr ptr( new SerializerOnlineCoachStdv1( cre_common(), cre_coach() ) );
    return ptr;
}

namespace {
RegHolder v1 = SerializerOnlineCoach::factory().autoReg( &SerializerOnlineCoachStdv1::create, 1 );
RegHolder v2 = SerializerOnlineCoach::factory().autoReg( &SerializerOnlineCoachStdv1::create, 2 );
RegHolder v3 = SerializerOnlineCoach::factory().autoReg( &SerializerOnlineCoachStdv1::create, 3 );
RegHolder v4 = SerializerOnlineCoach::factory().autoReg( &SerializerOnlineCoachStdv1::create, 4 );
RegHolder v5 = SerializerOnlineCoach::factory().autoReg( &SerializerOnlineCoachStdv1::create, 5 );
}

}
