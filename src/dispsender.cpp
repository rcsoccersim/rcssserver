// -*-c++-*-

/***************************************************************************
                                dispsender.cpp
                  Classes for sending display messages
                             -------------------
    begin                : 2008-02-14
    copyright            : (C) 2007 by The RoboCup Soccer Simulator
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

#include "dispsender.h"

#include "monitor.h"
#include "serializermonitor.h"
#include "team.h"
#include "player.h"
#include "field.h"

namespace rcss {

/*!
//===================================================================
//
//  CLASS: DispSender
//
//  DESC: Base class for the display protocol.
//
//===================================================================
*/

DispSender::DispSender( std::ostream & transport )
    : Sender( transport )
{

}

DispSender::~DispSender()
{

}

/*!
//===================================================================
//
//  CLASS: DispSenderMonitor
//
//  DESC: Base class for the display protocol for players.
//
//===================================================================
*/

DispSenderMonitor::Factory &
DispSenderMonitor::factory()
{
    static Factory rval;
    return rval;
}

DispSenderMonitor::DispSenderMonitor( const Params & params )
    : DispSender( params.M_transport ),
      M_serializer( params.M_serializer ),
      M_self( params.M_self ),
      M_stadium( params.M_stadium )
{

}

DispSenderMonitor::~DispSenderMonitor()
{

}


/*!
//===================================================================
//
//  CLASS: DispSenderMonitorV1
//
//  DESC: version 1 of the display protocol.
//
//===================================================================
*/

DispSenderMonitorV1::DispSenderMonitorV1( const Params & params )
    : DispSenderMonitor( params )
{

}

DispSenderMonitorV1::~DispSenderMonitorV1()
{

}

void
DispSenderMonitorV1::sendShow()
{

}

void
DispSenderMonitorV1::sendMsg()
{

}

/*!
//===================================================================
//
//  CLASS: DispSenderMonitorV2
//
//  DESC: version 2 of display protocol.
//
//===================================================================
*/

DispSenderMonitorV2::DispSenderMonitorV2( const Params & params )
    : DispSenderMonitorV1( params )
{

}

DispSenderMonitorV2::~DispSenderMonitorV2()
{

}

void
DispSenderMonitorV2::sendShow()
{

}

void
DispSenderMonitorV2::sendMsg()
{

}

/*!
//===================================================================
//
//  CLASS: DispSenderMonitorV3
//
//  DESC: version 3 of display protocol.
//
//===================================================================
*/

DispSenderMonitorV3::DispSenderMonitorV3( const Params & params )
    : DispSenderMonitorV2( params )
{

}

DispSenderMonitorV3::~DispSenderMonitorV3()
{

}

void
DispSenderMonitorV3::sendShow()
{
    serializer().serializeShowBegin( transport(),
                                     stadium().time() );
    serializer().serializeShowPlayMode( transport(),
                                        stadium().playmode() );
    serializer().serializeShowScore( transport(),
                                     stadium().teamLeft().point(),
                                     stadium().teamRight().point(),
                                     stadium().teamLeft().penaltyTaken(),
                                     stadium().teamRight().penaltyTaken(),
                                     stadium().teamLeft().penaltyPoint(),
                                     stadium().teamRight().penaltyPoint() );
    serializer().serializeShowBall( transport(),
                                    stadium().ball().pos(),
                                    stadium().ball().vel() );

    const Stadium::PlayerCont::const_iterator end = stadium().players().end();
    for ( Stadium::PlayerCont::const_iterator p = stadium().players().begin();
          p != end;
          ++p )
    {
        serializer().serializeShowPlayerBegin( transport(),
                                               (*p)->team()->side(),
                                               (*p)->unum(),
                                               (*p)->playerTypeId(),
                                               (*p)->state() );
        serializer().serializeShowPlayerPos( transport(),
                                             (*p)->pos(),
                                             (*p)->vel(),
                                             Rad2Deg( (*p)->angleBodyCommitted() ),
                                             Rad2Deg( (*p)->angleNeckCommitted() ) );

        if ( (*p)->arm().isPointing() )
        {
            rcss::geom::Vector2D arm_dest;
            if ( (*p)->arm().getRelDest( rcss::geom::Vector2D( (*p)->pos().x,
                                                               (*p)->pos().y ),
                                         (*p)->angleBodyCommitted()
                                         + (*p)->angleNeckCommitted(),
                                         arm_dest ) )
            {
                serializer().serializeShowPlayerArm( transport(),
                                                     arm_dest.getMag(),
                                                     Rad2Deg( arm_dest.getHead() ) );
            }
        }

        serializer().serializeShowPlayerViewMode( transport(),
                                                  (*p)->highquality(),
                                                  Rad2Deg( (*p)->visibleAngle() ) );
        serializer().serializeShowPlayerStamina( transport(),
                                                 (*p)->stamina(),
                                                 (*p)->effort(),
                                                 (*p)->recovery() );
        if ( (*p)->state() != DISABLE
             && (*p)->getFocusTarget() != NULL )
        {
            serializer().serializeShowPlayerFocus( transport(),
                                                   (*p)->getFocusTarget()->team()->side(),
                                                   (*p)->getFocusTarget()->unum() );
        }

        serializer().serializeShowPlayerCounts( transport(),
                                                (*p)->kickCount(),
                                                (*p)->dashCount(),
                                                (*p)->turnCount(),
                                                (*p)->catchCount(),
                                                (*p)->moveCount(),
                                                (*p)->turnNeckCount(),
                                                (*p)->changeViewCount(),
                                                (*p)->sayCount(),
                                                (*p)->tackleCount(),
                                                (*p)->arm().getCounter(),
                                                (*p)->attentiontoCount() );

        serializer().serializeShowPlayerEnd( transport() );
    }

    serializer().serializeShowEnd( transport() );

    transport() << std::ends << std::flush;
}

void
DispSenderMonitorV3::sendMsg()
{

}


namespace dispsender {

template< typename Sender >
DispSenderMonitor::Ptr
create( const DispSenderMonitor::Params & params )
{
    return DispSenderMonitor::Ptr( new Sender( params ) );
}

lib::RegHolder vm1 = DispSenderMonitor::factory().autoReg( &create< DispSenderMonitorV1 >, 1 );
lib::RegHolder vm2 = DispSenderMonitor::factory().autoReg( &create< DispSenderMonitorV1 >, 2 );
lib::RegHolder vm3 = DispSenderMonitor::factory().autoReg( &create< DispSenderMonitorV1 >, 3 );

}
}
