// -*-c++-*-

/***************************************************************************
                         serializeronlinecoachstdv13.cpp
               Class for serializing data to std v13 online coaches
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

#include "serializeronlinecoachstdv13.h"

namespace rcss {

SerializerOnlineCoachStdv13::SerializerOnlineCoachStdv13( const SerializerCommon::Ptr common,
                                                          const SerializerCoach::Ptr coach )
    : SerializerOnlineCoachStdv8( common, coach )
{

}

SerializerOnlineCoachStdv13::~SerializerOnlineCoachStdv13()
{

}

const
SerializerOnlineCoach::Ptr
SerializerOnlineCoachStdv13::create()
{
    SerializerCommon::Creator cre_common;
    if ( ! SerializerCommon::factory().getCreator( cre_common, 13 ) )
    {
        return SerializerOnlineCoach::Ptr();
    }

    SerializerCoach::Creator cre_coach;
    if ( ! SerializerCoach::factory().getCreator( cre_coach, 13 ) )
    {
        return SerializerOnlineCoach::Ptr();
    }

    SerializerOnlineCoach::Ptr ptr( new SerializerOnlineCoachStdv13( cre_common(), cre_coach() ) );
    return ptr;
}

namespace {
RegHolder v13 = SerializerOnlineCoach::factory().autoReg( &SerializerOnlineCoachStdv13::create, 13 );
}

}
