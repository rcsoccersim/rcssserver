// -*-c++-*-

/***************************************************************************
                         serializeronlinecoachstdv14.cpp
               Class for serializing data to std v14 online coaches
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

#include "serializeronlinecoachstdv14.h"

namespace rcss {

SerializerOnlineCoachStdv14::SerializerOnlineCoachStdv14( const SerializerCommon::Ptr common,
                                                          const SerializerCoach::Ptr coach )
    : SerializerOnlineCoachStdv13( common, coach )
{

}

SerializerOnlineCoachStdv14::~SerializerOnlineCoachStdv14()
{

}

const
SerializerOnlineCoach::Ptr
SerializerOnlineCoachStdv14::create()
{
    SerializerCommon::Creator cre_common;
    if ( ! SerializerCommon::factory().getCreator( cre_common, 14 ) )
    {
        return SerializerOnlineCoach::Ptr();
    }

    SerializerCoach::Creator cre_coach;
    if ( ! SerializerCoach::factory().getCreator( cre_coach, 14 ) )
    {
        return SerializerOnlineCoach::Ptr();
    }

    SerializerOnlineCoach::Ptr ptr( new SerializerOnlineCoachStdv14( cre_common(), cre_coach() ) );
    return ptr;
}

namespace {
RegHolder v14 = SerializerOnlineCoach::factory().autoReg( &SerializerOnlineCoachStdv14::create, 14 );
RegHolder v15 = SerializerOnlineCoach::factory().autoReg( &SerializerOnlineCoachStdv14::create, 15 );
//RegHolder v16 = SerializerOnlineCoach::factory().autoReg( &SerializerOnlineCoachStdv14::create, 16 );
}

}
