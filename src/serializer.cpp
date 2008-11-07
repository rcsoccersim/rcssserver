// -*-c++-*-

/***************************************************************************
                                 serializer.cc
                    Classes for serializing data to clients
                             -------------------
    begin                : 28-JAN-2002
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

#include "serializer.h"

namespace rcss {

SerializerCommon::FactoryHolder &
SerializerCommon::factory()
{
    static FactoryHolder rval;
    return rval;
}

SerializerCommon::SerializerCommon()
{

}

SerializerCommon::~SerializerCommon()
{

}


SerializerPlayer::FactoryHolder &
SerializerPlayer::factory()
{
    static FactoryHolder rval;
    return rval;
}

SerializerPlayer::SerializerPlayer( const SerializerCommon & common )
    : Serializer( common )
{

}

SerializerPlayer::~SerializerPlayer()
{

}


SerializerCoach::FactoryHolder &
SerializerCoach::factory()
{
    static FactoryHolder rval;
    return rval;
}

SerializerCoach::SerializerCoach( const SerializerCommon & common )
    : Serializer( common )
{

}

SerializerCoach::~SerializerCoach()
{

}

SerializerOnlineCoach::FactoryHolder &
SerializerOnlineCoach::factory()
{
    static FactoryHolder rval;
    return rval;
}


SerializerOnlineCoach::SerializerOnlineCoach( const SerializerCommon & common,
                                              const SerializerCoach & coach )
    : Serializer( common ),
      M_coach( coach )
{

}

SerializerOnlineCoach::~SerializerOnlineCoach()
{

}

}
