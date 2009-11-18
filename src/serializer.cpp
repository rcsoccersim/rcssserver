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
 *   Foundation; either version 3 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "serializer.h"

//#include <iostream>

namespace rcss {

SerializerCommon::FactoryHolder &
SerializerCommon::factory()
{
    static FactoryHolder rval;
    return rval;
}

SerializerCommon::SerializerCommon()
{
    //std::cerr << "create SerializerCommon" << std::endl;
}

SerializerCommon::~SerializerCommon()
{
    //std::cerr << "delete SerializerCommon" << std::endl;
}


Serializer::~Serializer()
{
    //std::cerr << "delete Serializer" << std::endl;
}


SerializerPlayer::FactoryHolder &
SerializerPlayer::factory()
{
    static FactoryHolder rval;
    return rval;
}

SerializerPlayer::SerializerPlayer( const SerializerCommon::Ptr common )
    : Serializer( common )
{
    //std::cerr << "create SerializerPlayer" << std::endl;
}

SerializerPlayer::~SerializerPlayer()
{
    //std::cerr << "delete SerializerPlayer" << std::endl;
}


SerializerCoach::FactoryHolder &
SerializerCoach::factory()
{
    static FactoryHolder rval;
    return rval;
}

SerializerCoach::SerializerCoach( const SerializerCommon::Ptr common )
    : Serializer( common )
{
    //std::cerr << "create SerializerCoach" << std::endl;
}

SerializerCoach::~SerializerCoach()
{
    //std::cerr << "delete SerializerCoach" << std::endl;
}

SerializerOnlineCoach::FactoryHolder &
SerializerOnlineCoach::factory()
{
    static FactoryHolder rval;
    return rval;
}


SerializerOnlineCoach::SerializerOnlineCoach( const SerializerCommon::Ptr common,
                                              const SerializerCoach::Ptr coach )
    : Serializer( common ),
      M_coach( coach )
{
    //std::cerr << "create SerializerOnlineCoach" << std::endl;
}

SerializerOnlineCoach::~SerializerOnlineCoach()
{
    //std::cerr << "delete SerializerOnlineCoach" << std::endl;
}


/*
//===================================================================
//
//  SerializerMonitor
//
//===================================================================
*/

SerializerMonitor::FactoryHolder &
SerializerMonitor::factory()
{
    static FactoryHolder rval;
    return rval;
}


SerializerMonitor::SerializerMonitor( const SerializerCommon::Ptr common )
    : Serializer( common )
{
    //std::cerr << "create SerializerMonitor" << std::endl;
}

SerializerMonitor::~SerializerMonitor()
{
    //std::cerr << "delete SerializerMonitor" << std::endl;
}

}
