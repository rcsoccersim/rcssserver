// -*-c++-*-

/***************************************************************************
                                fullstatesender.cc
                  Classes for sending full state messages
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

#include "fullstatesender.h"

#include "stadium.h"
#include "player.h"
#include "serializer.h"
#include "team.h"

namespace rcss {

/*!
//===================================================================
//
//  CLASS: FullStateSender
//
//  DESC: Base class for the full state protocol.
//
//===================================================================
*/

FullStateSender::FullStateSender( std::ostream & transport )
    : Sender( transport )
{

}

FullStateSender::~FullStateSender()
{

}

/*!
//===================================================================
//
//  CLASS: FullStateSenderPlayer
//
//  DESC: Base class for the full state protocol for players.
//
//===================================================================
*/

FullStateSenderPlayer::FactoryHolder &
FullStateSenderPlayer::factory()
{
    static FactoryHolder rval;
    return rval;
}

FullStateSenderPlayer::FullStateSenderPlayer( const Params & params )
    : FullStateSender( params.M_transport ),
      M_serializer( params.M_serializer ),
      M_self( params.M_self ),
      M_stadium( params.M_stadium )
{
    //std::cerr << "create FullStateSenderPlayer" << std::endl;
}

FullStateSenderPlayer::~FullStateSenderPlayer()
{
    //std::cerr << "delete FullStateSenderPlayer" << std::endl;
}


/*!
//===================================================================
//
//  CLASS: FullStateSenderPlayerV1
//
//  DESC: version 1 of the full state protocol. No actual information
//        is sent in this version as the protocol did not exist until
//        version 5
//
//===================================================================
*/


FullStateSenderPlayerV1::FullStateSenderPlayerV1( const Params & params )
    : FullStateSenderPlayer( params )
{

}

FullStateSenderPlayerV1::~FullStateSenderPlayerV1()
{

}

void
FullStateSenderPlayerV1::sendFullState()
{

}

/*!
//===================================================================
//
//  CLASS: FullStateSenderPlayerV5
//
//  DESC: version 5 of the full state protocol. This is the first
//        version in which full state information may be sent to
//        players.
//
//===================================================================
*/

FullStateSenderPlayerV5::FullStateSenderPlayerV5( const Params & params )
    : FullStateSenderPlayerV1( params )
{

}

FullStateSenderPlayerV5::~FullStateSenderPlayerV5()
{

}

void
FullStateSenderPlayerV5::sendFullState()
{
    static const char * playmode_string[] = PLAYMODE_STRINGS;

    // send begining of FS
    serializer().serializeFSBegin( transport(), stadium().time() );

    if ( stadium().playmode() == PM_FreeKick_Left
         && stadium().ballCatcher() )
    {
        serializer().serializeFSPlayMode( transport(),
                                          "goalie_catch_ball_l" );
    }
    else if ( stadium().playmode() == PM_FreeKick_Right
              && stadium().ballCatcher() )
    {
        serializer().serializeFSPlayMode( transport(),
                                          "goalie_catch_ball_r" );
    }
    else
    {
        serializer().serializeFSPlayMode( transport(),
                                          playmode_string[stadium().playmode()] );
    }

    serializer().serializeFSViewMode( transport(),
                                      ( self().highQuality()
                                        ? "high"
                                        : "low" ),
                                      ( self().viewWidth() == rcss::pcom::NARROW
                                        ? "narrow"
                                        : self().viewWidth() == rcss::pcom::WIDE
                                        ? "wide"
                                        : "normal" ) );

    sendSelf();

    sendScore();

    sendBall();

    const Stadium::PlayerCont::const_iterator end = stadium().players().end();
    for ( Stadium::PlayerCont::const_iterator p = stadium().players().begin();
          p != end;
          ++p )
    {
        if ( ! (*p)->isEnabled() ) continue;

        sendPlayer( *(*p) );
    }

    // send end of FS
    serializer().serializeFSEnd( transport() );

    transport() << std::ends << std::flush;
}

void
FullStateSenderPlayerV5::sendSelf()
{

}

void
FullStateSenderPlayerV5::sendScore()
{
    int left = 0, right = 0;
    if ( stadium().teamLeft().enabled() )
    {
        left = stadium().teamLeft().point();
    }

    if ( stadium().teamLeft().enabled() )
    {
        left = stadium().teamRight().point();
    }

    serializer().serializeFSScore( transport(),
                                   left,
                                   right );
}

void
FullStateSenderPlayerV5::sendBall()
{
    const float quantize_step = .001;
    serializer().serializeFSBall( transport(),
                                  Quantize( stadium().ball().pos().x,
                                            quantize_step ),
                                  Quantize( stadium().ball().pos().y,
                                            quantize_step ),
                                  Quantize( stadium().ball().vel().x,
                                            quantize_step ),
                                  Quantize( stadium().ball().vel().y,
                                            quantize_step ) );
}

void
FullStateSenderPlayerV5::sendPlayer( const Player & p )
{
    const float quantize_step = .001;
    char side = ( p.side() == LEFT ? 'l' : 'r' );
    serializer().serializeFSPlayerBegin( transport(),
                                         side,
                                         p.unum(),
                                         false, // goalie info not sent
                                         0, // player type info not sent
                                         Quantize( p.pos().x,
                                                   quantize_step ), //pos_x
                                         Quantize( p.pos().y,
                                                   quantize_step ), //pos_y
                                         Quantize( p.vel().x,
                                                   quantize_step ), //vel_x
                                         Quantize( p.vel().y,
                                                   quantize_step ), //vel_y
                                         Quantize( Rad2Deg( p.angleBodyCommitted() ),
                                                   quantize_step ),
                                         Quantize( Rad2Deg( p.angleNeckCommitted() ),
                                                   quantize_step ) ); //neck_angle
    serializer().serializeFSPlayerStamina( transport(),
                                           int( p.stamina() ),
                                           Quantize( p.effort(), .0001 ),
                                           Quantize( p.recovery(), .0001 ),
                                           p.staminaCapacity() );
    serializer().serializeFSPlayerEnd( transport() );
}

/*!
//===================================================================
//
//  CLASS: FullStateSenderPlayerV8
//
//  DESC: version 8 of the full state protocol.
//
//===================================================================
*/

FullStateSenderPlayerV8::FullStateSenderPlayerV8( const Params & params )
    : FullStateSenderPlayerV5( params )
{

}

FullStateSenderPlayerV8::~FullStateSenderPlayerV8()
{

}

void
FullStateSenderPlayerV8::sendSelf()
{
    rcss::geom::Vector2D arm_vec;
    self().arm().getRelDest( rcss::geom::Vector2D ( self().pos().x,
                                                    self().pos().y ),
                             self().angleBodyCommitted()
                             + self().angleNeckCommitted(),
                             arm_vec );
    serializer().serializeFSCounts( transport(),
                                    self().kickCount(),
                                    self().dashCount(),
                                    self().turnCount(),
                                    self().catchCount(),
                                    self().moveCount(),
                                    self().turnNeckCount(),
                                    self().changeViewCount(),
                                    self().sayCount() );
    serializer().serializeArm( transport(),
                               self().arm().getCyclesTillMovable(),
                               self().arm().getCyclesTillExpiry(),
                               arm_vec.getMag(), Rad2IDegRound( arm_vec.getHead() ),
                               self().arm().getCounter() );
}

void
FullStateSenderPlayerV8::sendScore()
{
    int left = 0, right = 0;
    if ( stadium().teamLeft().enabled() )
    {
        left = stadium().teamLeft().point();
    }

    if ( stadium().teamRight().enabled() )
    {
        right = stadium().teamRight().point();
    }

    if ( self().side() == RIGHT )
    {
        std::swap( left, right );
    }

    serializer().serializeFSScore( transport(),
                                   left,
                                   right );
}

void
FullStateSenderPlayerV8::sendBall()
{
    serializer().serializeFSBall( transport(),
                                  stadium().ball().pos().x,
                                  stadium().ball().pos().y,
                                  stadium().ball().vel().x,
                                  stadium().ball().vel().y );
}

void
FullStateSenderPlayerV8::sendPlayer( const Player & p )
{
    char side = ( p.team()->side() == LEFT ? 'l' : 'r' );
    serializer().serializeFSPlayerBegin( transport(),
                                         side,
                                         p.unum(),
                                         p.isGoalie(),
                                         p.playerTypeId(),
                                         p.pos().x,
                                         p.pos().y,
                                         p.vel().x,
                                         p.vel().y,
                                         Rad2Deg( p.angleBodyCommitted() ),
                                         Rad2Deg( p.angleNeckCommitted() ) );

    if ( p.arm().isPointing() )
    {
        rcss::geom::Vector2D arm_vec;
        p.arm().getRelDest( rcss::geom::Vector2D( p.pos().x, p.pos().y ),
                            p.angleBodyCommitted() + p.angleNeckCommitted(),
                            arm_vec );
        serializer().serializeFSPlayerArm( transport(),
                                           arm_vec.getMag(),
                                           arm_vec.getHead() );
    }

    serializer().serializeFSPlayerStamina( transport(),
                                           p.stamina(),
                                           p.effort(),
                                           p.recovery(),
                                           p.staminaCapacity() );

    serializer().serializeFSPlayerEnd( transport() );
}


/*!
//===================================================================
//
//  CLASS: FullStateSenderPlayerV13
//
//  DESC: version 13 of the full state protocol. The tackling or
//  kikcing information are added.
//
//===================================================================
*/

FullStateSenderPlayerV13::FullStateSenderPlayerV13( const Params & params )
    : FullStateSenderPlayerV8( params )
{

}

FullStateSenderPlayerV13::~FullStateSenderPlayerV13()
{

}


void
FullStateSenderPlayerV13::sendPlayer( const Player & p )
{
    char side = ( p.team()->side() == LEFT ? 'l' : 'r' );
    serializer().serializeFSPlayerBegin( transport(),
                                         side,
                                         p.unum(),
                                         p.isGoalie(),
                                         p.playerTypeId(),
                                         p.pos().x,
                                         p.pos().y,
                                         p.vel().x,
                                         p.vel().y,
                                         Rad2Deg( p.angleBodyCommitted() ),
                                         Rad2Deg( p.angleNeckCommitted() ) );

    if ( p.arm().isPointing() )
    {
        rcss::geom::Vector2D arm_vec;
        p.arm().getRelDest( rcss::geom::Vector2D( p.pos().x, p.pos().y ),
                            p.angleBodyCommitted() + p.angleNeckCommitted(),
                            arm_vec );
        serializer().serializeFSPlayerArm( transport(),
                                           arm_vec.getMag(),
                                           arm_vec.getHead() );
    }

    serializer().serializeFSPlayerStamina( transport(),
                                           p.stamina(),
                                           p.effort(),
                                           p.recovery(),
                                           p.staminaCapacity() );

    serializer().serializeFSPlayerState( transport(), p );

    serializer().serializeFSPlayerEnd( transport() );
}


/*!
//===================================================================
//
//  Register senders for different versions
//
//===================================================================
*/

namespace fullstatesender {

template< typename Sender >
FullStateSenderPlayer::Ptr
create( const FullStateSenderPlayer::Params & params )
{
    return FullStateSenderPlayer::Ptr( new Sender( params ) );
}

RegHolder vp1 = FullStateSenderPlayer::factory().autoReg( &create< FullStateSenderPlayerV1 >, 1 );
RegHolder vp2 = FullStateSenderPlayer::factory().autoReg( &create< FullStateSenderPlayerV1 >, 2 );
RegHolder vp3 = FullStateSenderPlayer::factory().autoReg( &create< FullStateSenderPlayerV1 >, 3 );
RegHolder vp4 = FullStateSenderPlayer::factory().autoReg( &create< FullStateSenderPlayerV1 >, 4 );
RegHolder vp5 = FullStateSenderPlayer::factory().autoReg( &create< FullStateSenderPlayerV5 >, 5 );
RegHolder vp6 = FullStateSenderPlayer::factory().autoReg( &create< FullStateSenderPlayerV5 >, 6 );
RegHolder vp7 = FullStateSenderPlayer::factory().autoReg( &create< FullStateSenderPlayerV5 >, 7 );
RegHolder vp8 = FullStateSenderPlayer::factory().autoReg( &create< FullStateSenderPlayerV8 >, 8 );
RegHolder vp9 = FullStateSenderPlayer::factory().autoReg( &create< FullStateSenderPlayerV8 >, 9 );
RegHolder vp10 = FullStateSenderPlayer::factory().autoReg( &create< FullStateSenderPlayerV8 >, 10 );
RegHolder vp11 = FullStateSenderPlayer::factory().autoReg( &create< FullStateSenderPlayerV8 >, 11 );
RegHolder vp12 = FullStateSenderPlayer::factory().autoReg( &create< FullStateSenderPlayerV8 >, 12 );
RegHolder vp13 = FullStateSenderPlayer::factory().autoReg( &create< FullStateSenderPlayerV13 >, 13 );
RegHolder vp14 = FullStateSenderPlayer::factory().autoReg( &create< FullStateSenderPlayerV13 >, 14 );
RegHolder vp15 = FullStateSenderPlayer::factory().autoReg( &create< FullStateSenderPlayerV13 >, 15 );
//RegHolder vp16 = FullStateSenderPlayer::factory().autoReg( &create< FullStateSenderPlayerV13 >, 16 );
}

}
