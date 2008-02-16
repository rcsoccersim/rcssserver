// -*-c++-*-

/***************************************************************************
                             initsenderlogger.cpp
               Classes for writing init messages for logger
                             -------------------
    begin                : 2008-02-16
    copyright            : (C) 2008 by The RoboCup Soccer Simulator
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
#include "config.h"
#endif

#include "initsenderlogger.h"

#include "logger.h"
#include "serializermonitor.h"
#include "types.h"

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif

namespace rcss {

/*
//===================================================================
//
//  CLASS: InitSenderLogger
//
//===================================================================
*/

InitSenderLogger::Factory &
InitSenderLogger::factory()
{
    static Factory rval;
    return rval;
}


InitSenderLogger::InitSenderLogger( const Params & params,
                                      const boost::shared_ptr< InitSenderCommon > common )
    : InitSender( params.M_transport, common ),
      M_serializer( params.M_serializer ),
      M_self( params.M_self ),
      M_stadium( params.M_stadium )
{

}

InitSenderLogger::~InitSenderLogger()
{

}

/*
//===================================================================
//
//  CLASS: InitSenderLoggerV1
//
//===================================================================
*/

InitSenderLoggerV1::InitSenderLoggerV1( const Params & params )
    : InitSenderLogger( params,
                         boost::shared_ptr< InitSenderCommon >
                         ( new InitSenderCommonV1( params.M_transport,
                                                   params.M_serializer,
                                                   params.M_stadium,
                                                   1 ) ) )
{

}

InitSenderLoggerV1::InitSenderLoggerV1( const Params & params,
                                          const boost::shared_ptr< InitSenderCommon > common )
    : InitSenderLogger( params, common )
{

}

InitSenderLoggerV1::~InitSenderLoggerV1()
{

}

void
InitSenderLoggerV1::sendHeader()
{

}

void
InitSenderLoggerV1::sendServerParams()
{

}

void
InitSenderLoggerV1::sendPlayerParams()
{

}

void
InitSenderLoggerV1::sendPlayerTypes()
{

}

void
InitSenderLoggerV1::sendPlayMode()
{

}

void
InitSenderLoggerV1::sendTeam()
{

}

/*
//===================================================================
//
//  InitSenderLoggerV2
//
//===================================================================
*/

InitSenderLoggerV2::InitSenderLoggerV2( const Params & params )
    : InitSenderLoggerV1( params,
                           boost::shared_ptr< InitSenderCommon >
                           ( new InitSenderCommonV1( params.M_transport,
                                                     params.M_serializer,
                                                     params.M_stadium,
                                                     (unsigned int)params.M_self.version() ) ) )
{

}

InitSenderLoggerV2::InitSenderLoggerV2( const Params & params,
                                          const boost::shared_ptr< InitSenderCommon > common )
    : InitSenderLoggerV1( params, common )
{

}

InitSenderLoggerV2::~InitSenderLoggerV2()
{

}

void
InitSenderLoggerV2::sendServerParams()
{
    dispinfo_t2 disp;

    disp.mode = htons( PARAM_MODE );
    disp.body.sparams = ServerParam::instance().convertToStruct();

    transport().write( reinterpret_cast< const char* >( &disp ),
                       sizeof( dispinfo_t2 ) );
    transport() << std::flush;
}

void
InitSenderLoggerV2::sendPlayerParams()
{
    dispinfo_t2 disp;

    disp.mode = htons( PPARAM_MODE );
    disp.body.pparams = PlayerParam::instance().convertToStruct();

    transport().write( reinterpret_cast< const  char* >( &disp ),
                       sizeof( dispinfo_t2 ) );
    transport() << std::flush;
}

void
InitSenderLoggerV2::sendPlayerTypes()
{
    dispinfo_t2 disp;

    disp.mode = htons ( PT_MODE );
    for ( int i = 0; i < PlayerParam::instance().playerTypes(); ++i )
    {
        const HeteroPlayer * p = stadium().playerType( i );
        if ( p )
        {
            disp.body.ptinfo = p->convertToStruct( i );
            transport().write( reinterpret_cast< const char* >( &disp ),
                               sizeof( dispinfo_t2 ) );
            transport() << std::flush;
        }
    }
}

/*
//===================================================================
//
//  InitSenderLoggerV3
//
//===================================================================
*/

InitSenderLoggerV3::InitSenderLoggerV3( const Params & params )
    : InitSenderLoggerV2( params,
                         boost::shared_ptr< InitSenderCommon >
                           ( new InitSenderCommonV8( params.M_transport,
                                                     params.M_serializer,
                                                     params.M_stadium,
                                                     999 ) ) )
{
    // The version of the common sender has to be "8".
    // The client version is "999" in order to send all parameters.
}

InitSenderLoggerV3::InitSenderLoggerV3( const Params & params,
                                          const boost::shared_ptr< InitSenderCommon > common )
    : InitSenderLoggerV2( params, common )
{

}

InitSenderLoggerV3::~InitSenderLoggerV3()
{

}

void
InitSenderLoggerV3::sendServerParams()
{
    commonSender().sendServerParams();
}

void
InitSenderLoggerV3::sendPlayerParams()
{
    commonSender().sendPlayerParams();
}

void
InitSenderLoggerV3::sendPlayerTypes()
{
    commonSender().sendPlayerTypes();
}

void
InitSenderLoggerV3::sendScore()
{
    serializer().serializeTeam( transport(),
                                stadium().time(),
                                stadium().teamLeft(),
                                stadium().teamRight() );
    transport() << std::ends << std::flush;
}

void
InitSenderLoggerV3::sendTeamGraphic()
{
    // TODO
    // serializer.serializeTeamGraphic( transport(), ...
}

void
InitSenderLoggerV3::sendPlayMode()
{
    serializer().serializePlayMode( transport(),
                                    stadium().time(),
                                    stadium().playmode() );
    transport() << std::ends << std::flush;
}

namespace initsender {

template< typename Sender >
InitSenderLogger::Ptr
create( const InitSenderLogger::Params & params )
{
    return InitSenderLogger::Ptr( new Sender( params ) );
}

lib::RegHolder vl1 = InitSenderLogger::factory().autoReg( &create< InitSenderLoggerV1 >, 1 );
lib::RegHolder vl2 = InitSenderLogger::factory().autoReg( &create< InitSenderLoggerV2 >, 2 );
lib::RegHolder vl3 = InitSenderLogger::factory().autoReg( &create< InitSenderLoggerV3 >, 3 );
lib::RegHolder vl4 = InitSenderLogger::factory().autoReg( &create< InitSenderLoggerV3 >, 4 );

}
}
