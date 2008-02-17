// -*-c++-*-

/***************************************************************************
                            serializermonitor.cpp
                  Class for serializing data to monitors
                             -------------------
    begin                : 2007-11-22
    copyright            : (C) 2007 by The RoboCup Soccer Server
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
#include <config.h>
#endif

#include "serializermonitor.h"

#include "param.h"
#include "utility.h"
#include "object.h"
#include "player.h"
#include "team.h"

namespace rcss {

/*
//===================================================================
//
//  SerializerMonitor
//
//===================================================================
*/

SerializerMonitor::Factory &
SerializerMonitor::factory()
{
    static Factory rval;
    return rval;
}


SerializerMonitor::SerializerMonitor( const SerializerCommon & common )
    : Serializer( common )
{

}

SerializerMonitor::~SerializerMonitor()
{

}


/*
//===================================================================
//
//  SerializerMonitorStdv1
//
//===================================================================
*/

SerializerMonitorStdv1::SerializerMonitorStdv1( const SerializerCommon & common )
    : SerializerMonitor( common )
{

}

SerializerMonitorStdv1::~SerializerMonitorStdv1()
{

}

const
SerializerMonitorStdv1 *
SerializerMonitorStdv1::instance()
{
    rcss::SerializerCommon::Creator cre_common;
    if ( ! rcss::SerializerCommon::factory().getCreator( cre_common, 8 ) )
    {
        return NULL;
    }

    static SerializerMonitorStdv1 ser( cre_common() );
    return &ser;
}

/*
//===================================================================
//
//  SerializerMonitorStdv3
//
//===================================================================
*/

const double SerializerMonitorStdv3::PREC = 0.0001;
const double SerializerMonitorStdv3::DPREC = 0.001;

SerializerMonitorStdv3::SerializerMonitorStdv3( const SerializerCommon & common )
    : SerializerMonitorStdv1( common )
{

}

SerializerMonitorStdv3::~SerializerMonitorStdv3()
{

}

const
SerializerMonitorStdv3 *
SerializerMonitorStdv3::instance()
{
    rcss::SerializerCommon::Creator cre_common;
    if ( ! rcss::SerializerCommon::factory().getCreator( cre_common, 8 ) )
    {
        return NULL;
    }

    static SerializerMonitorStdv3 ser( cre_common() );
    return &ser;
}


void
SerializerMonitorStdv3::serializeTeam( std::ostream & os,
                                       const int time,
                                       const Team & team_l,
                                       const Team & team_r ) const
{
    os << "(team " << time
       << ' ' << ( team_l.name().empty() ? "null" : team_l.name().c_str() )
       << ' ' << ( team_r.name().empty() ? "null" : team_r.name().c_str() )
       << ' ' << team_l.point()
       << ' ' << team_r.point();

    if ( team_l.penaltyTaken() > 0 || team_r.penaltyTaken() > 0 )
    {
        os << ' ' << team_l.penaltyPoint()
           << ' ' << team_l.penaltyTaken() - team_l.penaltyPoint()
           << ' ' << team_r.penaltyPoint()
           << ' ' << team_r.penaltyTaken() - team_r.penaltyPoint();
    }
    os << ')';

}

void
SerializerMonitorStdv3::serializePlayMode( std::ostream & os,
                                           const int time,
                                           const PlayMode pmode ) const
{
    static const char * playmode_strings[] = PLAYMODE_STRINGS;

    os << "(playmode " << time
       << ' ' << playmode_strings[pmode]
       << ')';
}


void
SerializerMonitorStdv3::serializeShowBegin( std::ostream & os,
                                            const int time ) const
{
    os << "(show " << time;
}

void
SerializerMonitorStdv3::serializeShowEnd( std::ostream & os ) const
{
    os << ')';
}

void
SerializerMonitorStdv3::serializePlayModeId( std::ostream & os,
                                             const PlayMode pmode ) const
{
    os << " (pm " << pmode << ')';
}

void
SerializerMonitorStdv3::serializeScore( std::ostream & os,
                                        const Team & team_l,
                                        const Team & team_r ) const
{
    os << " (tm "
       << ' ' << ( team_l.name().empty() ? "null" : team_l.name().c_str() )
       << ' ' << ( team_r.name().empty() ? "null" : team_r.name().c_str() )
       << ' ' << team_l.point()
       << ' ' << team_r.point();

    if ( team_l.penaltyTaken() > 0 || team_r.penaltyTaken() > 0 )
    {
        os << ' ' << team_l.penaltyPoint()
           << ' ' << team_l.penaltyTaken() - team_l.penaltyPoint()
           << ' ' << team_r.penaltyPoint()
           << ' ' << team_r.penaltyTaken() - team_r.penaltyPoint();
    }
    os << ')';
}

void
SerializerMonitorStdv3::serializeBall( std::ostream & os,
                                       const Ball & ball ) const
{
    os << " (" << BALL_NAME_SHORT
       << ' ' << Quantize( ball.pos().x, PREC )
       << ' ' << Quantize( ball.pos().y, PREC )
       << ' ' << Quantize( ball.vel().x, PREC )
       << ' ' << Quantize( ball.vel().y, PREC )
       << ')';
}

void
SerializerMonitorStdv3:: serializePlayerBegin( std::ostream & os,
                                               const Player & player ) const
{
    os << " ("
       << '(' << SideStr( player.side() ) << ' ' << player.unum() << ')'
       << ' ' << player.playerTypeId()
       << ' ' << std::hex << std::showbase
       << player.state()
       << std::dec << std::noshowbase;
}

void
SerializerMonitorStdv3::serializePlayerEnd( std::ostream & os ) const
{
    os << ')';
}

void
SerializerMonitorStdv3::serializePlayerPos( std::ostream & os,
                                                const Player & player ) const
{
    os << ' ' << Quantize( player.pos().x, PREC )
       << ' ' << Quantize( player.pos().y, PREC )
       << ' ' << Quantize( player.vel().x, PREC )
       << ' ' << Quantize( player.vel().y, PREC )
       << ' ' << Quantize( Rad2Deg( player.angleBodyCommitted() ), DPREC )
       << ' ' << Quantize( Rad2Deg( player.angleNeckCommitted() ), DPREC );
}

void
SerializerMonitorStdv3::serializePlayerArm( std::ostream & os,
                                            const Player & player ) const
{
    if ( player.arm().isPointing() )
    {
        rcss::geom::Vector2D arm_dest;
        if ( player.arm().getRelDest( rcss::geom::Vector2D( player.pos().x,
                                                            player.pos().y ),
                                      player.angleBodyCommitted() + player.angleNeckCommitted(),
                                      arm_dest ) )
        {
            os << ' ' << Quantize( arm_dest.getMag(), PREC )
               << ' ' << Quantize( Rad2Deg( arm_dest.getHead() ), DPREC );
        }
    }
}

void
SerializerMonitorStdv3::serializePlayerViewMode( std::ostream & os,
                                                 const Player & player ) const
{
    os << " (v "
       << ( player.highquality() ? "h " : "l " )
       << Quantize( Rad2Deg( player.visibleAngle() ), DPREC ) << ')';
}

void
SerializerMonitorStdv3::serializePlayerStamina( std::ostream & os,
                                                const Player & player ) const
{
    os << " (s "
       << player.stamina() << ' '
       << player.effort() << ' '
       << player.recovery() << ')';
}

void
SerializerMonitorStdv3::serializePlayerFocus( std::ostream & os,
                                              const Player & player ) const
{
    if ( player.state() != DISABLE
         && player.getFocusTarget() != NULL )
    {
        os << " (f "
           << SideStr( player.getFocusTarget()->side() ) << ' '
           << player.getFocusTarget()->unum() << ')';
    }
}

void
SerializerMonitorStdv3::serializePlayerCounts( std::ostream & os,
                                               const Player & player ) const
{
     os << " (c "
        << player.kickCount() << ' '
        << player.dashCount() << ' '
        << player.turnCount() << ' '
        << player.catchCount() << ' '
        << player.moveCount() << ' '
        << player.turnNeckCount() << ' '
        << player.changeViewCount() << ' '
        << player.sayCount() << ' '
        << player.tackleCount() << ' '
        << player.arm().getCounter() << ' '
        << player.attentiontoCount() << ')';
}

void
SerializerMonitorStdv3::serializeTeamGraphic( std::ostream & os,
                                              const int x,
                                              const int y,
                                              const char * msg ) const
{
    os << "(team_graphic " << x << ' ' << y
       << " \"" << msg << "\")";
}

namespace {

const
SerializerMonitor *
create1()
{
    return SerializerMonitorStdv1::instance();
}

const
SerializerMonitor *
create3()
{
    return SerializerMonitorStdv3::instance();
}

lib::RegHolder v1 = SerializerMonitor::factory().autoReg( &create1, 1 );
lib::RegHolder v2 = SerializerMonitor::factory().autoReg( &create1, 2 );
lib::RegHolder v3 = SerializerMonitor::factory().autoReg( &create3, 3 );

}
}
