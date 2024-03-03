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
      M_command_type( NONE ),
      M_kick_power( 0.0 ),
      M_kick_dir( 0.0 ),
      M_dash_power( 0.0 ),
      M_dash_dir( 0.0 )
{

}

void
Leg::resetCommand()
{
    M_command_type = NONE;
    M_kick_power = 0.0;
    M_kick_dir = 0.0;
    M_dash_power = 0.0;
    M_dash_dir = 0.0;
}

void
Leg::move()
{
    if ( commandDone() )
    {
        return;
    }

    M_command_type = MOVE;
}

void
Leg::kick( const double power,
           const double dir )
{
    if ( commandDone() )
    {
        return;
    }

    M_kick_power = power;
    M_kick_dir = dir;

    M_command_type = KICK;
}

void
Leg::turn()
{
    if ( commandDone() )
    {
        return;
    }

    M_command_type = TURN;
}


void
Leg::dash( const double power,
           const double dir )
{
    if ( commandDone() )
    {
        return;
    }

    M_dash_power = normalize_dash_power( power );
    M_dash_dir = normalize_dash_angle( dir );

    M_command_type = DASH;
}

void
Leg::tackle()
{
    if ( commandDone() )
    {
        return;
    }

    M_command_type = TACKLE;
}

PVector
Leg::calcDashAccel( const double consumed_stamina ) const
{
    const ServerParam & param = ServerParam::instance();

    const double power = normalize_dash_power( dashPower() < 0.0 ? -consumed_stamina : consumed_stamina * 2.0 );

    const double dir_rate = std::clamp( std::fabs( dashDir() ) > 90.0
                                        ? param.backDashRate() - ( ( param.backDashRate() - param.sideDashRate() )
                                                                   * ( 1.0 - ( std::fabs( dashDir() ) - 90.0 ) / 90.0 ) )
                                        : param.sideDashRate() + ( ( 1.0 - param.sideDashRate() )
                                                                   * ( 1.0 - std::fabs( dashDir() ) / 90.0 ) ),
                                        0.0,
                                        1.0 );
    double accel_magnitude = std::fabs( M_player.effort() * power * dir_rate * M_player.playerType()->dashPowerRate() );

    if ( M_player.pos().y < 0.0 )
    {
        accel_magnitude /= ( M_player.side() == LEFT
                             ? param.slownessOnTopForLeft()
                             : param.slownessOnTopForRight() );
    }

    PVector accel = PVector::fromPolar( accel_magnitude,
                                        normalize_angle( M_player.angleBodyCommitted() + Deg2Rad( dashDir() ) ) );
    if ( power < 0.0 )
    {
        accel = -accel;
    }

    return accel;
}
