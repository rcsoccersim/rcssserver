// -*-c++-*-

/***************************************************************************
    leg.cpp
    -------------------
    begin                : 23-OCT-2023
    copyright            : (C) 2023 by The RoboCup Soccer Simulator
                           Maintenance Group.
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

#include "leg.h"

#include "player.h"
#include "heteroplayer.h"
#include "serverparam.h"
#include "playerparam.h"

#include <algorithm>
#include <cmath>

namespace {

double
normalize_dash_power( const double p )
{
    return std::clamp( p,
                       ServerParam::instance().minDashPower(),
                       ServerParam::instance().maxDashPower() );
}

double
normalize_dash_angle( const double d )
{
    double dir = std::clamp( d,
                             ServerParam::instance().minDashAngle(),
                             ServerParam::instance().maxDashAngle() );
    if ( ServerParam::instance().dashAngleStep() < EPS )
    {
        // players can dash any direction.
    }
    else
    {
        // The dash direction is discretized by server::dash_angle_step
        dir = ServerParam::instance().dashAngleStep() * rint( dir / ServerParam::instance().dashAngleStep() );
    }
    return dir;
}

}


Leg::Leg( const Player & player )
    : M_player( player ),
      M_command_done( false ),
      M_accel( 0.0, 0.0 ),
      M_consumed_stamina( 0.0 )
{

}

void
Leg::resetCommand()
{
    M_command_done = false;
    M_accel.assign( 0.0, 0.0 );
    M_consumed_stamina = 0.0;
}


void
Leg::dash( const double power,
           const double dir )
{
    if ( M_command_done )
    {
        return;
    }

    const ServerParam & param = ServerParam::instance();

    const double normalized_power = normalize_dash_power( power );
    const double normalized_dir = normalize_dash_angle( dir );
    const bool back_dash = normalized_power < 0.0;
    const double consumed_stamina = 0.5 * std::min( M_player.stamina() + M_player.playerType()->extraStamina(),
                                                    ( back_dash ? normalized_power * -2.0 : normalized_power ) );
    const double actual_power = ( back_dash
                                  ? consumed_stamina / -2.0 * 2.0
                                  : consumed_stamina * 2.0 );
    const double dir_rate = std::clamp( std::fabs( normalized_dir ) > 90.0
                                        ? param.backDashRate() - ( ( param.backDashRate() - param.sideDashRate() )
                                                                   * ( 1.0 - ( std::fabs( dir ) - 90.0 ) / 90.0 ) )
                                        : param.sideDashRate() + ( ( 1.0 - param.sideDashRate() )
                                                                   * ( 1.0 - std::fabs( dir ) / 90.0 ) ),
                                        0.0,
                                        1.0 );
    const double accel_magnitude = std::fabs( M_player.effort()
                                              * actual_power
                                              * dir_rate
                                              * M_player.playerType()->dashPowerRate() )
        / ( M_player.pos().y < 0.0
            ? ( M_player.side() == LEFT
                ? param.slownessOnTopForLeft()
                : param.slownessOnTopForRight() )
            : 1.0 );

    const double accel_dir = ( back_dash
                               ? normalized_dir + 180.0
                               : normalized_dir );

    M_accel = PVector::fromPolar( accel_magnitude,
                                  normalize_angle( M_player.angleBodyCommitted() + Deg2Rad( accel_dir ) ) );
    M_consumed_stamina = consumed_stamina;

    M_command_done = true;
}
