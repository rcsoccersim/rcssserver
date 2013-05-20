// -*-c++-*-

/***************************************************************************
                          initsenderonlinecoach.cpp
                  Classes for sending init messages for online coaches
                             -------------------
    begin                : 2008-02-17
    copyright            : (C) 2008 by The RoboCup Soccer Simulator
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

#include "initsenderonlinecoach.h"

#include "serializer.h"
#include "coach.h"
#include "player.h"
#include "stadium.h"

namespace rcss {


/*!
//===================================================================
//
//  CLASS: InitSenderOnlineCoach
//
//  DESC: Base class for the init protocol for Online Coaches.
//
//===================================================================
*/

InitSenderOnlineCoach::FactoryHolder &
InitSenderOnlineCoach::factory()
{
    static FactoryHolder rval;
    return rval;
}


InitSenderOnlineCoach::InitSenderOnlineCoach( const Params& params,
                                              const boost::shared_ptr< InitSenderCommon > common )
    : InitSender( params.M_transport, common ),
      M_serializer( params.M_serializer ),
      M_self( params.M_self ),
      M_stadium( params.M_stadium )
{

}

InitSenderOnlineCoach::~InitSenderOnlineCoach()
{

}

void
InitSenderOnlineCoach::sendPlayerClangVer()
{
    for ( Stadium::PlayerCont::const_iterator i = stadium().remotePlayers().begin();
          i != stadium().remotePlayers().end();
          ++i )
    {
        if ( (*i)->clangMinVer() != 0
             || (*i)->clangMaxVer() != 0 )
        {
            sendPlayerClangVer( **i );
        }
    }
}

/*!
//===================================================================
//
//  CLASS: InitSenderOnlineCoachV1
//
//  DESC: version 1 of the init protocol.
//
//===================================================================
*/


InitSenderOnlineCoachV1::InitSenderOnlineCoachV1( const Params & params )
    : InitSenderOnlineCoach( params,
                             boost::shared_ptr< rcss::InitSenderCommon >
                             ( new InitSenderCommonV1( params.M_transport,
                                                       params.M_serializer,
                                                       params.M_stadium,
                                                       (unsigned int)params.M_self.version() ) ) )
{}

InitSenderOnlineCoachV1::InitSenderOnlineCoachV1( const Params & params,
                                                  const boost::shared_ptr< InitSenderCommon > common )
    : InitSenderOnlineCoach( params, common )
{}

InitSenderOnlineCoachV1::~InitSenderOnlineCoachV1()
{}

void
InitSenderOnlineCoachV1::sendInit()
{
    serializer().serializeInit( transport() );
    transport() << std::ends << std::flush;
}


void
InitSenderOnlineCoachV1::sendChangedPlayers()
{

}

void
InitSenderOnlineCoachV1::sendScore()
{

}

/*!
//===================================================================
//
//  CLASS: InitSenderOnlineCoachV6
//
//  DESC: version 6 of the init protocol.  Coaches now get to know
//        what side they are on.
//
//===================================================================
*/


InitSenderOnlineCoachV6::InitSenderOnlineCoachV6( const Params& params )
    : InitSenderOnlineCoachV1( params )
{

}

InitSenderOnlineCoachV6::InitSenderOnlineCoachV6( const Params& params,
                                                  const boost::shared_ptr< InitSenderCommon > common )
    : InitSenderOnlineCoachV1( params, common )
{

}

InitSenderOnlineCoachV6::~InitSenderOnlineCoachV6()
{

}

void
InitSenderOnlineCoachV6::sendInit()
{
    serializer().serializeInit( transport(), self().side() );
    transport() << std::ends << std::flush;
}

/*!
//===================================================================
//
//  CLASS: InitSenderOnlineCoachV7
//
//  DESC: version 7 of the init protocol.  Coaches now get server
//        parameters, player parameters and player types.
//
//===================================================================
*/


InitSenderOnlineCoachV7::InitSenderOnlineCoachV7( const Params & params )
    : InitSenderOnlineCoachV6( params,
                               boost::shared_ptr< rcss::InitSenderCommon >
                               ( new InitSenderCommonV7( params.M_transport,
                                                         params.M_serializer,
                                                         params.M_stadium,
                                                         (unsigned int)params.M_self.version() ) ) )
{

}

InitSenderOnlineCoachV7::InitSenderOnlineCoachV7( const Params & params,
                                                  const boost::shared_ptr< InitSenderCommon > common )
    : InitSenderOnlineCoachV6( params, common )
{

}

InitSenderOnlineCoachV7::~InitSenderOnlineCoachV7()
{

}

void
InitSenderOnlineCoachV7::sendChangedPlayers()
{
    const Stadium::PlayerCont::const_iterator end = stadium().players().end();
    for ( Stadium::PlayerCont::const_iterator p = stadium().players().begin();
          p != end;
          ++p )
    {
        if ( (*p)->playerTypeId() == 0 ) continue;

        if ( self().side() == (*p)->side() )
        {
            serializer().serializeChangedPlayer( transport(),
                                                 (*p)->unum(),
                                                 (*p)->playerTypeId() );
        }
        else
        {
            serializer().serializeChangedPlayer( transport(),
                                                 (*p)->unum() );
        }
        transport() << std::ends << std::flush;
    }

}

/*!
//===================================================================
//
//  CLASS: InitSenderOnlineCoachV8
//
//  DESC: version 8 of the init protocol.  Coaches now get server
//        parameters, player parameters and player types.
//
//===================================================================
*/


InitSenderOnlineCoachV8::InitSenderOnlineCoachV8( const Params & params )
    : InitSenderOnlineCoachV7( params,
                               boost::shared_ptr< rcss::InitSenderCommon >
                               ( new InitSenderCommonV8( params.M_transport,
                                                         params.M_serializer,
                                                         params.M_stadium,
                                                         (unsigned int)params.M_self.version() ) ) )
{

}

InitSenderOnlineCoachV8::InitSenderOnlineCoachV8( const Params & params,
                                                  const boost::shared_ptr< InitSenderCommon > common )
    : InitSenderOnlineCoachV7( params, common )
{

}

InitSenderOnlineCoachV8::~InitSenderOnlineCoachV8()
{

}

void
InitSenderOnlineCoachV8::sendPlayerClangVer( const Player & player )
{
    serializer().serializePlayerClangVer( transport(),
                                          player.shortName(),
                                          player.clangMinVer(),
                                          player.clangMaxVer() );
    transport() << std::ends << std::flush;
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
InitSenderOnlineCoach::Ptr
create( const InitSenderOnlineCoach::Params & params )
{
    return InitSenderOnlineCoach::Ptr( new Sender( params ) );
}

RegHolder voc1 = InitSenderOnlineCoach::factory().autoReg( &create< InitSenderOnlineCoachV1 >, 1 );
RegHolder voc2 = InitSenderOnlineCoach::factory().autoReg( &create< InitSenderOnlineCoachV1 >, 2 );
RegHolder voc3 = InitSenderOnlineCoach::factory().autoReg( &create< InitSenderOnlineCoachV1 >, 3 );
RegHolder voc4 = InitSenderOnlineCoach::factory().autoReg( &create< InitSenderOnlineCoachV1 >, 4 );
RegHolder voc5 = InitSenderOnlineCoach::factory().autoReg( &create< InitSenderOnlineCoachV1 >, 5 );
RegHolder voc6 = InitSenderOnlineCoach::factory().autoReg( &create< InitSenderOnlineCoachV6 >, 6 );
RegHolder voc7 = InitSenderOnlineCoach::factory().autoReg( &create< InitSenderOnlineCoachV7 >, 7 );
RegHolder voc8 = InitSenderOnlineCoach::factory().autoReg( &create< InitSenderOnlineCoachV8 >, 8 );
RegHolder voc9 = InitSenderOnlineCoach::factory().autoReg( &create< InitSenderOnlineCoachV8 >, 9 );
RegHolder voc10 = InitSenderOnlineCoach::factory().autoReg( &create< InitSenderOnlineCoachV8 >, 10 );
RegHolder voc11 = InitSenderOnlineCoach::factory().autoReg( &create< InitSenderOnlineCoachV8 >, 11 );
RegHolder voc12 = InitSenderOnlineCoach::factory().autoReg( &create< InitSenderOnlineCoachV8 >, 12 );
RegHolder voc13 = InitSenderOnlineCoach::factory().autoReg( &create< InitSenderOnlineCoachV8 >, 13 );
RegHolder voc14 = InitSenderOnlineCoach::factory().autoReg( &create< InitSenderOnlineCoachV8 >, 14 );
RegHolder voc15 = InitSenderOnlineCoach::factory().autoReg( &create< InitSenderOnlineCoachV8 >, 15 );
//RegHolder voc16 = InitSenderOnlineCoach::factory().autoReg( &create< InitSenderOnlineCoachV8 >, 16 );
}

}
