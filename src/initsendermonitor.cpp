// -*-c++-*-

/***************************************************************************
                             initsendermonitor.cpp
               Classes for sending sense init messages for monitors
                             -------------------
    begin                : 2007-11-21
    copyright            : (C) 2007 by The RoboCup Soccer Simulator
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

#include "initsendermonitor.h"

#include "monitor.h"
#include "stadium.h"
#include "heteroplayer.h"
#include "team.h"
#include "serializermonitor.h"

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_WINSOCK2_H
#include <winsock2.h>
#endif

namespace rcss {

/*
//===================================================================
//
//  CLASS: InitSenderMonitor
//
//===================================================================
*/

InitSenderMonitor::FactoryHolder &
InitSenderMonitor::factory()
{
    static FactoryHolder rval;
    return rval;
}


InitSenderMonitor::InitSenderMonitor( const Params & params,
                                      const boost::shared_ptr< InitSenderCommon > common )
    : InitSender( params.M_transport, common ),
      M_serializer( params.M_serializer ),
      M_self( params.M_self ),
      M_stadium( params.M_stadium )
{

}

InitSenderMonitor::~InitSenderMonitor()
{

}

/*
//===================================================================
//
//  CLASS: InitSenderMonitorV1
//
//===================================================================
*/

InitSenderMonitorV1::InitSenderMonitorV1( const Params & params )
    : InitSenderMonitor( params,
                         boost::shared_ptr< InitSenderCommon >
                         ( new InitSenderCommonV1( params.M_transport,
                                                   params.M_serializer,
                                                   params.M_stadium,
                                                   1 ) ) )
{

}

InitSenderMonitorV1::InitSenderMonitorV1( const Params & params,
                                          const boost::shared_ptr< InitSenderCommon > common )
    : InitSenderMonitor( params, common )
{

}

InitSenderMonitorV1::~InitSenderMonitorV1()
{

}

void
InitSenderMonitorV1::sendInit()
{

}

void
InitSenderMonitorV1::sendServerParams()
{

}

void
InitSenderMonitorV1::sendPlayerParams()
{

}

void
InitSenderMonitorV1::sendPlayerTypes()
{

}

void
InitSenderMonitorV1::sendChangedPlayers()
{

}

void
InitSenderMonitorV1::sendScore()
{

}


void
InitSenderMonitorV1::sendPlayMode()
{

}

/*
//===================================================================
//
//  InitSenderMonitorV2
//
//===================================================================
*/

InitSenderMonitorV2::InitSenderMonitorV2( const Params & params )
    : InitSenderMonitorV1( params,
                           boost::shared_ptr< InitSenderCommon >
                           ( new InitSenderCommonV1( params.M_transport,
                                                     params.M_serializer,
                                                     params.M_stadium,
                                                     2 ) ) )
{

}

InitSenderMonitorV2::InitSenderMonitorV2( const Params & params,
                                          const boost::shared_ptr< InitSenderCommon > common )
    : InitSenderMonitorV1( params, common )
{

}

InitSenderMonitorV2::~InitSenderMonitorV2()
{

}

void
InitSenderMonitorV2::sendServerParams()
{
    dispinfo_t2 disp;

    disp.mode = htons( PARAM_MODE );
    disp.body.sparams = ServerParam::instance().convertToStruct();

    transport().write( reinterpret_cast< const char * >( &disp ),
                       sizeof( dispinfo_t2 ) );
    transport() << std::flush;
}

void
InitSenderMonitorV2::sendPlayerParams()
{
    dispinfo_t2 disp;

    disp.mode = htons( PPARAM_MODE );
    disp.body.pparams = PlayerParam::instance().convertToStruct();

    transport().write( reinterpret_cast< const char * >( &disp ),
                       sizeof( dispinfo_t2 ) );
    transport() << std::flush;
}

void
InitSenderMonitorV2::sendPlayerTypes()
{
    dispinfo_t2 disp;

    disp.mode = htons( PT_MODE );
    for ( int i = 0; i < PlayerParam::instance().playerTypes(); ++i )
    {
        const HeteroPlayer * p = stadium().playerType( i );
        if ( p )
        {
            disp.body.ptinfo = p->convertToStruct( i );
            transport().write( reinterpret_cast< const char * >( &disp ),
                               sizeof( dispinfo_t2 ) );
            transport() << std::flush;
        }
    }
}

/*
//===================================================================
//
//  InitSenderMonitorV3
//
//===================================================================
*/

InitSenderMonitorV3::InitSenderMonitorV3( const Params & params )
    : InitSenderMonitorV2( params,
                         boost::shared_ptr< InitSenderCommon >
                           ( new InitSenderCommonV8( params.M_transport,
                                                     params.M_serializer,
                                                     params.M_stadium,
                                                     999 ) ) )
{
    // The version of the common sender has to be "8".
    // But, the client version is "999" in order to send all parameters.
}

InitSenderMonitorV3::InitSenderMonitorV3( const Params & params,
                                          const boost::shared_ptr< InitSenderCommon > common )
    : InitSenderMonitorV2( params, common )
{

}

InitSenderMonitorV3::~InitSenderMonitorV3()
{

}

void
InitSenderMonitorV3::sendServerParams()
{
    commonSender().sendServerParams();
}

void
InitSenderMonitorV3::sendPlayerParams()
{
    commonSender().sendPlayerParams();
}

void
InitSenderMonitorV3::sendPlayerTypes()
{
    commonSender().sendPlayerTypes();
}

void
InitSenderMonitorV3::sendScore()
{
    serializer().serializeTeam( transport(),
                                stadium().time(),
                                stadium().teamLeft(),
                                stadium().teamRight() );
    transport() << std::ends << std::flush;
}

void
InitSenderMonitorV3::sendPlayMode()
{
    serializer().serializePlayMode( transport(),
                                    stadium().time(),
                                    stadium().playmode() );
    transport() << std::ends << std::flush;
}

namespace initsender {

template< typename Sender >
InitSenderMonitor::Ptr
create( const InitSenderMonitor::Params & params )
{
    return InitSenderMonitor::Ptr( new Sender( params ) );
}

RegHolder v1 = InitSenderMonitor::factory().autoReg( &create< InitSenderMonitorV1 >, 1 );
RegHolder v2 = InitSenderMonitor::factory().autoReg( &create< InitSenderMonitorV2 >, 2 );
RegHolder v3 = InitSenderMonitor::factory().autoReg( &create< InitSenderMonitorV3 >, 3 );
RegHolder v4 = InitSenderMonitor::factory().autoReg( &create< InitSenderMonitorV3 >, 4 );
}

}
