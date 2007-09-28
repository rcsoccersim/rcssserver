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
 *   Foundation; either version 2 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#include "serializeronlinecoachstdv8.h"

namespace rcss
{

SerializerOnlineCoachStdv8::SerializerOnlineCoachStdv8( const SerializerCommon & common,
                                                        const SerializerCoach & coach )
    : SerializerOnlineCoachStdv7( common, coach )
{}

SerializerOnlineCoachStdv8::~SerializerOnlineCoachStdv8()
{}

const SerializerOnlineCoachStdv8*
SerializerOnlineCoachStdv8::instance()
{
    rcss::SerializerCommon::Creator cre_common;
    if( !rcss::SerializerCommon::factory().getCreator( cre_common, 8 ) )
        return NULL;
    rcss::SerializerCoach::Creator cre_coach;
    if( !rcss::SerializerCoach::factory().getCreator( cre_coach, 8 ) )
        return NULL;
    static SerializerOnlineCoachStdv8 ser( cre_common(), *cre_coach() );
    return &ser;
}

void
SerializerOnlineCoachStdv8::serializePlayerClangVer( std::ostream& strm,
                                                     const std::string& name,
                                                     const unsigned int& min,
                                                     const unsigned int& max ) const
{ strm << "(clang (ver " << name << " " << min << " " << max << "))"; }

namespace
{
const SerializerOnlineCoach*
create()
{ return SerializerOnlineCoachStdv8::instance(); }

lib::RegHolder v8 = SerializerOnlineCoach::factory().autoReg( &create, 8 );
lib::RegHolder v9 = SerializerOnlineCoach::factory().autoReg( &create, 9 );
lib::RegHolder v10 = SerializerOnlineCoach::factory().autoReg( &create, 10 );
lib::RegHolder v11 = SerializerOnlineCoach::factory().autoReg( &create, 11 );
}
}
