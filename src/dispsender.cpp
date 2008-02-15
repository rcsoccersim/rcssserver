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
#include "types.h"

#include <sstream>
#include <cmath>

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif

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
    static dispinfo_t dinfo;
    static int last_sent_time = -1;
    static int last_sent_stoppage_time = -1;

    //
    // send cached data
    //

    if ( stadium().time() == last_sent_time
         && stadium().stoppageTime() == last_sent_stoppage_time )
    {
        transport().write( reinterpret_cast< char * >( &dinfo ),
                           sizeof( dispinfo_t ) );
        transport().flush();
        return;
    }

    last_sent_time = stadium().time();
    last_sent_stoppage_time = stadium().stoppageTime();

    //
    // show mode
    //

    dinfo.mode = htons( SHOW_MODE );

    //
    // playmode
    //

    dinfo.body.show.pmode = static_cast< char >( stadium().playmode() );

    //
    // teams
    //

    if ( ! stadium().teamLeft().name().empty() )
    {
        std::strncpy( dinfo.body.show.team[0].name,
                      stadium().teamLeft().name().c_str(),
                      std::min( stadium().teamLeft().name().length(), size_t( 16 ) ) );
        dinfo.body.show.team[0].score = htons( stadium().teamLeft().point() );
    }
    else
    {
        std::strcpy( dinfo.body.show.team[0].name, "" );
        dinfo.body.show.team[0].score = htons( 0 );
    }

    if ( ! stadium().teamRight().name().empty() )
    {
        std::strncpy( dinfo.body.show.team[1].name,
                      stadium().teamRight().name().c_str(),
                      std::min( stadium().teamRight().name().length(), size_t( 16 ) ) );
        dinfo.body.show.team[1].score = htons( stadium().teamRight().point() );
    }
    else
    {
        std::strcpy( dinfo.body.show.team[1].name, "" );
        dinfo.body.show.team[1].score = htons( 0 );
    }

    //
    // ball
    //

    dinfo.body.show.pos[0].x = htons( (Int16)rint( ( stadium().ball().pos().x * SHOWINFO_SCALE ) ) );
    dinfo.body.show.pos[0].y = htons( (Int16)rint( ( stadium().ball().pos().y * SHOWINFO_SCALE ) ) );

    //
    // players
    //

    const Stadium::PlayerCont & players = stadium().players();
    for ( int i = 0; i < MAX_PLAYER * 2; ++i )
    {
        dinfo.body.show.pos[i+1].enable = htons( players[i]->state() );
        dinfo.body.show.pos[i+1].x = htons( (Int16)rint( players[i]->pos().x * SHOWINFO_SCALE ) );
        dinfo.body.show.pos[i+1].y = htons( (Int16)rint( players[i]->pos().y * SHOWINFO_SCALE ) );
        dinfo.body.show.pos[i+1].angle = htons( (Int16)Rad2IDegRound( players[i]->angleBodyCommitted() ) );
        dinfo.body.show.pos[i+1].side = htons( players[i]->team()->side() );
        dinfo.body.show.pos[i+1].unum = htons( players[i]->unum() );
    }

    //
    // time
    //

    dinfo.body.show.time = htons( stadium().time() );


    //
    // write to stream
    //

    transport().write( reinterpret_cast< char * >( &dinfo ),
                       sizeof( dispinfo_t ) );
    transport().flush();
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
    static dispinfo_t2 dinfo;
    static int last_sent_time = -1;
    static int last_sent_stoppage_time = -1;

    //
    // send cached data
    //

    if ( stadium().time() == last_sent_time
         && stadium().stoppageTime() == last_sent_stoppage_time )
    {
        transport().write( reinterpret_cast< char * >( &dinfo ),
                           sizeof( dispinfo_t2 ) );
        transport().flush();
        return;
    }

    last_sent_time = stadium().time();
    last_sent_stoppage_time = stadium().stoppageTime();

    //
    // show mode
    //

    dinfo.mode = htons( SHOW_MODE );

    //
    // playmode
    //

    dinfo.body.show.pmode = static_cast< char >( stadium().playmode() );

    //
    // teams
    //

    if ( ! stadium().teamLeft().name().empty() )
    {
        std::strncpy( dinfo.body.show.team[0].name,
                      stadium().teamLeft().name().c_str(),
                      std::min( stadium().teamLeft().name().length(), size_t( 16 ) ) );
        dinfo.body.show.team[0].score = htons( stadium().teamLeft().point() );
    }
    else
    {
        std::strcpy( dinfo.body.show.team[0].name, "" );
        dinfo.body.show.team[0].score = htons( 0 );
    }

    if ( ! stadium().teamRight().name().empty() )
    {
        std::strncpy( dinfo.body.show.team[1].name,
                      stadium().teamRight().name().c_str(),
                      std::min( stadium().teamRight().name().length(), size_t( 16 ) ) );
        dinfo.body.show.team[1].score = htons( stadium().teamRight().point() );
    }
    else
    {
        std::strcpy( dinfo.body.show.team[1].name, "" );
        dinfo.body.show.team[1].score = htons( 0 );
    }

    //
    // ball
    //

    dinfo.body.show.ball.x = htonl( (Int32)rint( ( stadium().ball().pos().x * SHOWINFO_SCALE2 ) ) );
    dinfo.body.show.ball.y = htonl( (Int32)rint( ( stadium().ball().pos().y * SHOWINFO_SCALE2 ) ) );
    dinfo.body.show.ball.deltax = htonl( (Int32)rint( ( stadium().ball().vel().x * SHOWINFO_SCALE2 ) ) );
    dinfo.body.show.ball.deltay = htonl( (Int32)rint( ( stadium().ball().vel().y * SHOWINFO_SCALE2) ) );

    //
    // players
    //

    const Stadium::PlayerCont & players = stadium().players();
    for ( int i = 0; i < MAX_PLAYER * 2; ++i )
    {
        dinfo.body.show.pos[i].mode = htons( players[i]->state() );
        dinfo.body.show.pos[i].type = htons( players[i]->playerTypeId() );

        //M_players[i]->alive &= (STAND | GOALIE | DISCARD);
        dinfo.body.show.pos[i].x = htonl( (Int32)rint( players[i]->pos().x * SHOWINFO_SCALE2 ) );
        dinfo.body.show.pos[i].y = htonl( (Int32)rint( players[i]->pos().y * SHOWINFO_SCALE2 ) );
        dinfo.body.show.pos[i].deltax = htonl( (Int32)rint( players[i]->vel().x * SHOWINFO_SCALE2 ) );
        dinfo.body.show.pos[i].deltay = htonl( (Int32)rint( players[i]->vel().y * SHOWINFO_SCALE2 ) );
        dinfo.body.show.pos[i].body_angle = htonl( (Int32)rint( players[i]->angleBodyCommitted() * SHOWINFO_SCALE2 ) );
        dinfo.body.show.pos[i].head_angle = htonl( (Int32)rint( players[i]->angleNeckCommitted() * SHOWINFO_SCALE2 ) );
        dinfo.body.show.pos[i].view_width = htonl( (Int32)rint( players[i]->visibleAngle() * SHOWINFO_SCALE2 ) );
        dinfo.body.show.pos[i].view_quality = htons( (Int16)rint( players[i]->highquality() ) );

        dinfo.body.show.pos[i].stamina = htonl( (Int32)rint( players[i]->stamina() * SHOWINFO_SCALE2 ) );
        dinfo.body.show.pos[i].effort = htonl( (Int32)rint( players[i]->effort() * SHOWINFO_SCALE2 ) );
        dinfo.body.show.pos[i].recovery = htonl( (Int32)rint( players[i]->recovery() * SHOWINFO_SCALE2 ) );

        dinfo.body.show.pos[i].kick_count = htons( players[i]->kickCount() );
        dinfo.body.show.pos[i].dash_count = htons( players[i]->dashCount() );
        dinfo.body.show.pos[i].turn_count = htons( players[i]->turnCount() );
        dinfo.body.show.pos[i].say_count = htons( players[i]->sayCount() );
        dinfo.body.show.pos[i].tneck_count = htons( players[i]->turnNeckCount() );
        dinfo.body.show.pos[i].catch_count = htons( players[i]->catchCount() );
        dinfo.body.show.pos[i].move_count = htons( players[i]->moveCount() );
        dinfo.body.show.pos[i].chg_view_count = htons( players[i]->changeViewCount() );
    }

    //
    // time
    //

    dinfo.body.show.time = htons( stadium().time() );


    //
    // write to stream
    //

    transport().write( reinterpret_cast< char * >( &dinfo ),
                       sizeof( dispinfo_t2 ) );
    transport().flush();
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
    static std::string message;
    static int last_sent_time = -1;
    static int last_sent_stoppage_time = -1;

    //
    // send cached data
    //

    if ( stadium().time() == last_sent_time
         && stadium().stoppageTime() == last_sent_stoppage_time )
    {
        transport() << message << std::ends << std::flush;
        return;
    }

    last_sent_time = stadium().time();
    last_sent_stoppage_time = stadium().stoppageTime();

    message.erase();

    std::ostringstream ostr;

    serializer().serializeShowBegin( ostr,
                                     stadium().time() );
    serializer().serializeShowPlayMode( ostr,
                                        stadium().playmode() );
    serializer().serializeShowScore( ostr,
                                     stadium().teamLeft().point(),
                                     stadium().teamRight().point(),
                                     stadium().teamLeft().penaltyTaken(),
                                     stadium().teamRight().penaltyTaken(),
                                     stadium().teamLeft().penaltyPoint(),
                                     stadium().teamRight().penaltyPoint() );
    serializer().serializeShowBall( ostr,
                                    stadium().ball().pos(),
                                    stadium().ball().vel() );

    const Stadium::PlayerCont::const_iterator end = stadium().players().end();
    for ( Stadium::PlayerCont::const_iterator p = stadium().players().begin();
          p != end;
          ++p )
    {
        serializer().serializeShowPlayerBegin( ostr,
                                               (*p)->team()->side(),
                                               (*p)->unum(),
                                               (*p)->playerTypeId(),
                                               (*p)->state() );
        serializer().serializeShowPlayerPos( ostr,
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
                serializer().serializeShowPlayerArm( ostr,
                                                     arm_dest.getMag(),
                                                     Rad2Deg( arm_dest.getHead() ) );
            }
        }

        serializer().serializeShowPlayerViewMode( ostr,
                                                  (*p)->highquality(),
                                                  Rad2Deg( (*p)->visibleAngle() ) );
        serializer().serializeShowPlayerStamina( ostr,
                                                 (*p)->stamina(),
                                                 (*p)->effort(),
                                                 (*p)->recovery() );
        if ( (*p)->state() != DISABLE
             && (*p)->getFocusTarget() != NULL )
        {
            serializer().serializeShowPlayerFocus( ostr,
                                                   (*p)->getFocusTarget()->team()->side(),
                                                   (*p)->getFocusTarget()->unum() );
        }

        serializer().serializeShowPlayerCounts( ostr,
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

        serializer().serializeShowPlayerEnd( ostr );
    }

    serializer().serializeShowEnd( ostr );

    message = ostr.str();
    transport() << message << std::ends << std::flush;
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
lib::RegHolder vm2 = DispSenderMonitor::factory().autoReg( &create< DispSenderMonitorV2 >, 2 );
lib::RegHolder vm3 = DispSenderMonitor::factory().autoReg( &create< DispSenderMonitorV3 >, 3 );

}
}
