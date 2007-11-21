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

SerializerCommon::Factory &
SerializerCommon::factory()
{
    static Factory rval;
    return rval;
}

SerializerCommon::SerializerCommon()
{}

SerializerCommon::~SerializerCommon()
{}


SerializerPlayer::Factory &
SerializerPlayer::factory()
{
    static Factory rval;
    return rval;
}

SerializerPlayer::SerializerPlayer( const SerializerCommon & common )
    : Serializer( common )
{

}

SerializerPlayer::~SerializerPlayer()
{

}


SerializerCoach::Factory &
SerializerCoach::factory()
{
    static Factory rval;
    return rval;
}

SerializerCoach::SerializerCoach( const SerializerCommon & common )
    : Serializer( common )
{}

SerializerCoach::~SerializerCoach()
{}

SerializerOnlineCoach::Factory &
SerializerOnlineCoach::factory()
{
    static Factory rval;
    return rval;
}


SerializerOnlineCoach::SerializerOnlineCoach( const SerializerCommon & common,
                                              const SerializerCoach & coach )
    : Serializer( common )
    , M_coach( coach )
{

}

SerializerOnlineCoach::~SerializerOnlineCoach()
{

}

}
