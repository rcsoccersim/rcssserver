// -*-c++-*-

/***************************************************************************
                         serializeronlinecoachstdv13.cc
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
 *   Foundation; either version 2 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#include "serializeronlinecoachstdv13.h"

namespace rcss {

SerializerOnlineCoachStdv13::SerializerOnlineCoachStdv13( const SerializerCommon & common,
                                                          const SerializerCoach & coach )
    : SerializerOnlineCoachStdv8( common, coach )
{

}

SerializerOnlineCoachStdv13::~SerializerOnlineCoachStdv13()
{

}

const
SerializerOnlineCoachStdv13 *
SerializerOnlineCoachStdv13::instance()
{
    rcss::SerializerCommon::Creator cre_common;
    if ( ! rcss::SerializerCommon::factory().getCreator( cre_common, 13 ) )
    {
        return NULL;
    }

    rcss::SerializerCoach::Creator cre_coach;
    if ( ! rcss::SerializerCoach::factory().getCreator( cre_coach, 13 ) )
    {
        return NULL;
    }

    static SerializerOnlineCoachStdv13 ser( cre_common(), *cre_coach() );
    return &ser;
}

namespace {
const
SerializerOnlineCoach *
create()
{
    return SerializerOnlineCoachStdv13::instance();
}

RegHolder v13 = SerializerOnlineCoach::factory().autoReg( &create, 13 );
}

}
