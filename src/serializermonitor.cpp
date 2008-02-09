// -*-c++-*-

/***************************************************************************
                            serializermonitor.cpp
                  Class for serializing data to monitors
                             -------------------
    begin                : 2007-11-22
    copyright            : (C) 2007 by The RoboCup Soccer Server
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "serializermonitor.h"

namespace rcss {

/*
//===================================================================
//
//  SerializerMonitor
//
//===================================================================
*/

SerializerMonitor::Factory &
SerializerMonitor::factory()
{
    static Factory rval;
    return rval;
}


SerializerMonitor::SerializerMonitor( const SerializerCommon & common )
    : Serializer( common )
{

}

SerializerMonitor::~SerializerMonitor()
{

}


/*
//===================================================================
//
//  SerializerMonitorStdv1
//
//===================================================================
*/

SerializerMonitorStdv1::SerializerMonitorStdv1( const SerializerCommon & common )
    : SerializerMonitor( common )
{

}

SerializerMonitorStdv1::~SerializerMonitorStdv1()
{

}

const
SerializerMonitorStdv1 *
SerializerMonitorStdv1::instance()
{
    rcss::SerializerCommon::Creator cre_common;
    if ( ! rcss::SerializerCommon::factory().getCreator( cre_common, 8 ) )
    {
        return NULL;
    }

    static SerializerMonitorStdv1 ser( cre_common() );
    return &ser;
}


/*
//===================================================================
//
//  SerializerMonitorStdv3
//
//===================================================================
*/

SerializerMonitorStdv3::SerializerMonitorStdv3( const SerializerCommon & common )
    : SerializerMonitorStdv1( common )
{

}

SerializerMonitorStdv3::~SerializerMonitorStdv3()
{

}

const
SerializerMonitorStdv3 *
SerializerMonitorStdv3::instance()
{
    rcss::SerializerCommon::Creator cre_common;
    if ( ! rcss::SerializerCommon::factory().getCreator( cre_common, 8 ) )
    {
        return NULL;
    }

    static SerializerMonitorStdv3 ser( cre_common() );
    return &ser;
}


void
SerializerMonitorStdv3::serializeScore( std::ostream & os,
                                        const int time,
                                        const std::string & left_name,
                                        const std::string & right_name,
                                        const int left_point,
                                        const int right_point,
                                        const int left_pen_taken,
                                        const int right_pen_taken,
                                        const int left_pen_point,
                                        const int right_pen_point ) const
{
    os << "(team " << time
       << ' ' << ( left_name.empty() ? "null" : left_name.c_str() )
       << ' ' << ( right_name.empty() ? "null" : right_name.c_str() )
       << ' ' << left_point
       << ' ' << right_point;

    if ( left_pen_taken > 0 || right_pen_taken > 0 )
    {
        os << ' ' << left_pen_point
           << ' ' << left_pen_taken - left_pen_point
           << ' ' << right_pen_point
           << ' ' << right_pen_taken - right_pen_point;
    }
    os << ')';

}

void
SerializerMonitorStdv3::serializePlayMode( std::ostream & os,
                                           const int time,
                                           const PlayMode pmode ) const
{

    static const char * playmode_strings[] = PLAYMODE_STRINGS;

    os << "(playmode "
       << time << ' '
       << playmode_strings[pmode] << ')';
}


namespace {

const
SerializerMonitor *
create1()
{
    return SerializerMonitorStdv1::instance();
}

const
SerializerMonitor *
create3()
{
    return SerializerMonitorStdv3::instance();
}

lib::RegHolder v1 = SerializerMonitor::factory().autoReg( &create1, 1 );
lib::RegHolder v2 = SerializerMonitor::factory().autoReg( &create1, 2 );
lib::RegHolder v3 = SerializerMonitor::factory().autoReg( &create3, 3 );

}
}
