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
 *   Foundation; either version 2 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "initsendercoach.h"

#include "coach.h"
#include "field.h"
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

InitSenderOfflineCoach::Factory &
InitSenderOfflineCoach::factory()
{
    static Factory rval;
    return rval;
}


InitSenderOfflineCoach::InitSenderOfflineCoach( const Params & params,
                                                const boost::shared_ptr< InitSenderCommon > common )
    : InitSender( params.m_transport, common ),
      M_serializer( params.m_ser ),
      M_self( params.m_self ),
      M_stadium( params.m_stadium )
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
                              ( new InitSenderCommonV1( params.m_transport,
                                                        params.m_ser,
                                                        params.m_stadium,
                                                        (unsigned int)params.m_self.version() ) ) )
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
                                ( new InitSenderCommonV7( params.m_transport,
                                                          params.m_ser,
                                                          params.m_stadium,
                                                          (unsigned int)params.m_self.version() ) ) )
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
                                ( new InitSenderCommonV8( params.m_transport,
                                                          params.m_ser,
                                                          params.m_stadium,
                                                          (unsigned int)params.m_self.version() ) ) )
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

lib::RegHolder vc1 = InitSenderOfflineCoach::factory().autoReg( &create< InitSenderOfflineCoachV1 >, 1 );
lib::RegHolder vc2 = InitSenderOfflineCoach::factory().autoReg( &create< InitSenderOfflineCoachV1 >, 2 );
lib::RegHolder vc3 = InitSenderOfflineCoach::factory().autoReg( &create< InitSenderOfflineCoachV1 >, 3 );
lib::RegHolder vc4 = InitSenderOfflineCoach::factory().autoReg( &create< InitSenderOfflineCoachV1 >, 4 );
lib::RegHolder vc5 = InitSenderOfflineCoach::factory().autoReg( &create< InitSenderOfflineCoachV1 >, 5 );
lib::RegHolder vc6 = InitSenderOfflineCoach::factory().autoReg( &create< InitSenderOfflineCoachV1 >, 6 );
lib::RegHolder vc7 = InitSenderOfflineCoach::factory().autoReg( &create< InitSenderOfflineCoachV7 >, 7 );
lib::RegHolder vc8 = InitSenderOfflineCoach::factory().autoReg( &create< InitSenderOfflineCoachV8 >, 8 );
lib::RegHolder vc9 = InitSenderOfflineCoach::factory().autoReg( &create< InitSenderOfflineCoachV8 >, 9 );
lib::RegHolder vc10 = InitSenderOfflineCoach::factory().autoReg( &create< InitSenderOfflineCoachV8 >, 10 );
lib::RegHolder vc11 = InitSenderOfflineCoach::factory().autoReg( &create< InitSenderOfflineCoachV8 >, 11 );
lib::RegHolder vc12 = InitSenderOfflineCoach::factory().autoReg( &create< InitSenderOfflineCoachV8 >, 12 );

}
}
