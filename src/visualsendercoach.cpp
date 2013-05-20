// -*-c++-*-

/***************************************************************************
                             visualsendercoach.cpp
              Classes for building visual messages for coaches
                             -------------------
    begin                : 2008-02-17
    copyright            : (C) 2008 by The RoboCup Soccer Server
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

#include "visualsendercoach.h"

#include "stadium.h"
#include "player.h"
#include "coach.h"
#include "serializer.h"

namespace rcss {

/*!
//===================================================================
//
//  CLASS: VisualSenderCoach
//
//  DESC: Base class for the visual protocol for coaches.
//
//===================================================================
*/

VisualSenderCoach::FactoryHolder &
VisualSenderCoach::factory()
{
    static FactoryHolder rval;
    return rval;
}

VisualSenderCoach::VisualSenderCoach( const Params & params )
    : VisualSender( params.M_transport ),
      M_serializer( params.M_serializer ),
      M_self( params.M_self ),
      M_stadium( params.M_stadium )
{

}

VisualSenderCoach::~VisualSenderCoach()
{
    //std::cerr << "delete VisualSenderCoach" << std::endl;
}

/*!
//===================================================================
//
//  CLASS: VisualSenderCoachV1
//
//  DESC:
//
//===================================================================
*/

VisualSenderCoachV1::VisualSenderCoachV1( const Params & params )
    : VisualSenderCoach( params )
{

}

VisualSenderCoachV1::~VisualSenderCoachV1()
{

}

void
VisualSenderCoachV1::sendVisual()
{
    serializer().serializeVisualBegin( transport(), stadium().time() );

    sendGoals();
    sendBall();

    const Stadium::PlayerCont::const_iterator end = stadium().players().end();
    for ( Stadium::PlayerCont::const_iterator p = stadium().players().begin();
          p != end;
          ++p )
    {
        if ( ! (*p)->isEnabled() ) continue;

        serializePlayer( **p );
    }

    serializer().serializeVisualEnd( transport() );
    transport() << std::ends << std::flush;
}

void
VisualSenderCoachV1::sendLook()
{
    serializer().serializeLookBegin( transport(), stadium().time() );

    sendGoals();
    sendBall();

    const Stadium::PlayerCont::const_iterator end = stadium().players().end();
    for ( Stadium::PlayerCont::const_iterator p = stadium().players().begin();
          p != end;
          ++p )
    {
        if ( ! (*p)->isEnabled() ) continue;

        serializePlayerLook( **p );
    }

    serializer().serializeLookEnd( transport() );
    transport() << std::ends << std::flush;
}

void
VisualSenderCoachV1::sendOKEye()
{
    serializer().serializeOKEye( transport(), self().isEyeOn() );
    transport() << std::ends << std::flush;
}

void
VisualSenderCoachV1::sendGoals()
{
    const std::vector< const PObject * >::const_iterator end = stadium().field().goals().end();
    for ( std::vector< const PObject * >::const_iterator it = stadium().field().goals().begin();
          it != end;
          ++it )
    {
        sendGoal( **it );
    }
}


void
VisualSenderCoachV1::sendGoal( const PObject & goal )
{
    serializer().serializeVisualObject( transport(),
                                        calcName( goal ),
                                        goal.pos() );
}

void
VisualSenderCoachV1::sendBall()
{
    serializer().serializeVisualObject( transport(),
                                        calcName( stadium().ball() ),
                                        stadium().ball().pos(),
                                        stadium().ball().vel() );
}

void
VisualSenderCoachV1::serializePlayer( const Player & player )
{
    serializer().serializeVisualObject( transport(),
                                        calcName( player ),
                                        player.pos(),
                                        player.vel(),
                                        rad2Deg( player.angleBodyCommitted() ),
                                        rad2Deg( player.angleNeckCommitted() ) );
}

void
VisualSenderCoachV1::serializePlayerLook( const Player & player )
{
    serializePlayer( player );
}

/*!
//===================================================================
//
//  CLASS: VisualSenderCoachV7
//
//  DESC:
//
//===================================================================
*/

VisualSenderCoachV7::VisualSenderCoachV7( const Params & params )
    : VisualSenderCoachV1( params )
{

}

VisualSenderCoachV7::~VisualSenderCoachV7()
{

}

/*!
//===================================================================
//
//  CLASS: VisualSenderCoachV8
//
//  DESC:
//
//===================================================================
*/

VisualSenderCoachV8::VisualSenderCoachV8( const Params & params )
    : VisualSenderCoachV7( params )
{

}

VisualSenderCoachV8::~VisualSenderCoachV8()
{

}

int
VisualSenderCoachV8::calcPointDir( const Player & player )
{
    double arm_dir = 0.0;
    if ( player.arm().getRelDir( rcss::geom::Vector2D( player.pos().x,
                                                       player.pos().y ),
                                 player.angleBodyCommitted()
                                 + player.angleNeckCommitted(),
                                 arm_dir ) )
    {
        return Rad2IDegRound( normalize_angle( arm_dir
                                               + player.angleNeckCommitted()
                                               + player.angleBodyCommitted() ) );
    }

    return 0;
}

void
VisualSenderCoachV8::serializePlayer( const Player & player )
{
    if ( player.arm().isPointing() )
    {
        serializer().serializeVisualObject( transport(),
                                            calcName( player ),
                                            player.pos(),
                                            player.vel(),
                                            rad2Deg( player.angleBodyCommitted() ),
                                            rad2Deg( player.angleNeckCommitted() ),
                                            calcPointDir( player ),
                                            player.isTackling() );
    }
    else
    {
        serializer().serializeVisualObject( transport(),
                                            calcName( player ),
                                            player.pos(),
                                            player.vel(),
                                            rad2Deg( player.angleBodyCommitted() ),
                                            rad2Deg( player.angleNeckCommitted() ),
                                            player.isTackling() );
    }
}

void
VisualSenderCoachV8::serializePlayerLook( const Player & player )
{
    VisualSenderCoachV7::serializePlayer( player );
}


/*!
//===================================================================
//
//  CLASS: VisualSenderCoachV13
//
//  DESC:
//
//===================================================================
*/

VisualSenderCoachV13::VisualSenderCoachV13( const Params & params )
    : VisualSenderCoachV8( params )
{

}

VisualSenderCoachV13::~VisualSenderCoachV13()
{

}

void
VisualSenderCoachV13::serializePlayer( const Player & player )
{
    if ( player.arm().isPointing() )
    {
        serializer().serializeVisualPlayer( transport(),
                                            player,
                                            calcName( player ),
                                            player.pos(),
                                            player.vel(),
                                            rad2Deg( player.angleBodyCommitted() ),
                                            rad2Deg( player.angleNeckCommitted() ),
                                            calcPointDir( player ) );
    }
    else
    {
        serializer().serializeVisualPlayer( transport(),
                                            player,
                                            calcName( player ),
                                            player.pos(),
                                            player.vel(),
                                            rad2Deg( player.angleBodyCommitted() ),
                                            rad2Deg( player.angleNeckCommitted() ) );
    }
}

void
VisualSenderCoachV13::serializePlayerLook( const Player & player )
{
    VisualSenderCoachV8::serializePlayer( player );
}

/*!
//===================================================================
//
//  Register senders for different versions
//
//===================================================================
*/

namespace visual {
template< typename Sender >
VisualSenderCoach::Ptr
create( const VisualSenderCoach::Params & params )
{
    return VisualSenderCoach::Ptr( new Sender( params ) );
}

RegHolder vc1 = VisualSenderCoach::factory().autoReg( &create< VisualSenderCoachV1 >, 1 );
RegHolder vc2 = VisualSenderCoach::factory().autoReg( &create< VisualSenderCoachV1 >, 2 );
RegHolder vc3 = VisualSenderCoach::factory().autoReg( &create< VisualSenderCoachV1 >, 3 );
RegHolder vc4 = VisualSenderCoach::factory().autoReg( &create< VisualSenderCoachV1 >, 4 );
RegHolder vc5 = VisualSenderCoach::factory().autoReg( &create< VisualSenderCoachV1 >, 5 );
RegHolder vc6 = VisualSenderCoach::factory().autoReg( &create< VisualSenderCoachV1 >, 6 );
RegHolder vc7 = VisualSenderCoach::factory().autoReg( &create< VisualSenderCoachV7 >, 7 );
RegHolder vc8 = VisualSenderCoach::factory().autoReg( &create< VisualSenderCoachV8 >, 8 );
RegHolder vc9 = VisualSenderCoach::factory().autoReg( &create< VisualSenderCoachV8 >, 9 );
RegHolder vc10 = VisualSenderCoach::factory().autoReg( &create< VisualSenderCoachV8 >, 10 );
RegHolder vc11 = VisualSenderCoach::factory().autoReg( &create< VisualSenderCoachV8 >, 11 );
RegHolder vc12 = VisualSenderCoach::factory().autoReg( &create< VisualSenderCoachV8 >, 12 );
RegHolder vc13 = VisualSenderCoach::factory().autoReg( &create< VisualSenderCoachV13 >, 13 );
RegHolder vc14 = VisualSenderCoach::factory().autoReg( &create< VisualSenderCoachV13 >, 14 );
RegHolder vc15 = VisualSenderCoach::factory().autoReg( &create< VisualSenderCoachV13 >, 15 );
//RegHolder vc16 = VisualSenderCoach::factory().autoReg( &create< VisualSenderCoachV13 >, 16 );
}

}
