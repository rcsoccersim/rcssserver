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
 *   Foundation; either version 2 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "bodysender.h"
#include "player.h"
#include "serializer.h"
#include "field.h"

namespace rcss
{

/*!
//===================================================================
//
//  CLASS: BodySender
//
//  DESC: Base class for the sense body protocol.
//
//===================================================================
*/

BodySender::BodySender( std::ostream& transport )
    : Sender( transport )
{}

BodySender::~BodySender()
{}

/*!
//===================================================================
//
//  CLASS: BodySenderPlayer
//
//  DESC: Base class for the sense body protocol for players.
//
//===================================================================
*/

BodySenderPlayer::Factory&
BodySenderPlayer::factory()
{ static Factory rval; return rval; }

BodySenderPlayer::BodySenderPlayer( const Params& params )
    : BodySender( params.m_transport ),
      M_serializer( params.m_ser ),
      M_self( params.m_self )
{}

BodySenderPlayer::~BodySenderPlayer()
{}


/*!
//===================================================================
//
//  CLASS: BodySenderPlayerV1
//
//  DESC: version 1 of the sense body protocol.
//
//===================================================================
*/


BodySenderPlayerV1::BodySenderPlayerV1( const Params& params )
    : BodySenderPlayer( params )
{}

BodySenderPlayerV1::~BodySenderPlayerV1()
{}

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
                                        ( self().highquality()
                                          ? "high"
                                          : "low" ),
                                        ( self().visibleAngle()
                                          == self().defangle*0.5
                                          ? "narrow"
                                          : ( self().visibleAngle()
                                              == self().defangle*2.0
                                              ? "wide"
                                              : "normal" ) ) );
    serializer().serializeBodyStamina( transport(),
                                       self().stamina(),
                                       self().effort() );
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

BodySenderPlayerV5::BodySenderPlayerV5( const Params& params )
    : BodySenderPlayerV1( params )
{}

BodySenderPlayerV5::~BodySenderPlayerV5()
{}

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

BodySenderPlayerV6::BodySenderPlayerV6( const Params& params )
    : BodySenderPlayerV5( params )
{}

BodySenderPlayerV6::~BodySenderPlayerV6()
{}

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

BodySenderPlayerV7::BodySenderPlayerV7( const Params& params )
    : BodySenderPlayerV6( params )
{}

BodySenderPlayerV7::~BodySenderPlayerV7()
{}

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

BodySenderPlayerV8::BodySenderPlayerV8( const Params& params )
    : BodySenderPlayerV7( params )
{}

BodySenderPlayerV8::~BodySenderPlayerV8()
{}

void
BodySenderPlayerV8::sendBodyData()
{
    BodySenderPlayerV7::sendBodyData();
    Arm::State state = self().getArm()
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
    if( self().getFocusTarget() == NULL )
        serializer().serializeFocus( transport(),
                                     "none",
                                     self().getFocusCount() );
    else
        serializer().serializeFocus( transport(),
                                     ( self().getFocusTarget()->team()->side()
                                       == LEFT
                                       ? "l" : "r" ),
                                     self().getFocusTarget()->unum(),
                                     self().getFocusCount() );
    serializer().serializeTackle( transport(),
                                  self().getTackleCycles(),
                                  self().getTackleCount() );
}

namespace bodysender
{
template< typename Sender >
std::auto_ptr< BodySenderPlayer >
create( const BodySenderPlayer::Params& params )
{ return std::auto_ptr< BodySenderPlayer >( new Sender( params ) ); }

lib::RegHolder vp1 = BodySenderPlayer::factory().autoReg( &create< BodySenderPlayerV1 >, 1 );
lib::RegHolder vp2 = BodySenderPlayer::factory().autoReg( &create< BodySenderPlayerV1 >, 2 );
lib::RegHolder vp3 = BodySenderPlayer::factory().autoReg( &create< BodySenderPlayerV1 >, 3 );
lib::RegHolder vp4 = BodySenderPlayer::factory().autoReg( &create< BodySenderPlayerV1 >, 4 );
lib::RegHolder vp5 = BodySenderPlayer::factory().autoReg( &create< BodySenderPlayerV5 >, 5 );
lib::RegHolder vp6 = BodySenderPlayer::factory().autoReg( &create< BodySenderPlayerV6 >, 6 );
lib::RegHolder vp7 = BodySenderPlayer::factory().autoReg( &create< BodySenderPlayerV7 >, 7 );
lib::RegHolder vp8 = BodySenderPlayer::factory().autoReg( &create< BodySenderPlayerV8 >, 8 );
lib::RegHolder vp9 = BodySenderPlayer::factory().autoReg( &create< BodySenderPlayerV8 >, 9 );
lib::RegHolder vp10 = BodySenderPlayer::factory().autoReg( &create< BodySenderPlayerV8 >, 10 );
lib::RegHolder vp11 = BodySenderPlayer::factory().autoReg( &create< BodySenderPlayerV8 >, 11 );
}
}
