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
 *   Foundation; either version 2 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#include "fullstatesender.h"
#include "player.h"
#include "serializer.h"
#include "field.h"

static char *PlayModeString[] = PLAYMODE_STRINGS;


namespace rcss
{

/*!
//===================================================================
//
//  CLASS: FullStateSender
//
//  DESC: Base class for the full state protocol.
//
//===================================================================
*/

FullStateSender::FullStateSender( std::ostream& transport )
    : M_transport( transport )
{}

FullStateSender::~FullStateSender()
{}

/*!
//===================================================================
//
//  CLASS: FullStateObserver
//
//  DESC: Interface for an object that receives full state information.
//
//===================================================================
*/

FullStateObserver::FullStateObserver()
    : M_sender( NULL ),
      M_owns_sender( false )
{}

FullStateObserver::FullStateObserver( FullStateSender& sender )
    : M_sender( &sender ),
      M_owns_sender( false )
{}

FullStateObserver::FullStateObserver( std::auto_ptr< FullStateSender > sender )
    : M_sender( sender.release() ),
      M_owns_sender( true )
{}

FullStateObserver::~FullStateObserver()
{ clear(); }

void
FullStateObserver::setFullStateSender( FullStateSender& sender )
{
    clear();
    M_sender = &sender;
    M_owns_sender = false;
}

void
FullStateObserver::setFullStateSender( std::auto_ptr< FullStateSender > sender )
{
    clear();
    M_sender = sender.release();
    M_owns_sender = true;
}

void
FullStateObserver::sendFullState()
{ M_sender->sendFullState(); }

void
FullStateObserver::clear()
{
    if( M_owns_sender )
    {
        delete M_sender;
        M_sender = NULL;
    }
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

FullStateSenderPlayer::Factory&
FullStateSenderPlayer::factory()
{ static Factory rval; return rval; }

FullStateSenderPlayer::FullStateSenderPlayer( const Params& params )
    : FullStateSender( params.m_transport ),
      M_serializer( params.m_serializer ),
      M_self( params.m_self ),
      M_stadium( params.m_stadium )
{}

FullStateSenderPlayer::~FullStateSenderPlayer()
{}


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


FullStateSenderPlayerV1::FullStateSenderPlayerV1( const Params& params )
    : FullStateSenderPlayer( params )
{}

FullStateSenderPlayerV1::~FullStateSenderPlayerV1()
{}

void
FullStateSenderPlayerV1::sendFullState()
{}

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

FullStateSenderPlayerV5::FullStateSenderPlayerV5( const Params& params )
    : FullStateSenderPlayerV1( params )
{}

FullStateSenderPlayerV5::~FullStateSenderPlayerV5()
{}

void
FullStateSenderPlayerV5::sendFullState()
{
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
                                          PlayModeString[stadium().playmode()] );
    }

    serializer().serializeFSViewMode( transport(),
                                      ( self().highquality()
                                        ? "high"
                                        : "low" ),
                                      ( self().visibleAngle()
                                        == self().defangle/2
                                        ? "narrow"
                                        : ( self().visibleAngle()
                                            == self().defangle*2
                                            ? "wide"
                                            : "normal" ) ) );

    sendSelf();

    sendScore();

    sendBall();

    const Stadium::PlayerCont::const_iterator end = stadium().players().end();
    for ( Stadium::PlayerCont::const_iterator p = stadium().players().begin();
          p != end;
          ++p )
    {
        if ( (*p)->alive == DISABLE )
            continue;

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
FullStateSenderPlayerV5::sendPlayer( const Player& p )
{
    const float quantize_step = .001;
    char side = (p.team()->side() == LEFT) ? 'l' : 'r';
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
    serializer().serializeFSPlayerEnd( transport(),
                                       int( p.stamina() ),
                                       Quantize( p.effort(), .0001 ),
                                       Quantize( p.recovery(), .0001) );
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

FullStateSenderPlayerV8::FullStateSenderPlayerV8( const Params& params )
    : FullStateSenderPlayerV5( params )
{}

FullStateSenderPlayerV8::~FullStateSenderPlayerV8()
{}

void
FullStateSenderPlayerV8::sendSelf()
{
    rcss::geom::Vector2D arm_vec;
    self().getArm().getRelDest ( rcss::geom::Vector2D ( self().pos().x,
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
                               self().getArm().getCyclesTillMovable(),
                               self().getArm().getCyclesTillExpiry(),
                               arm_vec.getMag(), Rad2IDegRound( arm_vec.getHead() ),
                               self().getArm().getCounter() );
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

    if ( self().team()->side() == RIGHT )
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
FullStateSenderPlayerV8::sendPlayer( const Player& p )
{
    char side = (p.team()->side() == LEFT) ? 'l' : 'r';
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

    if( p.getArm().isPointing() )
    {
        rcss::geom::Vector2D arm_vec;
        p.getArm().getRelDest( rcss::geom::Vector2D( p.pos().x, p.pos().y ),
                               p.angleBodyCommitted() + p.angleNeckCommitted(),
                               arm_vec );
        serializer().serializeFSPlayerArm( transport(),
                                           arm_vec.getMag(),
                                           arm_vec.getHead() );
    }

    serializer().serializeFSPlayerEnd( transport(),
                                       p.stamina(),
                                       p.effort(),
                                       p.recovery() );
}
/*!
//===================================================================
//
//  Register senders for different versions
//
//===================================================================
*/

namespace fullstatesender
{
template< typename Sender >
FullStateSender::Ptr
create( const FullStateSenderPlayer::Params& params )
{ return FullStateSender::Ptr( new Sender( params ) ); }

lib::RegHolder vp1 = FullStateSenderPlayer::factory().autoReg( &create< FullStateSenderPlayerV1 >, 1 );
lib::RegHolder vp2 = FullStateSenderPlayer::factory().autoReg( &create< FullStateSenderPlayerV1 >, 2 );
lib::RegHolder vp3 = FullStateSenderPlayer::factory().autoReg( &create< FullStateSenderPlayerV1 >, 3 );
lib::RegHolder vp4 = FullStateSenderPlayer::factory().autoReg( &create< FullStateSenderPlayerV1 >, 4 );
lib::RegHolder vp5 = FullStateSenderPlayer::factory().autoReg( &create< FullStateSenderPlayerV5 >, 5 );
lib::RegHolder vp6 = FullStateSenderPlayer::factory().autoReg( &create< FullStateSenderPlayerV5 >, 6 );
lib::RegHolder vp7 = FullStateSenderPlayer::factory().autoReg( &create< FullStateSenderPlayerV5 >, 7 );
lib::RegHolder vp8 = FullStateSenderPlayer::factory().autoReg( &create< FullStateSenderPlayerV8 >, 8 );
lib::RegHolder vp9 = FullStateSenderPlayer::factory().autoReg( &create< FullStateSenderPlayerV8 >, 9 );
lib::RegHolder vp10 = FullStateSenderPlayer::factory().autoReg( &create< FullStateSenderPlayerV8 >, 10 );
lib::RegHolder vp11 = FullStateSenderPlayer::factory().autoReg( &create< FullStateSenderPlayerV8 >, 11 );
}
}
