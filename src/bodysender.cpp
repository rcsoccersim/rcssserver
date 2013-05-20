// -*-c++-*-

/***************************************************************************
                                bodysender.cc
                  Classes for sending sense body messages
                             -------------------
    begin                : 22-AUG-2002
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

#include "bodysender.h"

#include "stadium.h"
#include "player.h"
#include "serializer.h"

namespace rcss {

/*!
//===================================================================
//
//  CLASS: BodySender
//
//  DESC: Base class for the sense body protocol.
//
//===================================================================
*/

BodySender::BodySender( std::ostream & transport )
    : Sender( transport )
{

}

BodySender::~BodySender()
{

}

/*!
//===================================================================
//
//  CLASS: BodySenderPlayer
//
//  DESC: Base class for the sense body protocol for players.
//
//===================================================================
*/

BodySenderPlayer::FactoryHolder &
BodySenderPlayer::factory()
{
    static FactoryHolder rval;
    return rval;
}

BodySenderPlayer::BodySenderPlayer( const Params & params )
    : BodySender( params.M_transport ),
      M_serializer( params.M_serializer ),
      M_self( params.M_self )
{

}

BodySenderPlayer::~BodySenderPlayer()
{
    //std::cerr << "delete BodySenderPlayer" << std::endl;
}


/*!
//===================================================================
//
//  CLASS: BodySenderPlayerV1
//
//  DESC: version 1 of the sense body protocol.
//
//===================================================================
*/


BodySenderPlayerV1::BodySenderPlayerV1( const Params & params )
    : BodySenderPlayer( params )
{

}

BodySenderPlayerV1::~BodySenderPlayerV1()
{

}

void
BodySenderPlayerV1::sendBody()
{
    serializer().serializeBodyBegin( transport(), self().stadium().time() );
    sendBodyData();
    serializer().serializeBodyEnd( transport() );
    transport() << std::ends << std::flush;
}

void
BodySenderPlayerV1::sendBodyData()
{
    serializer().serializeBodyViewMode( transport(),
                                        ( self().highQuality()
                                          ? "high"
                                          : "low" ),
                                        ( self().viewWidth() == rcss::pcom::NARROW
                                          ? "narrow"
                                          : self().viewWidth() == rcss::pcom::WIDE
                                          ? "wide"
                                          : "normal" ) );
    serializer().serializeBodyStamina( transport(),
                                       self().stamina(),
                                       self().effort(),
                                       self().staminaCapacity() );
    sendVelocity();
    sendNeck();
    sendCounts();
}

void
BodySenderPlayerV1::sendVelocity()
{
    serializer().serializeBodyVelocity( transport(),
                                        Quantize ( self().vel().r(),
                                                   0.01 ) );
}

void
BodySenderPlayerV1::sendNeck()
{

}

void
BodySenderPlayerV1::sendCounts()
{
    serializer().serializeBodyCounts( transport(),
                                      self().kickCount(),
                                      self().dashCount(),
                                      self().turnCount(),
                                      self().sayCount() );
}

/*!
//===================================================================
//
//  CLASS: BodySenderPlayerV5
//
//  DESC: version 5 of the sense body protocol. Head angle and neck
//        turn count added
//
//===================================================================
*/

BodySenderPlayerV5::BodySenderPlayerV5( const Params & params )
    : BodySenderPlayerV1( params )
{

}

BodySenderPlayerV5::~BodySenderPlayerV5()
{

}

void
BodySenderPlayerV5::sendNeck()
{
    int ang = Rad2IDeg( self().angleNeckCommitted() );
    serializer().serializeNeckAngle( transport(),
                                     ang );
}

void
BodySenderPlayerV5::sendCounts()
{
    BodySenderPlayerV1::sendCounts();
    serializer().serializeNeckCount( transport(),
                                     self().turnNeckCount() );
}

/*!
//===================================================================
//
//  CLASS: BodySenderPlayerV6
//
//  DESC: version 6 of the sense body protocol. Velocity direction
//        added
//
//===================================================================
*/

BodySenderPlayerV6::BodySenderPlayerV6( const Params & params )
    : BodySenderPlayerV5( params )
{

}

BodySenderPlayerV6::~BodySenderPlayerV6()
{

}

void
BodySenderPlayerV6::sendVelocity()
{
    double mag = Quantize( self().vel().r(), 0.01 );
    int head =  Rad2IDeg( normalize_angle
                          ( self().vel().th()
                            - self().angleBodyCommitted()
                            - self().angleNeckCommitted() ) );
    serializer().serializeBodyVelocity( transport(), mag, head );
}

/*!
//===================================================================
//
//  CLASS: BodySenderPlayerV7
//
//  DESC: version 7 of the sense body protocol. Extra counts added
//
//===================================================================
*/

BodySenderPlayerV7::BodySenderPlayerV7( const Params & params )
    : BodySenderPlayerV6( params )
{

}

BodySenderPlayerV7::~BodySenderPlayerV7()
{

}

void
BodySenderPlayerV7::sendCounts()
{
    BodySenderPlayerV6::sendCounts();
    serializer().serializeBodyCounts( transport(),
                                      self().catchCount(),
                                      self().moveCount(),
                                      self().changeViewCount() );
}

/*!
//===================================================================
//
//  CLASS: BodySenderPlayerV8
//
//  DESC: version 8 of the sense body protocol. Added arm, focus and
//        tackle details
//
//===================================================================
*/

BodySenderPlayerV8::BodySenderPlayerV8( const Params & params )
    : BodySenderPlayerV7( params )
{}

BodySenderPlayerV8::~BodySenderPlayerV8()
{}

void
BodySenderPlayerV8::sendBodyData()
{
    BodySenderPlayerV7::sendBodyData();
    Arm::State state = self().arm()
        .getState( rcss::geom::Vector2D( self().pos().x,
                                         self().pos().y ),
                   self().angleBodyCommitted()
                   + self().angleNeckCommitted() );
    serializer().serializeArm( transport(),
                               state.cyclesTillMovable(),
                               state.cyclesTillExpires(),
                               state.dist(),
                               (int)state.head(),
                               state.count() );
    if ( self().getFocusTarget() == NULL )
    {
        serializer().serializeFocus( transport(),
                                     "none",
                                     self().getFocusCount() );
    }
    else
    {
        serializer().serializeFocus( transport(),
                                     ( self().getFocusTarget()->side() == LEFT
                                       ? "l"
                                       : "r" ),
                                     self().getFocusTarget()->unum(),
                                     self().getFocusCount() );
    }
    serializer().serializeTackle( transport(),
                                  self().tackleCycles(),
                                  self().tackleCount() );
}


/*!
//===================================================================
//
//  CLASS: BodySenderPlayerV12
//
//  DESC: version 12 of the sense body protocol. Added collision info.
//
//===================================================================
*/

BodySenderPlayerV12::BodySenderPlayerV12( const Params & params )
    : BodySenderPlayerV8( params )
{

}

BodySenderPlayerV12::~BodySenderPlayerV12()
{

}

void
BodySenderPlayerV12::sendBodyData()
{
    BodySenderPlayerV8::sendBodyData();

    serializer().serializeCollision( transport(),
                                     self().ballCollide(),
                                     self().playerCollide(),
                                     self().postCollide() );
}


/*!
//===================================================================
//
//  CLASS: BodySenderPlayerV13
//
//  DESC: version 13 of the sense body protocol. Added stamina
//        capacity info.
//
//===================================================================
*/

BodySenderPlayerV13::BodySenderPlayerV13( const Params & params )
    : BodySenderPlayerV12( params )
{

}

BodySenderPlayerV13::~BodySenderPlayerV13()
{

}


/*!
//===================================================================
//
//  CLASS: BodySenderPlayerV14
//
//  DESC: version 14 of the sense body protocol. Added foul charged & card info
//
//===================================================================
*/

BodySenderPlayerV14::BodySenderPlayerV14( const Params & params )
    : BodySenderPlayerV13( params )
{

}

BodySenderPlayerV14::~BodySenderPlayerV14()
{

}

void
BodySenderPlayerV14::sendBodyData()
{
    BodySenderPlayerV13::sendBodyData();

    serializer().serializeFoul( transport(), self() );
}



namespace bodysender {

template< typename Sender >
BodySenderPlayer::Ptr
create( const BodySenderPlayer::Params & params )
{
    return BodySenderPlayer::Ptr( new Sender( params ) );
}

RegHolder vp1 = BodySenderPlayer::factory().autoReg( &create< BodySenderPlayerV1 >, 1 );
RegHolder vp2 = BodySenderPlayer::factory().autoReg( &create< BodySenderPlayerV1 >, 2 );
RegHolder vp3 = BodySenderPlayer::factory().autoReg( &create< BodySenderPlayerV1 >, 3 );
RegHolder vp4 = BodySenderPlayer::factory().autoReg( &create< BodySenderPlayerV1 >, 4 );
RegHolder vp5 = BodySenderPlayer::factory().autoReg( &create< BodySenderPlayerV5 >, 5 );
RegHolder vp6 = BodySenderPlayer::factory().autoReg( &create< BodySenderPlayerV6 >, 6 );
RegHolder vp7 = BodySenderPlayer::factory().autoReg( &create< BodySenderPlayerV7 >, 7 );
RegHolder vp8 = BodySenderPlayer::factory().autoReg( &create< BodySenderPlayerV8 >, 8 );
RegHolder vp9 = BodySenderPlayer::factory().autoReg( &create< BodySenderPlayerV8 >, 9 );
RegHolder vp10 = BodySenderPlayer::factory().autoReg( &create< BodySenderPlayerV8 >, 10 );
RegHolder vp11 = BodySenderPlayer::factory().autoReg( &create< BodySenderPlayerV8 >, 11 );
RegHolder vp12 = BodySenderPlayer::factory().autoReg( &create< BodySenderPlayerV12 >, 12 );
RegHolder vp13 = BodySenderPlayer::factory().autoReg( &create< BodySenderPlayerV13 >, 13 );
RegHolder vp14 = BodySenderPlayer::factory().autoReg( &create< BodySenderPlayerV14 >, 14 );
RegHolder vp15 = BodySenderPlayer::factory().autoReg( &create< BodySenderPlayerV14 >, 15 );
//RegHolder vp16 = BodySenderPlayer::factory().autoReg( &create< BodySenderPlayerV14 >, 16 );
}

}
