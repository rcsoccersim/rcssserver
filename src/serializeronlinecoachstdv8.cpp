// -*-c++-*-

/***************************************************************************
                            serializeronlinecoachstdv8.cc
               Class for serializing data to std v8 online coaches
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

#include "serializeronlinecoachstdv8.h"

namespace rcss {

SerializerOnlineCoachStdv8::SerializerOnlineCoachStdv8( const SerializerCommon::Ptr common,
                                                        const SerializerCoach::Ptr coach )
    : SerializerOnlineCoachStdv7( common, coach )
{

}

SerializerOnlineCoachStdv8::~SerializerOnlineCoachStdv8()
{

}

void
SerializerOnlineCoachStdv8::serializePlayerClangVer( std::ostream& strm,
                                                     const std::string & name,
                                                     const unsigned int min,
                                                     const unsigned int max ) const
{
    strm << "(clang (ver " << name << ' ' << min << ' ' << max << "))";
}


const
SerializerOnlineCoach::Ptr
SerializerOnlineCoachStdv8::create()
{
    SerializerCommon::Creator cre_common;
    if ( ! SerializerCommon::factory().getCreator( cre_common, 8 ) )
    {
        return SerializerOnlineCoach::Ptr();
    }

    SerializerCoach::Creator cre_coach;
    if ( ! SerializerCoach::factory().getCreator( cre_coach, 8 ) )
    {
        return SerializerOnlineCoach::Ptr();
    }

    SerializerOnlineCoach::Ptr ptr( new SerializerOnlineCoachStdv8( cre_common(), cre_coach() ) );
    return ptr;
}


namespace {
RegHolder v8 = SerializerOnlineCoach::factory().autoReg( &SerializerOnlineCoachStdv8::create, 8 );
RegHolder v9 = SerializerOnlineCoach::factory().autoReg( &SerializerOnlineCoachStdv8::create, 9 );
RegHolder v10 = SerializerOnlineCoach::factory().autoReg( &SerializerOnlineCoachStdv8::create, 10 );
RegHolder v11 = SerializerOnlineCoach::factory().autoReg( &SerializerOnlineCoachStdv8::create, 11 );
RegHolder v12 = SerializerOnlineCoach::factory().autoReg( &SerializerOnlineCoachStdv8::create, 12 );
}

}
