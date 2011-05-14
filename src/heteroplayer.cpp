/* -*- Mode: C++ -*- */

/*
 *Copyright:

 Copyright (C) 1996-2000 Electrotechnical Laboratory.
 Itsuki Noda, Yasuo Kuniyoshi and Hitoshi Matsubara.
 Copyright (C) 2000, 2001 RoboCup Soccer Server Maintainance Group.
 Patrick Riley, Tom Howard, Daniel Polani, Itsuki Noda,
 Mikhail Prokopenko, Jan Wendler
 Copyright (C) 2002- RoboCup Soccer Simulator Maintainance Group.

 This file is a part of SoccerServer.

 This code is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 3 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 *EndCopyright:
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "heteroplayer.h"

#include "serverparam.h"
#include "playerparam.h"
#include "utility.h"

#include <boost/random.hpp>

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h> /* needed for htonl, htons, ... */
#endif
#ifdef HAVE_WINSOCK2_H
#include <winsock2.h> /* needed for htonl, htons, ... */
#endif

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h> // gettimeofday
#endif

HeteroPlayer::HeteroPlayer()
{
    const int MAX_TRIAL = 1000;

    const ServerParam & SP = ServerParam::instance();
    const PlayerParam & PP = PlayerParam::instance();

    int trial = 0;
    while ( ++trial <= MAX_TRIAL )
    {
        // trade-off player_speed_max with stamina_inc_max (actually unused)
        double tmp_delta = HeteroPlayer::delta( PP.playerSpeedMaxDeltaMin(),
                                                PP.playerSpeedMaxDeltaMax() );
        M_player_speed_max = SP.playerSpeedMax() + tmp_delta;
        if ( M_player_speed_max <= 0.0 ) continue;
        M_stamina_inc_max = SP.staminaInc() + tmp_delta * PP.staminaIncMaxDeltaFactor();
        if ( M_stamina_inc_max <= 0.0 ) continue;

        // trade-off player_decay with inertia_moment
        tmp_delta = HeteroPlayer::delta( PP.playerDecayDeltaMin(),
                                         PP.playerDecayDeltaMax() );
        M_player_decay = SP.playerDecay() + tmp_delta;
        if ( M_player_decay <= 0.0 ) continue;
        M_inertia_moment = SP.inertiaMoment() + tmp_delta * PP.inertiaMomentDeltaFactor();
        if ( M_inertia_moment < 0.0 ) continue;

        // trade-off dash_power_rate with player_size (actually unused)
        tmp_delta = HeteroPlayer::delta( PP.dashPowerRateDeltaMin(),
                                         PP.dashPowerRateDeltaMax() );
        M_dash_power_rate = SP.dashPowerRate() + tmp_delta;
        if ( M_dash_power_rate <= 0.0 ) continue;
        M_player_size = SP.playerSize() + tmp_delta * PP.playerSizeDeltaFactor();
        if ( M_player_size <= 0.0 ) continue;

        // trade-off stamina_inc_max with dash_power_rate
        tmp_delta = HeteroPlayer::delta( PP.newDashPowerRateDeltaMin(),
                                         PP.newDashPowerRateDeltaMax() );
        M_dash_power_rate = SP.dashPowerRate() + tmp_delta;
        if ( M_dash_power_rate <= 0.0 ) continue;
        M_stamina_inc_max = SP.staminaInc() + tmp_delta * PP.newStaminaIncMaxDeltaFactor();
        if ( M_stamina_inc_max <= 0.0 ) continue;

        // trade-off kickable_margin with kick_rand
        tmp_delta = HeteroPlayer::delta( PP.kickableMarginDeltaMin(),
                                         PP.kickableMarginDeltaMax() );
        M_kickable_margin = SP.kickableMargin() + tmp_delta;
        if ( M_kickable_margin <= 0.0 ) continue;
        M_kick_rand = SP.kickRand() + tmp_delta * PP.kickRandDeltaFactor();
        if ( M_kick_rand < 0.0 ) continue;

        // trade-off extra_stamina with effort_{min,max}
        tmp_delta = HeteroPlayer::delta( PP.extraStaminaDeltaMin(),
                                         PP.extraStaminaDeltaMax() );
        M_extra_stamina = SP.extraStamina() + tmp_delta;
        if ( M_extra_stamina < 0.0 ) continue;
        M_effort_max = SP.effortInit() + tmp_delta * PP.effortMaxDeltaFactor();
        M_effort_min = SP.effortMin()  + tmp_delta * PP.effortMinDeltaFactor();
        if ( M_effort_max <= 0.0 ) continue;
        if ( M_effort_min <= 0.0 ) continue;

        // v14
        // trade-off kick_power_rate with foul_detect_probability
        tmp_delta = HeteroPlayer::delta( PP.kickPowerRateDeltaMin(),
                                         PP.kickPowerRateDeltaMax() );
        M_kick_power_rate = SP.kickPowerRate() + tmp_delta;
        M_foul_detect_probability = SP.foulDetectProbability()
            + tmp_delta * PP.foulDetectProbabilityDeltaFactor();

        // trade-off catchable_area_l with catch probability
        tmp_delta = HeteroPlayer::delta( PP.catchAreaLengthStretchMin(),
                                         PP.catchAreaLengthStretchMax() );
        M_catchable_area_l_stretch = tmp_delta;

        //
        double real_speed_max
            = ( SP.maxPower() * M_dash_power_rate * M_effort_max )
            / ( 1.0 - M_player_decay );
        if ( SP.playerSpeedMaxMin() - EPS < real_speed_max
             && real_speed_max < M_player_speed_max + EPS )
        {
            break;
        }
    }

    if ( SP.verboseMode() )
    {
        std::cout << "HeteroPlayer creation trial = " << trial << std::endl;
    }

    // TODO: print error message and exit the simulator.
    if ( trial > MAX_TRIAL )
    {
        std::cout << "HeteroPlayer set default parameters." << std::endl;
        setDefault();
    }
}

HeteroPlayer::HeteroPlayer( int )
{
    setDefault();
}

HeteroPlayer::~HeteroPlayer()
{

}

double
HeteroPlayer::delta( const double & min,
                     const double & max )
{
    static bool s_seeded = false;
    static boost::mt19937 s_engine;

    if ( ! s_seeded )
    {
        if ( PlayerParam::instance().randomSeed() >= 0 )
        {
            std::cout << "Using given Hetero Player Seed: "
                      << PlayerParam::instance().randomSeed() << std::endl;
            s_engine.seed( PlayerParam::instance().randomSeed() );
        }
        else if ( ServerParam::instance().randomSeed() >= 0 )
        {
            std::cout << "Using simulator's random seed as Hetero Player Seed: "
                      << ServerParam::instance().randomSeed() << std::endl;
            PlayerParam::instance().setRandomSeed( ServerParam::instance().randomSeed() );
            s_engine.seed( PlayerParam::instance().randomSeed() );
        }
        else
        {
            timeval now;
            gettimeofday ( &now, NULL );

            int seed = static_cast< int >( now.tv_usec );
            PlayerParam::instance().setRandomSeed( seed );
            std::cout << "Hetero Player Seed: " << seed << std::endl;
            s_engine.seed( PlayerParam::instance().randomSeed() );
        }
        s_seeded = true;
    }

    if ( min == max )
    {
        return min;
    }

    double minv = min;
    double maxv = max;

    if ( minv > maxv )
    {
        std::swap( minv, maxv );
    }

    boost::uniform_real< double > rng( minv, maxv );
    boost::variate_generator< boost::mt19937&, boost::uniform_real<> > gen( s_engine, rng );
    return gen();
}

void
HeteroPlayer::setDefault()
{
    const ServerParam & SP = ServerParam::instance();

    M_player_speed_max = SP.playerSpeedMax();
    M_stamina_inc_max = SP.staminaInc();

    M_player_decay = SP.playerDecay();
    M_inertia_moment = SP.inertiaMoment();

    M_dash_power_rate = SP.dashPowerRate();
    M_player_size = SP.playerSize();

    M_kickable_margin = SP.kickableMargin();
    M_kick_rand = SP.kickRand();

    M_extra_stamina = SP.extraStamina();
    M_effort_max = SP.effortInit();
    M_effort_min = SP.effortMin();

    // v14
    M_kick_power_rate = SP.kickPowerRate();
    M_foul_detect_probability = SP.foulDetectProbability();
    M_catchable_area_l_stretch = 1.0;
}

std::ostream &
HeteroPlayer::print( std::ostream & o ) const
{
    o << "Hetero Player Type:\n";
    o << "\tPlayer speed max = " << playerSpeedMax() << '\n';
    o << "\tStamina inc = " << staminaIncMax() <<  '\n';
    o << "\tPlayer decay = " << playerDecay() << '\n';
    o << "\tInertia moment = " << inertiaMoment() << '\n';
    o << "\tDash power rate = " << dashPowerRate() << '\n';
    o << "\tPlayer size = " << playerSize() << '\n';
    o << "\tKickable margin = " << kickableMargin() << '\n';;
    o << "\tKick rand = " << kickRand() << '\n';
    o << "\tKick power rate = " << kickPowerRate() << '\n';
    o << "\tFoul detect probability = " << foulDetectProbability() << '\n';
    o << "\tExtra stamina = " << extraStamina() << '\n';
    o << "\tEffort max = " << effortMax() << '\n';
    o << "\tEffort min = " << effortMin() << '\n';
    double power_cons_at_max_vel
        = ( ( playerSpeedMax() * ( 1 - playerDecay() ) )
            / dashPowerRate() )
        - staminaIncMax();
    o << "\tPower Cons @ max vel = " << power_cons_at_max_vel << std::endl;
    return o;
}

player_type_t
HeteroPlayer::convertToStruct( int id ) const
{
    player_type_t tmp;

    tmp.id = htons( id );
    tmp.player_speed_max = htonl( static_cast< Int32 >( rint( ( playerSpeedMax() * SHOWINFO_SCALE2 ) ) ) );
    tmp.stamina_inc_max = htonl( static_cast< Int32 >( rint( ( staminaIncMax() * SHOWINFO_SCALE2 ) ) ) );
    tmp.player_decay = htonl( static_cast< Int32 >( rint( ( playerDecay() * SHOWINFO_SCALE2 ) ) ) );
    tmp.inertia_moment = htonl( static_cast< Int32 >( rint( ( inertiaMoment() * SHOWINFO_SCALE2 ) ) ) );
    tmp.dash_power_rate = htonl( static_cast< Int32 >( rint( ( dashPowerRate() * SHOWINFO_SCALE2 ) ) ) );
    tmp.player_size = htonl( static_cast< Int32 >( rint( ( playerSize() * SHOWINFO_SCALE2 ) ) ) );
    tmp.kickable_margin = htonl( static_cast< Int32 >( rint( ( kickableMargin() * SHOWINFO_SCALE2 ) ) ) );
    tmp.kick_rand = htonl( static_cast< Int32 >( rint( ( kickRand() * SHOWINFO_SCALE2 ) ) ) );
    tmp.extra_stamina = htonl( static_cast< Int32 >( rint( ( extraStamina() * SHOWINFO_SCALE2 ) ) ) );
    tmp.effort_max = htonl( static_cast< Int32 >( rint( ( effortMax() * SHOWINFO_SCALE2 ) ) ) );
    tmp.effort_min = htonl( static_cast< Int32 >( rint( ( effortMin() * SHOWINFO_SCALE2 ) ) ) );

    tmp.kick_power_rate = htonl( static_cast< Int32 >( rint( ( kickPowerRate() * SHOWINFO_SCALE2 ) ) ) );
    tmp.foul_detect_probability = htonl( static_cast< Int32 >( rint( ( foulDetectProbability() * SHOWINFO_SCALE2 ) ) ) );
    tmp.catchable_area_l_stretch = htonl( static_cast< Int32 >( rint( ( catchAreaLengthStretch() * SHOWINFO_SCALE2 ) ) ) );

    return tmp;
}

namespace {

template < typename T >
void
serialize_param( std::ostream & o,
                 const std::string & name,
                 const T & value )
{
    o << '(' << name << ' ' << value << ')';
}

}

void
HeteroPlayer::serializeParams( std::ostream & o,
                               const unsigned int version,
                               const int id ) const
{
    serialize_param( o, "id", id );
    serialize_param( o, "player_speed_max", playerSpeedMax() );
    serialize_param( o, "stamina_inc_max",  staminaIncMax() );
    serialize_param( o, "player_decay",     playerDecay() );
    serialize_param( o, "inertia_moment",   inertiaMoment() );
    serialize_param( o, "dash_power_rate",  dashPowerRate() );
    serialize_param( o, "player_size",      playerSize() );
    serialize_param( o, "kickable_margin",  kickableMargin() );
    serialize_param( o, "kick_rand",        kickRand() );
    serialize_param( o, "extra_stamina",    extraStamina() );
    serialize_param( o, "effort_max",       effortMax() );
    serialize_param( o, "effort_min",       effortMin() );

    if ( version < 14 )
    {
        return;
    }

    serialize_param( o, "kick_power_rate", kickPowerRate() );
    serialize_param( o, "foul_detect_probability", foulDetectProbability() );
    serialize_param( o, "catchable_area_l_stretch", catchAreaLengthStretch() );
}
