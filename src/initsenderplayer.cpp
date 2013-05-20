// -*-c++-*-

/***************************************************************************
                             initsenderplayer.cpp
                  Classes for sending init messages for players
                             -------------------
    begin                : 208-02-17
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

#include "initsenderplayer.h"

#include "serializer.h"
#include "stadium.h"
#include "player.h"
#include "team.h"

namespace rcss {

/*!
//===================================================================
//
//  CLASS: InitSenderPlayer
//
//  DESC: Base class for the init protocol for players.
//
//===================================================================
*/

InitSenderPlayer::FactoryHolder &
InitSenderPlayer::factory()
{
    static FactoryHolder rval;
    return rval;
}


InitSenderPlayer::InitSenderPlayer( const Params & params,
                                    const boost::shared_ptr< InitSenderCommon > common )
    : InitSender( params.M_transport, common ),
      M_serializer( params.M_serializer ),
      M_self( params.M_self ),
      M_stadium( params.M_stadium )
{

}

InitSenderPlayer::~InitSenderPlayer()
{

}





/*!
//===================================================================
//
//  CLASS: InitSenderPlayerV1
//
//  DESC: version 1 of the init protocol.
//
//===================================================================
*/


InitSenderPlayerV1::InitSenderPlayerV1( const Params & params )
    : InitSenderPlayer( params,
                        boost::shared_ptr< rcss::InitSenderCommon >
                        ( new InitSenderCommonV1( params.M_transport,
                                                  params.M_serializer,
                                                  params.M_stadium,
                                                  (unsigned int)params.M_self.version() ) ) )
{

}

InitSenderPlayerV1::InitSenderPlayerV1( const Params& params,
                                        const boost::shared_ptr< InitSenderCommon > common )
    : InitSenderPlayer( params, common )
{

}

InitSenderPlayerV1::~InitSenderPlayerV1()
{

}

void
InitSenderPlayerV1::sendInit()
{
    serializer().serializeInit( transport(),
                                SideStr( self().side() ),
                                self().unum(),
                                stadium().playmode() );
    transport() << std::ends << std::flush;
}

void
InitSenderPlayerV1::sendReconnect()
{
    //          std::cerr << "Sending reconnect" << std::endl;
    //          transport() << "test" << std::ends << std::flush;
    //          std::cerr << "side = " << self().team->side << std::endl;
    //          std::cerr << "mode = " << stadium().mode << std::endl;

    serializer().serializeReconnect( transport(),
                                     SideStr( self().side() ),
                                     stadium().playmode() );
    transport() << std::ends << std::flush;
    //          std::cerr << "Sent" << std::endl;
}

void
InitSenderPlayerV1::sendScore()
{
    int our_score = self().team()->point();
    int opp_score = 0;
    if( self().side() == LEFT
        && stadium().teamRight().enabled() )
    {
        opp_score = stadium().teamRight().point();
    }
    else if( self().side() == RIGHT
             && stadium().teamLeft().enabled() )
    {
        opp_score = stadium().teamLeft().point();
    }
    serializer().serializeScore( transport(),
                                 stadium().time(),
                                 our_score,
                                 opp_score );
    transport() << std::ends << std::flush;
}

void
InitSenderPlayerV1::sendChangedPlayers()
{

}

/*!
//===================================================================
//
//  CLASS: InitSenderPlayerV7
//
//  DESC: version 7 of the init protocol.
//
//===================================================================
*/


InitSenderPlayerV7::InitSenderPlayerV7( const Params & params )
    : InitSenderPlayerV1( params,
                          boost::shared_ptr< rcss::InitSenderCommon >
                          ( new InitSenderCommonV7( params.M_transport,
                                                    params.M_serializer,
                                                    params.M_stadium,
                                                    (unsigned int)params.M_self.version() ) ) )
{

}

InitSenderPlayerV7::InitSenderPlayerV7( const Params & params,
                                        const boost::shared_ptr< InitSenderCommon > common )
    : InitSenderPlayerV1( params, common )
{

}

InitSenderPlayerV7::~InitSenderPlayerV7()
{

}

void
InitSenderPlayerV7::sendChangedPlayers()
{
    const Stadium::PlayerCont::const_iterator end = stadium().players().end();
    for ( Stadium::PlayerCont::const_iterator p = stadium().players().begin();
          p != end;
          ++p )
    {
        if ( (*p)->playerTypeId() == 0 ) continue;

        if ( self().team() == (*p)->team() )
        {
            serializer().serializeChangePlayer( transport(),
                                                (*p)->unum(),
                                                (*p)->playerTypeId() );
        }
        else
        {
            serializer().serializeChangePlayer( transport(),
                                                (*p)->unum() );
        }
        transport() << std::ends << std::flush;
    }

}

/*!
//===================================================================
//
//  CLASS: InitSenderPlayerV8
//
//  DESC: version 8 of the init protocol.
//
//===================================================================
*/


InitSenderPlayerV8::InitSenderPlayerV8( const Params & params )
    : InitSenderPlayerV7( params,
                          boost::shared_ptr< rcss::InitSenderCommon >
                          ( new InitSenderCommonV8( params.M_transport,
                                                    params.M_serializer,
                                                    params.M_stadium,
                                                    (unsigned int)params.M_self.version() ) ) )
{

}

InitSenderPlayerV8::InitSenderPlayerV8( const Params& params,
                                        const boost::shared_ptr< InitSenderCommon > common )
    : InitSenderPlayerV7( params, common )
{

}

InitSenderPlayerV8::~InitSenderPlayerV8()
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
InitSenderPlayer::Ptr
create( const InitSenderPlayer::Params & params )
{
    return InitSenderPlayer::Ptr( new Sender( params ) );
}

RegHolder vp1 = InitSenderPlayer::factory().autoReg( &create< InitSenderPlayerV1 >, 1 );
RegHolder vp2 = InitSenderPlayer::factory().autoReg( &create< InitSenderPlayerV1 >, 2 );
RegHolder vp3 = InitSenderPlayer::factory().autoReg( &create< InitSenderPlayerV1 >, 3 );
RegHolder vp4 = InitSenderPlayer::factory().autoReg( &create< InitSenderPlayerV1 >, 4 );
RegHolder vp5 = InitSenderPlayer::factory().autoReg( &create< InitSenderPlayerV1 >, 5 );
RegHolder vp6 = InitSenderPlayer::factory().autoReg( &create< InitSenderPlayerV1 >, 6 );
RegHolder vp7 = InitSenderPlayer::factory().autoReg( &create< InitSenderPlayerV7 >, 7 );
RegHolder vp8 = InitSenderPlayer::factory().autoReg( &create< InitSenderPlayerV8 >, 8 );
RegHolder vp9 = InitSenderPlayer::factory().autoReg( &create< InitSenderPlayerV8 >, 9 );
RegHolder vp10 = InitSenderPlayer::factory().autoReg( &create< InitSenderPlayerV8 >, 10 );
RegHolder vp11 = InitSenderPlayer::factory().autoReg( &create< InitSenderPlayerV8 >, 11 );
RegHolder vp12 = InitSenderPlayer::factory().autoReg( &create< InitSenderPlayerV8 >, 12 );
RegHolder vp13 = InitSenderPlayer::factory().autoReg( &create< InitSenderPlayerV8 >, 13 );
RegHolder vp14 = InitSenderPlayer::factory().autoReg( &create< InitSenderPlayerV8 >, 14 );
RegHolder vp15 = InitSenderPlayer::factory().autoReg( &create< InitSenderPlayerV8 >, 15 );
//RegHolder vp16 = InitSenderPlayer::factory().autoReg( &create< InitSenderPlayerV8 >, 16 );
}

}
