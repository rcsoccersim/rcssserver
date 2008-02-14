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
SerializerMonitorStdv3::serializeScore( std::ostream & os,
                                        const int time,
                                        const std::string & left_name,
                                        const std::string & right_name,
                                        const int left_point,
                                        const int right_point,
                                        const int left_pen_taken,
                                        const int right_pen_taken,
                                        const int left_pen_point,
                                        const int right_pen_point ) const
{
    os << "(team " << time
       << ' ' << ( left_name.empty() ? "null" : left_name.c_str() )
       << ' ' << ( right_name.empty() ? "null" : right_name.c_str() )
       << ' ' << left_point
       << ' ' << right_point;

    if ( left_pen_taken > 0 || right_pen_taken > 0 )
    {
        os << ' ' << left_pen_point
           << ' ' << left_pen_taken - left_pen_point
           << ' ' << right_pen_point
           << ' ' << right_pen_taken - right_pen_point;
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
SerializerMonitorStdv3::serializeShowPlayMode( std::ostream & os,
                                               const PlayMode pmode ) const
{
    os << " (pm " << pmode << ')';
}

void
SerializerMonitorStdv3::serializeShowScore( std::ostream & os,
                                            const int score_l,
                                            const int score_r,
                                            const int pen_taken_l,
                                            const int pen_taken_r,
                                            const int pen_score_l,
                                            const int pen_score_r ) const
{
    os << " (score "
       << ' ' << score_l
       << ' ' << score_r;
    if ( pen_taken_l > 0
         || pen_taken_r > 0 )
    {
        os << ' ' << pen_score_l
           << ' ' << pen_taken_l - pen_score_l
           << ' ' << pen_score_r
           << ' ' << pen_taken_r - pen_score_r;
    }
    os << ')';
}

void
SerializerMonitorStdv3::serializeShowBall( std::ostream & os,
                                           const PVector & pos,
                                           const PVector & vel ) const
{
    os << " (" << BALL_NAME_SHORT
       << ' ' << Quantize( pos.x, PREC )
       << ' ' << Quantize( pos.y, PREC )
       << ' ' << Quantize( vel.x, PREC )
       << ' ' << Quantize( vel.y, PREC )
       << ')';
}

void
SerializerMonitorStdv3:: serializeShowPlayerBegin( std::ostream & os,
                                                   const Side side,
                                                   const int unum,
                                                   const int type,
                                                   const int state ) const
{
    os << " ("
       << '(' << SideStr( side )
       << ' ' << unum
       << ')'
       << ' ' << type
       << ' ' << std::hex << std::showbase
       << state
       << std::dec << std::noshowbase;
}

void
SerializerMonitorStdv3::serializeShowPlayerEnd( std::ostream & os ) const
{
    os << ')';
}

void
SerializerMonitorStdv3::serializeShowPlayerPos( std::ostream & os,
                                                const PVector & pos,
                                                const PVector & vel,
                                                const double & body,
                                                const double & neck ) const
{
    os << ' ' << Quantize( pos.x, PREC )
       << ' ' << Quantize( pos.y, PREC )
       << ' ' << Quantize( vel.x, PREC )
       << ' ' << Quantize( vel.y, PREC )
       << ' ' << Quantize( body, DPREC )
       << ' ' << Quantize( neck, DPREC );
}

void
SerializerMonitorStdv3::serializeShowPlayerArm( std::ostream & os,
                                                const double & arm_dist,
                                                const double & arm_dir ) const
{
    os << ' ' << Quantize( arm_dist, PREC )
       << ' ' << Quantize( arm_dir, DPREC );
}

void
SerializerMonitorStdv3::serializeShowPlayerViewMode( std::ostream & os,
                                                     const bool high_quality,
                                                     const double & vis_angle ) const
{
    os << " (v "
           << ( high_quality ? "h " : "l " )
           << Quantize( vis_angle, DPREC )
           << ')';
}

void
SerializerMonitorStdv3::serializeShowPlayerStamina( std::ostream & os,
                                                    const double & stamina,
                                                    const double & effort,
                                                    const double & recovery ) const
{
    os << " (s " << stamina << ' ' << effort << ' ' << recovery << ')';
}

void
SerializerMonitorStdv3::serializeShowPlayerFocus( std::ostream & os,
                                                  const Side side,
                                                  const int unum ) const
{
    os << " (f " << SideStr( side ) << ' ' << unum << ')';
}

void
SerializerMonitorStdv3::serializeShowPlayerCounts( std::ostream & os,
                                                   const int kick,
                                                   const int dash,
                                                   const int turn,
                                                   const int goalie_catch,
                                                   const int move,
                                                   const int turn_neck,
                                                   const int change_view,
                                                   const int say,
                                                   const int tackle,
                                                   const int pointto,
                                                   const int attentionto ) const
{
    os << " (c "
       << kick << ' '
       << dash << ' '
       << turn << ' '
       << goalie_catch << ' '
       << move << ' '
       << turn_neck << ' '
       << change_view << ' '
       << say << ' '
       << tackle << ' '
       << pointto << ' '
       << attentionto << ')';
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
