// -*-c++-*-

/***************************************************************************
                         serializeronlinecoachstdv18.cpp
               Class for serializing data to std v18 online coaches
                             -------------------
    begin                : 2008-11-03
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

#include "serializeronlinecoachstdv18.h"

namespace rcss {

SerializerOnlineCoachStdv18::SerializerOnlineCoachStdv18( const SerializerCommon::Ptr common,
                                                          const SerializerCoach::Ptr coach )
        : SerializerOnlineCoachStdv14( common, coach )
{

}

SerializerOnlineCoachStdv18::~SerializerOnlineCoachStdv18()
{

}

const
SerializerOnlineCoach::Ptr
SerializerOnlineCoachStdv18::create()
{
    SerializerCommon::Creator cre_common;
    if ( ! SerializerCommon::factory().getCreator( cre_common, 18 ) )
    {
        return SerializerOnlineCoach::Ptr();
    }

    SerializerCoach::Creator cre_coach;
    if ( ! SerializerCoach::factory().getCreator( cre_coach, 18 ) )
    {
        return SerializerOnlineCoach::Ptr();
    }

    SerializerOnlineCoach::Ptr ptr( new SerializerOnlineCoachStdv18( cre_common(), cre_coach() ) );
    return ptr;
}
namespace {
RegHolder v18 = SerializerOnlineCoach::factory().autoReg( &SerializerOnlineCoachStdv18::create, 18 );
}

}
