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
 *   Foundation; either version 3 of the License, or (at your option) any  *
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
#include "xpmholder.h"

#include <iomanip>

namespace rcss {

/*
//===================================================================
//
//  SerializerMonitorStdv1
//
//===================================================================
*/

SerializerMonitorStdv1::SerializerMonitorStdv1( const SerializerCommon::Ptr common )
    : SerializerMonitor( common )
{

}

SerializerMonitorStdv1::~SerializerMonitorStdv1()
{

}

const
SerializerMonitor::Ptr
SerializerMonitorStdv1::create()
{
    SerializerCommon::Creator cre_common;
    if ( ! SerializerCommon::factory().getCreator( cre_common, 8 ) )
    {
        return SerializerMonitor::Ptr();
    }

    SerializerMonitor::Ptr ptr( new SerializerMonitorStdv1( cre_common() ) );
    return ptr;
}

void
SerializerMonitorStdv1::serializeTeamGraphic( std::ostream & os,
                                              const Side side,
                                              const unsigned int x,
                                              const unsigned int y,
                                              const XPMHolder & xpm ) const
{
    os << "(team_graphic_"
       << ( side == LEFT ? "l" : "r" )
       << " (" << x << " " << y << " "
       << xpm << ")";
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

SerializerMonitorStdv3::SerializerMonitorStdv3( const SerializerCommon::Ptr common )
    : SerializerMonitorStdv1( common )
{

}

SerializerMonitorStdv3::~SerializerMonitorStdv3()
{

}

const
SerializerMonitor::Ptr
SerializerMonitorStdv3::create()
{
    SerializerCommon::Creator cre_common;
    if ( ! SerializerCommon::factory().getCreator( cre_common, 8 ) )
    {
        return SerializerMonitor::Ptr();
    }

    SerializerMonitor::Ptr ptr( new SerializerMonitorStdv3( cre_common() ) );
    return ptr;
}


void
SerializerMonitorStdv3::serializeTeam( std::ostream & os,
                                       const int time, const int /*stime*/,
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
                                           const int time, const int /*stime*/,
                                           const PlayMode pmode ) const
{
    static const char * playmode_strings[] = PLAYMODE_STRINGS;

    os << "(playmode " << time
       << ' ' << playmode_strings[pmode]
       << ')';
}


void
SerializerMonitorStdv3::serializeShowBegin( std::ostream & os,
                                            const int time, const int /*stime*/ ) const
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
    os << " (tm"
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
        os << ' ' << Quantize( player.arm().dest().getX(), PREC )
           << ' ' << Quantize( player.arm().dest().getY(), PREC );
//         rcss::geom::Vector2D arm_dest;
//         if ( player.arm().getRelDest( rcss::geom::Vector2D( player.pos().x,
//                                                             player.pos().y ),
//                                       0.0,
//                                       arm_dest ) )
//         {
//             os << ' ' << Quantize( arm_dest.getMag(), PREC )
//                << ' ' << Quantize( Rad2Deg( arm_dest.getHead() ), DPREC );
//         }
    }
}

void
SerializerMonitorStdv3::serializePlayerViewMode( std::ostream & os,
                                                 const Player & player ) const
{
    os << " (v "
       << ( player.highQuality() ? "h " : "l " )
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
    if ( player.isEnabled()
         && player.getFocusTarget() )
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

/*
//===================================================================
//
//  SerializerMonitorStdv4
//
//===================================================================
*/

SerializerMonitorStdv4::SerializerMonitorStdv4( const SerializerCommon::Ptr common )
    : SerializerMonitorStdv3( common )
{

}

SerializerMonitorStdv4::~SerializerMonitorStdv4()
{

}

const
SerializerMonitor::Ptr
SerializerMonitorStdv4::create()
{
    SerializerCommon::Creator cre_common;
    if ( ! SerializerCommon::factory().getCreator( cre_common, 15 ) )
    {
        return SerializerMonitor::Ptr();
    }

    SerializerMonitor::Ptr ptr( new SerializerMonitorStdv4( cre_common() ) );
    return ptr;
}

void
SerializerMonitorStdv4::serializePlayerStamina( std::ostream & os,
                                                const Player & player ) const
{
    os << " (s "
       << player.stamina() << ' '
       << player.effort() << ' '
       << player.recovery() << ' '
       << player.staminaCapacity() << ')';
}

/*
//===================================================================
//
//  SerializerMonitorJSON (JSON)
//
//===================================================================
*/

#define USE_FLAT_STYLE

SerializerMonitorJSON::SerializerMonitorJSON( const SerializerCommon::Ptr common )
    : SerializerMonitor( common )
{

}

SerializerMonitorJSON::~SerializerMonitorJSON()
{

}

const
SerializerMonitor::Ptr
SerializerMonitorJSON::create()
{
    // The version of JSON-based common serializer is always -1
    SerializerCommon::Creator cre_common;
    if ( ! SerializerCommon::factory().getCreator( cre_common, -1 ) ) // *** versin of common = -1 ***
    {
        return SerializerMonitor::Ptr();
    }

    SerializerMonitor::Ptr ptr( new SerializerMonitorJSON( cre_common() ) );
    return ptr;
}

namespace {

std::string
to_string( const Side side )
{
    return ( side == LEFT
             ? "l"
             : side == RIGHT
             ? "r"
             : "n" );
}

void
time_to_json( std::ostream & os,
              const int time,
              const int stime )
{
    os << std::quoted( "time" ) << ':' << time;
    if ( stime > 0 )
    {
        os << ',';
        os << std::quoted( "stime" ) << ':' << stime;
    }
}

#ifdef USE_FLAT_STYLE

void
team_to_json( std::ostream & os,
              const Team & team )
{
    const std::string side = to_string( team.side() );
    if ( side != "l" && side != "r" )
    {
        return;
    }

    os << '{'
       << std::quoted( "side" ) << ':' << std::quoted( side );

    os << ','
       << std::quoted( "name" ) << ':';
    if ( team.name().empty() ) os << "null"; else os << std::quoted( team.name() );

    os << ','
       << std::quoted( "score" ) << ':' << team.point();
    if ( team.penaltyTaken() > 0 )
    {
        os << ',';
        os << std::quoted( "pen_score" ) << ':' << team.penaltyPoint() << ','
           << std::quoted( "pen_miss" ) << ':' << team.penaltyTaken() - team.penaltyPoint();
    }

    os << '}';
}

void
teams_to_json( std::ostream & os,
               const Team & team_l,
               const Team & team_r )
{
    os << std::quoted( "teams" ) << ':';

    os << '[';
    team_to_json( os, team_l );

    os << ',';
    team_to_json( os, team_r );

    os << ']';
}

}

void
SerializerMonitorJSON::serializeTeam( std::ostream & os,
                                       const int time, const int stime,
                                       const Team & team_l,
                                       const Team & team_r ) const
{
    os << '{';
    os << std::quoted( "type" ) << ':' << std::quoted( "team" );

    os << ',';
    time_to_json( os, time, stime );

    os << ',';
    teams_to_json( os, team_l, team_r );

    os << '}';
}


void
SerializerMonitorJSON::serializePlayMode( std::ostream & os,
                                           const int time, const int stime,
                                           const PlayMode pmode ) const
{
    os << '{';
    os << std::quoted( "type" ) << ':' << std::quoted( "playmode" );

    os << ',';
    time_to_json( os, time, stime );

    os << ',';
    serializePlayModeId( os, pmode );

    os << '}';
}


void
SerializerMonitorJSON::serializeShowBegin( std::ostream & os,
                                            const int time, const int stime ) const
{
    os << '{';
    os << std::quoted( "type" ) << ':'  << std::quoted( "show" );

    os << ',';
    time_to_json( os, time, stime );
}


void
SerializerMonitorJSON::serializeShowEnd( std::ostream & os ) const
{
    os << '}';
}


void
SerializerMonitorJSON::serializePlayModeId( std::ostream & os,
                                             const PlayMode pmode ) const
{
    static const char * playmode_strings[] = PLAYMODE_STRINGS;
    os << std::quoted( "mode" ) << ':' << std::quoted( playmode_strings[pmode] );
}


void
SerializerMonitorJSON::serializeScore( std::ostream & os,
                                        const Team & team_l,
                                        const Team & team_r ) const
{
    teams_to_json( os, team_l, team_r );
}


namespace {
constexpr double POS_PREC = 0.0001;
constexpr double DIR_PREC = 0.001;

#else
void
vec_to_json( std::ostream & os,
             const std::string & name,
             const double x,
             const double y )
{
    os << std::quoted( name ) << ':'
       << '{'
       << std::quoted( "x" ) << ':' << Quantize( x, POS_PREC )
       << ','
       << std::quoted( "y" ) << ':' << Quantize( y, POS_PREC )
       << '}';
}
#endif
}

void
SerializerMonitorJSON::serializeBall( std::ostream & os,
                                       const Ball & ball ) const
{
    os << std::quoted( "ball" ) << ':'
       << '{';
#ifdef USE_FLAT_STYLE
    os << std::quoted( "x" ) << ':' << Quantize( ball.pos().x, POS_PREC )
       << ','
       << std::quoted( "y" ) << ':' << Quantize( ball.pos().y, POS_PREC )
       << ','
       << std::quoted( "vx" ) << ':' << Quantize( ball.vel().x, POS_PREC )
       << ','
       << std::quoted( "vy" ) << ':' << Quantize( ball.vel().y, POS_PREC );
#else
    vec_to_json( os, "pos", ball.pos().x, ball.pos().x );
    os << ',';
    vec_to_json( os, "vel", ball.vel().x, ball.vel().x );
#endif
    os << '}';

}

void
SerializerMonitorJSON::serializePlayerArrayBegin( std::ostream & os ) const
{
    os << std::quoted( "players" ) << ':'
       << '[';
}

void
SerializerMonitorJSON::serializePlayerArrayEnd( std::ostream & os ) const
{
    os << ']';
}

void
SerializerMonitorJSON::serializePlayerBegin( std::ostream & os,
                                              const Player & player ) const
{
    os << '{'
       << std::quoted( "side" ) << ':' << std::quoted( to_string( player.side() ) )
       << ','
       << std::quoted( "unum" ) << ':' << player.unum()
       << ','
       << std::quoted( "type" ) << ':' << player.playerTypeId()
       << ','
       << std::quoted( "state" ) << ':' << player.state();
}


void
SerializerMonitorJSON::serializePlayerEnd( std::ostream & os ) const
{
    os << '}';
}


void
SerializerMonitorJSON::serializePlayerPos( std::ostream & os,
                                            const Player & player ) const
{
#ifdef USE_FLAT_STYLE
    os << ','
       << std::quoted( "x" ) << ':' << Quantize( player.pos().x, POS_PREC )
       << ','
       << std::quoted( "y" ) << ':' << Quantize( player.pos().y, POS_PREC );
    os << ','
       << std::quoted( "vx" ) << ':' << Quantize( player.vel().x, POS_PREC )
       << ','
       << std::quoted( "vy" ) << ':' << Quantize( player.vel().y, POS_PREC );
#else
    os << ',';
    vec_to_json( os, "pos", player.pos().x, player.pos().y );
    os << ',';
    vec_to_json( os, "vel", player.vel().x, player.vel().y );
#endif

    os << ','
       << std::quoted( "body" ) << ':' << Quantize( Rad2Deg( player.angleBodyCommitted() ), DIR_PREC );
    os << ','
       << std::quoted( "neck" ) << ':' << Quantize( Rad2Deg( player.angleNeckCommitted() ), DIR_PREC );
}


void
SerializerMonitorJSON::serializePlayerArm( std::ostream & os,
                                            const Player & player ) const
{
    if ( player.arm().isPointing() )
    {
#ifdef USE_FLAT_STYLE
        os << ','
           << std::quoted( "px" ) << ':' << Quantize( player.arm().dest().getX(), POS_PREC )
           << ','
           << std::quoted( "py" ) << ':' << Quantize( player.arm().dest().getY(), POS_PREC );
#else
        os << ',';
        vec_to_json( os, "arm", player.arm().dest().getX(), player.arm().dest().getY() );
#endif
    }
}


void SerializerMonitorJSON::serializePlayerViewMode( std::ostream & os,
                                                      const Player & player ) const
{
    os << ',';
#ifdef USE_FLAT_STYLE
    os << std::quoted( "vq" ) << ':' << std::quoted( player.highQuality() ? "h" : "l" )
       << ','
       << std::quoted( "vw" ) << ':' << Quantize( Rad2Deg( player.visibleAngle() ), DIR_PREC );
#else
    os << std::quoted( "view" ) << ':'
       << '{'
       << std::quoted( "q" ) << ':' << std::quoted( player.highQuality() ? "h" : "l" )
       << ','
       << std::quoted( "w" ) << ':' << Quantize( Rad2Deg( player.visibleAngle() ), DIR_PREC )
       << '}';
#endif
}


void
SerializerMonitorJSON::serializePlayerStamina( std::ostream & os,
                                               const Player & player ) const
{
    os << ',';
#ifdef USE_FLAT_STYLE
    os << std::quoted( "stamina" ) << ':' << player.stamina()
       << ','
       << std::quoted( "effort" ) << ':' << player.effort()
       << ','
       << std::quoted( "recovery" ) << ':' << player.recovery()
       << ','
       << std::quoted( "capacity" ) << ':' << player.staminaCapacity();
#else
    os << std::quoted( "stamina" ) << ':'
       << '{';
    os << std::quoted( "v" ) << ':' << player.stamina()
       << ','
       << std::quoted( "e" ) << ':' << player.effort()
       << ','
       << std::quoted( "r" ) << ':' << player.recovery()
       << ','
       << std::quoted( "c" ) << ':' << player.staminaCapacity();
    os << '}';
#endif
}


void SerializerMonitorJSON::serializePlayerFocus( std::ostream & os,
                                                   const Player & player ) const
{
    if ( player.isEnabled()
         && player.getFocusTarget() )
    {
        os << ',';
#if 1
        os << std::quoted( "fside" ) << ':' << std::quoted( to_string( player.getFocusTarget()->side() ) )
           << ','
           << std::quoted( "fnum" ) << ':' << player.getFocusTarget()->unum();
#else
        os << std::quoted( "focus" ) << ':'
           << '{'
           << std::quoted( "s" ) << ':' << std::quoted( to_string( player.getFocusTarget()->side() ) )
           << ','
           << std::quoted( "n" ) << ':' << player.getFocusTarget()->unum()
           << '}';
#endif
    }
}


void
SerializerMonitorJSON::serializePlayerCounts( std::ostream & os,
                                               const Player & player ) const

{
    os << ',';
    os << std::quoted( "count" ) << ':'
#if 0
       << '[' << player.kickCount()
       << ',' << player.dashCount()
       << ',' << player.turnCount()
       << ',' << player.catchCount()
       << ',' << player.moveCount()
       << ',' << player.turnNeckCount()
       << ',' << player.changeViewCount()
       << ',' << player.sayCount()
       << ',' << player.tackleCount()
       << ',' << player.arm().getCounter()
       << ',' << player.attentiontoCount()
       << ']';
#else
       << '{'
       << std::quoted( "kick" ) << ':' << player.kickCount()
       << ','
       << std::quoted( "dash" ) << ':' << player.dashCount()
       << ','
       << std::quoted( "turn" ) << ':' << player.turnCount()
       << ','
       << std::quoted( "catch" ) << ':' << player.catchCount()
       << ','
       << std::quoted( "move" ) << ':' << player.moveCount()
       << ','
       << std::quoted( "turn_neck" ) << ':' << player.turnNeckCount()
       << ','
       << std::quoted( "change_view" ) << ':' << player.changeViewCount()
       << ','
       << std::quoted( "say" ) << ':' << player.sayCount()
       << ','
       << std::quoted( "tackle" ) << ':' << player.tackleCount()
       << ','
       << std::quoted( "pointto" ) << ':' << player.arm().getCounter()
       << ','
       << std::quoted( "attentionto" ) << ':' << player.attentiontoCount()
       << '}';
#endif
}


void
SerializerMonitorJSON::serializeTeamGraphic( std::ostream & os,
                                              const Side side,
                                              const unsigned int x,
                                              const unsigned int y,
                                              const XPMHolder & xpm ) const
{
    if ( ! xpm.valid()
         || xpm.colors() >= static_cast< int >( xpm.data().size() ) )
    {
        return;
    }

    os << '{'
       << std::quoted( "type" ) << ':' << std::quoted( "team_graphic" );

    os << ',';
    os << std::quoted( "side" ) << ':' << std::quoted( to_string( side ) );

    os << ',';
#ifdef USE_FLAT_STYLE
    os << std::quoted( "x" ) << ':' << x
       << ','
       << std::quoted( "y" ) << ':' << y;
#else
    os << std::quoted( "index" ) << ':'
       << '{'
       << std::quoted( "x" ) << ':' << x
       << ','
       << std::quoted( "y" ) << ':' << y
       << '}';
#endif

    os << ',';
    os << std::quoted( "xpm" ) << ':'
       << '['; // begin xpm array
    bool first = true;
    for ( std::string str : xpm.data() )
    {
        if ( first ) first = false; else os << ',';

        std::replace( str.begin(), str.end(), '\t', ' ' );
        os << std::quoted( str );
    }
    os << ']'; // end xpm array

    os << '}';
}

void
SerializerMonitorJSON::serializeMsg( std::ostream & os,
                                     const int time, const int stime,
                                     const int board,
                                     const char * msg ) const
{
    os << '{'
       << std::quoted( "type" ) << ':' << std::quoted( "msg" );

    os << ',';
    time_to_json( os, time, stime );

    os << ',';
    os << std::quoted( "board" ) << ':' << board;

    os << ',';
    os << std::quoted( "message" ) << ':' << std::quoted( msg );

    os << '}';

}

/*
//===================================================================
//
//  Factory registration
//
//===================================================================
*/

namespace {
RegHolder v1 = SerializerMonitor::factory().autoReg( &SerializerMonitorStdv1::create, 1 );
RegHolder v2 = SerializerMonitor::factory().autoReg( &SerializerMonitorStdv1::create, 2 );
RegHolder v3 = SerializerMonitor::factory().autoReg( &SerializerMonitorStdv3::create, 3 );
RegHolder v4 = SerializerMonitor::factory().autoReg( &SerializerMonitorStdv4::create, 4 );
RegHolder v5 = SerializerMonitor::factory().autoReg( &SerializerMonitorJSON::create, 5 );
}

}
