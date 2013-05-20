// -*-c++-*-

/***************************************************************************
                              initsendercoach.cpp
                  Classes for sending init messages for offline coach
                             -------------------
    begin                : 2008-02-17
    copyright            : (C) 2002 by The RoboCup Soccer Simulator
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

#include "initsendercoach.h"

#include "coach.h"
#include "stadium.h"
#include "serializer.h"

namespace rcss {

/*!
//===================================================================
//
//  CLASS: InitSenderOfflineCoach
//
//  DESC: Base class for the init protocol for Offline Coaches.
//
//===================================================================
*/

InitSenderOfflineCoach::FactoryHolder &
InitSenderOfflineCoach::factory()
{
    static FactoryHolder rval;
    return rval;
}


InitSenderOfflineCoach::InitSenderOfflineCoach( const Params & params,
                                                const boost::shared_ptr< InitSenderCommon > common )
    : InitSender( params.M_transport, common ),
      M_serializer( params.M_serializer ),
      M_self( params.M_self ),
      M_stadium( params.M_stadium )
{

}

InitSenderOfflineCoach::~InitSenderOfflineCoach()
{

}


/*!
//===================================================================
//
//  CLASS: InitSenderOfflineCoachV1
//
//  DESC: version 1 of the init protocol.
//
//===================================================================
*/


InitSenderOfflineCoachV1::InitSenderOfflineCoachV1( const Params & params )
    : InitSenderOfflineCoach( params,
                              boost::shared_ptr< InitSenderCommon >
                              ( new InitSenderCommonV1( params.M_transport,
                                                        params.M_serializer,
                                                        params.M_stadium,
                                                        (unsigned int)params.M_self.version() ) ) )
{

}

InitSenderOfflineCoachV1::InitSenderOfflineCoachV1( const Params& params,
                                                    const boost::shared_ptr< InitSenderCommon > common )
    : InitSenderOfflineCoach( params, common )
{

}

InitSenderOfflineCoachV1::~InitSenderOfflineCoachV1()
{

}

void
InitSenderOfflineCoachV1::sendInit()
{
    serializer().serializeInit( transport() );
    transport() << std::ends << std::flush;
}


void
InitSenderOfflineCoachV1::sendChangedPlayers()
{

}

void
InitSenderOfflineCoachV1::sendScore()
{

}

/*!
//===================================================================
//
//  CLASS: InitSenderOfflineCoachV7
//
//  DESC: version 7 of the init protocol.
//
//===================================================================
*/


InitSenderOfflineCoachV7::InitSenderOfflineCoachV7( const Params & params )
    : InitSenderOfflineCoachV1( params,
                                boost::shared_ptr< rcss::InitSenderCommon >
                                ( new InitSenderCommonV7( params.M_transport,
                                                          params.M_serializer,
                                                          params.M_stadium,
                                                          (unsigned int)params.M_self.version() ) ) )
{

}

InitSenderOfflineCoachV7::InitSenderOfflineCoachV7( const Params & params,
                                                    const boost::shared_ptr< InitSenderCommon > common )
    : InitSenderOfflineCoachV1( params, common )
{

}

InitSenderOfflineCoachV7::~InitSenderOfflineCoachV7()
{

}

/*!
//===================================================================
//
//  CLASS: InitSenderOfflineCoachV8
//
//  DESC: version 8 of the init protocol.
//
//===================================================================
*/


InitSenderOfflineCoachV8::InitSenderOfflineCoachV8( const Params & params )
    : InitSenderOfflineCoachV7( params,
                                boost::shared_ptr< rcss::InitSenderCommon >
                                ( new InitSenderCommonV8( params.M_transport,
                                                          params.M_serializer,
                                                          params.M_stadium,
                                                          (unsigned int)params.M_self.version() ) ) )
{

}

InitSenderOfflineCoachV8::InitSenderOfflineCoachV8( const Params & params,
                                                    const boost::shared_ptr< InitSenderCommon > common )
    : InitSenderOfflineCoachV7( params, common )
{

}

InitSenderOfflineCoachV8::~InitSenderOfflineCoachV8()
{

}


/*!
//===================================================================
//
//  Register senders for different versions
//
//===================================================================
*/

namespace initsender {

template< typename Sender >
InitSenderOfflineCoach::Ptr
create( const InitSenderOfflineCoach::Params & params )
{
    return InitSenderOfflineCoach::Ptr( new Sender( params ) );
}

RegHolder vc1 = InitSenderOfflineCoach::factory().autoReg( &create< InitSenderOfflineCoachV1 >, 1 );
RegHolder vc2 = InitSenderOfflineCoach::factory().autoReg( &create< InitSenderOfflineCoachV1 >, 2 );
RegHolder vc3 = InitSenderOfflineCoach::factory().autoReg( &create< InitSenderOfflineCoachV1 >, 3 );
RegHolder vc4 = InitSenderOfflineCoach::factory().autoReg( &create< InitSenderOfflineCoachV1 >, 4 );
RegHolder vc5 = InitSenderOfflineCoach::factory().autoReg( &create< InitSenderOfflineCoachV1 >, 5 );
RegHolder vc6 = InitSenderOfflineCoach::factory().autoReg( &create< InitSenderOfflineCoachV1 >, 6 );
RegHolder vc7 = InitSenderOfflineCoach::factory().autoReg( &create< InitSenderOfflineCoachV7 >, 7 );
RegHolder vc8 = InitSenderOfflineCoach::factory().autoReg( &create< InitSenderOfflineCoachV8 >, 8 );
RegHolder vc9 = InitSenderOfflineCoach::factory().autoReg( &create< InitSenderOfflineCoachV8 >, 9 );
RegHolder vc10 = InitSenderOfflineCoach::factory().autoReg( &create< InitSenderOfflineCoachV8 >, 10 );
RegHolder vc11 = InitSenderOfflineCoach::factory().autoReg( &create< InitSenderOfflineCoachV8 >, 11 );
RegHolder vc12 = InitSenderOfflineCoach::factory().autoReg( &create< InitSenderOfflineCoachV8 >, 12 );
RegHolder vc13 = InitSenderOfflineCoach::factory().autoReg( &create< InitSenderOfflineCoachV8 >, 13 );
RegHolder vc14 = InitSenderOfflineCoach::factory().autoReg( &create< InitSenderOfflineCoachV8 >, 14 );
RegHolder vc15 = InitSenderOfflineCoach::factory().autoReg( &create< InitSenderOfflineCoachV8 >, 15 );
//RegHolder vc16 = InitSenderOfflineCoach::factory().autoReg( &create< InitSenderOfflineCoachV8 >, 16 );
}

}
