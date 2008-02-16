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
#include "field.h"
#include "team.h"
#include "serverparam.h"
#include "playerparam.h"
#include "heteroplayer.h"

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
    std::cerr << "InitSenderLoggerV1::sendHeader" << std::endl;
}

void
InitSenderLoggerV1::sendServerParams()
{
    std::cerr << "InitSenderLoggerV1::sendServerParams" << std::endl;
}

void
InitSenderLoggerV1::sendPlayerParams()
{
    std::cerr << "InitSenderLoggerV1::sendPlayerParams" << std::endl;
}

void
InitSenderLoggerV1::sendPlayerTypes()
{
    std::cerr << "InitSenderLoggerV1::sendPlayerTypess" << std::endl;
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
//  CLASS: InitSenderLoggerV2
//
//===================================================================
*/

InitSenderLoggerV2::InitSenderLoggerV2( const Params & params )
    : InitSenderLoggerV1( params,
                          boost::shared_ptr< InitSenderCommon >
                          ( new InitSenderCommonV1( params.M_transport,
                                                    params.M_serializer,
                                                    params.M_stadium,
                                                    1 ) ) )
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
InitSenderLoggerV2::sendHeader()
{
    std::cerr << "InitSenderLoggerV2::sendHeader" << std::endl;
    char header[5];
    header[0] = 'U';
    header[1] = 'L';
    header[2] = 'G';
    header[3] = static_cast< char >( 2 );

    transport().write( header, 4 );
}

void
InitSenderLoggerV2::sendServerParams()
{
    std::cerr << "InitSenderLoggerV2::sendServerParams" << std::endl;
}

void
InitSenderLoggerV2::sendPlayerParams()
{
    std::cerr << "InitSenderLoggerV2::sendPlayerParams" << std::endl;
}

void
InitSenderLoggerV2::sendPlayerTypes()
{
    std::cerr << "InitSenderLoggerV2::sendPlayerTypes" << std::endl;
}

void
InitSenderLoggerV2::sendPlayMode()
{

}

void
InitSenderLoggerV2::sendTeam()
{

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
                           ( new InitSenderCommonV1( params.M_transport,
                                                     params.M_serializer,
                                                     params.M_stadium,
                                                     1 ) ) )
{

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
InitSenderLoggerV3::sendHeader()
{
    std::cerr << "InitSenderLoggerV3::sendHeader" << std::endl;
    char header[5];
    header[0] = 'U';
    header[1] = 'L';
    header[2] = 'G';
    header[3] = static_cast< char >( REC_VERSION_3 );

    transport().write( header, 4 );
}

void
InitSenderLoggerV3::sendServerParams()
{
    std::cerr << "InitSenderLoggerV3::sendServerParams" << std::endl;
    Int16 mode = htons( PARAM_MODE );
    server_params_t param = ServerParam::instance().convertToStruct();

    transport().write( reinterpret_cast< const char * >( &mode ),
                       sizeof( mode ) );
    transport().write( reinterpret_cast< const char * >( &param ),
                       sizeof( param ) );
}

void
InitSenderLoggerV3::sendPlayerParams()
{
    std::cerr << "InitSenderLoggerV3::sendPlayerParams" << std::endl;
    Int16 mode = htons( PPARAM_MODE );
    player_params_t param = PlayerParam::instance().convertToStruct();

    transport().write( reinterpret_cast< const char * >( &mode ),
                       sizeof( mode ) );
    transport().write( reinterpret_cast< const char * >( &param ),
                       sizeof( param ) );
}

void
InitSenderLoggerV3::sendPlayerTypes()
{
    std::cerr << "InitSenderLoggerV3::sendPlayerTypes" << std::endl;
    Int16 mode = htons( PT_MODE );

    for ( int i = 0; i < PlayerParam::instance().playerTypes(); ++i )
    {
        const HeteroPlayer * p = stadium().playerType( i );
        if ( p )
        {
            player_type_t param = p->convertToStruct( i );
            transport().write( reinterpret_cast< const char * >( &mode ),
                               sizeof( mode ) );
            transport().write( reinterpret_cast< const char * >( &param ),
                               sizeof( param ) );
        }
    }
}


void
InitSenderLoggerV3::sendPlayMode()
{
    Int16 mode = htons( PM_MODE );
    char pm = static_cast< char >( stadium().playmode() );

    transport().write( reinterpret_cast< const char * >( &mode ),
                       sizeof( mode ) );
    transport().write( reinterpret_cast< const char * >( &pm ),
                       sizeof( char ) );
}

void
InitSenderLoggerV3::sendTeam()
{
    Int16 mode = htons( TEAM_MODE );
    team_t teams[1];

    std::strncpy( teams[0].name,
                  stadium().teamLeft().name().c_str(),
                  std::min( stadium().teamLeft().name().length() + 1, size_t( 16 ) ) );
    teams[0].score = htons( stadium().teamLeft().point() );

    std::strncpy( teams[1].name,
                  stadium().teamRight().name().c_str(),
                  std::min( stadium().teamRight().name().length() + 1, size_t( 16 ) ) );
    teams[1].score = htons( stadium().teamRight().point() );

    transport().write( reinterpret_cast< const char * >( &mode ),
                       sizeof( mode ) );
    transport().write( reinterpret_cast< const char * >( teams ),
                       sizeof( team_t ) * 2 );
}


/*
//===================================================================
//
//  InitSenderLoggerV4
//
//===================================================================
*/

InitSenderLoggerV4::InitSenderLoggerV4( const Params & params )
    : InitSenderLoggerV3( params,
                         boost::shared_ptr< InitSenderCommon >
                           ( new InitSenderCommonV8( params.M_transport,
                                                     params.M_serializer,
                                                     params.M_stadium,
                                                     999,  // accept all parameters
                                                     true ) ) ) // new line
{
    // The version of the common sender has to be "8".
    // The client version is "999" in order to send all parameters.
}

InitSenderLoggerV4::InitSenderLoggerV4( const Params & params,
                                          const boost::shared_ptr< InitSenderCommon > common )
    : InitSenderLoggerV3( params, common )
{

}

InitSenderLoggerV4::~InitSenderLoggerV4()
{

}

void
InitSenderLoggerV4::sendHeader()
{
    std::cerr << "InitSenderLoggerV4::sendHeader" << std::endl;
    transport() << "URG4" << std::endl;
}

void
InitSenderLoggerV4::sendServerParams()
{
    std::cerr << "InitSenderLoggerV4::sendServerParams" << std::endl;
    commonSender().sendServerParams();
}

void
InitSenderLoggerV4::sendPlayerParams()
{
    std::cerr << "InitSenderLoggerV4::sendPlayerParams" << std::endl;
    commonSender().sendPlayerParams();
}

void
InitSenderLoggerV4::sendPlayerTypes()
{
    std::cerr << "InitSenderLoggerV4::sendPlayerTypes" << std::endl;
    commonSender().sendPlayerTypes();
}


void
InitSenderLoggerV4::sendPlayMode()
{
    serializer().serializePlayMode( transport(),
                                    stadium().time(),
                                    stadium().playmode() );
    transport() << std::endl;
}

void
InitSenderLoggerV4::sendTeam()
{
    serializer().serializeTeam( transport(),
                                stadium().time(),
                                stadium().teamLeft(),
                                stadium().teamRight() );
    transport() << std::endl;
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
lib::RegHolder vl4 = InitSenderLogger::factory().autoReg( &create< InitSenderLoggerV4 >, 4 );

}
}
