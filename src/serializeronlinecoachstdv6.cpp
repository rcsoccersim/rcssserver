// -*-c++-*-

/***************************************************************************
                            serializeronlinecoachstdv6.cc
               Class for serializing data to std v6 online coaches
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

#include "serializeronlinecoachstdv6.h"

namespace rcss {

SerializerOnlineCoachStdv6::SerializerOnlineCoachStdv6( const SerializerCommon::Ptr common,
                                                        const SerializerCoach::Ptr coach )
    : SerializerOnlineCoachStdv1( common, coach )
{

}

SerializerOnlineCoachStdv6::~SerializerOnlineCoachStdv6()
{

}

void
SerializerOnlineCoachStdv6::serializeInit( std::ostream& strm,
                                           const int side ) const
{
    strm << "(init "
         << ( side == LEFT ? 'l' : 'r' )
         << " ok)";
}

const
SerializerOnlineCoach::Ptr
SerializerOnlineCoachStdv6::create()
{
    SerializerCommon::Creator cre_common;
    if ( ! SerializerCommon::factory().getCreator( cre_common, 6 ) )
    {
        return SerializerOnlineCoach::Ptr();
    }

    SerializerCoach::Creator cre_coach;
    if ( ! SerializerCoach::factory().getCreator( cre_coach, 6 ) )
    {
        return SerializerOnlineCoach::Ptr();
    }

    SerializerOnlineCoach::Ptr ptr( new SerializerOnlineCoachStdv6( cre_common(), cre_coach() ) );
    return ptr;
}

namespace {
RegHolder v6 = SerializerOnlineCoach::factory().autoReg( &SerializerOnlineCoachStdv6::create, 6 );
}

}
