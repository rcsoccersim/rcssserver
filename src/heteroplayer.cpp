/* -*- Mode: C++ -*- */

/*
 *Copyright:

 Copyright (C) 1996-2000 Electrotechnical Laboratory.
 Itsuki Noda, Yasuo Kuniyoshi and Hitoshi Matsubara.
 Copyright (C) 2000, 2001 RoboCup Soccer Server Maintainance Group.
 Patrick Riley, Tom Howard, Daniel Polani, Itsuki Noda,
 Mikhail Prokopenko, Jan Wendler

 This file is a part of SoccerServer.

 This code is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

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
#include "config.h"
#endif

#include "heteroplayer.h"

#include <boost/random.hpp>

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h> /* needed for htonl, htons, ... */
#endif
#ifdef HAVE_WINSOCK2_H
#include <Winsock2.h> /* needed for htonl, htons, ... */
#endif

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

double
HetroPlayer::delta( const double & min,
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
            //srandom( PlayerParam::instance().randomSeed() );
            s_engine.seed( PlayerParam::instance().randomSeed() );
        }
        else
        {
            timeval now;
            gettimeofday ( &now, NULL );
            std::cout << "Hetero Player Seed: " << now.tv_usec << std::endl;
            //srandom( now.tv_usec );
            s_engine.seed( static_cast< boost::mt19937::result_type >( now.tv_usec ) );
        }
        s_seeded = true;
    }

    //return drand( min, max );
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

HetroPlayer::HetroPlayer()
{
    const int MAX_TRIAL = 1000;

    int trial = 0;
    while ( ++trial <= MAX_TRIAL )
    {
        double tmp_delta = HetroPlayer::delta( PlayerParam::instance().playerSpeedMaxDeltaMin(),
                                               PlayerParam::instance().playerSpeedMaxDeltaMax() );
        player_speed_max = ServerParam::instance().playerSpeedMax() + tmp_delta;
        stamina_inc_max = ServerParam::instance().staminaInc()
            + tmp_delta * PlayerParam::instance().staminaIncMaxDeltaFactor();

        tmp_delta = HetroPlayer::delta( PlayerParam::instance().playerDecayDeltaMin(),
                                        PlayerParam::instance().playerDecayDeltaMax() );
        player_decay = ServerParam::instance().playerDecay() + tmp_delta;
        inertia_moment = ServerParam::instance().inertiaMoment()
            + tmp_delta * PlayerParam::instance().inertiaMomentDeltaFactor();

        tmp_delta = HetroPlayer::delta( PlayerParam::instance().dashPowerRateDeltaMin(),
                                        PlayerParam::instance().dashPowerRateDeltaMax() );
        dash_power_rate = ServerParam::instance().dashPowerRate() + tmp_delta;
        player_size = ServerParam::instance().playerSize()
            + tmp_delta * PlayerParam::instance().playerSizeDeltaFactor();

        // trade-off stamina_inc_max with dash_power_rate
        tmp_delta = HetroPlayer::delta( PlayerParam::instance().newDashPowerRateDeltaMin(),
                                        PlayerParam::instance().newDashPowerRateDeltaMax() );
        dash_power_rate = ServerParam::instance().dashPowerRate() + tmp_delta;
        stamina_inc_max = ServerParam::instance().staminaInc()
            + tmp_delta * PlayerParam::instance().newStaminaIncMaxDeltaFactor();

        tmp_delta = HetroPlayer::delta( PlayerParam::instance().kickableMarginDeltaMin(),
                                        PlayerParam::instance().kickableMarginDeltaMax() );
        kickable_margin = ServerParam::instance().kickableMargin() + tmp_delta;
        kick_rand = ServerParam::instance().kickRand()
            + tmp_delta * PlayerParam::instance().kickRandDeltaFactor();

        tmp_delta = HetroPlayer::delta( PlayerParam::instance().extraStaminaDeltaMin(),
                                        PlayerParam::instance().extraStaminaDeltaMax() );
        extra_stamina = ServerParam::instance().extraStamina() + tmp_delta;
        effort_max = ServerParam::instance().effortInit()
            + tmp_delta * PlayerParam::instance().effortMaxDeltaFactor();
        effort_min = ServerParam::instance().effortMin()
            + tmp_delta * PlayerParam::instance().effortMinDeltaFactor();


        double real_speed_max
            = ( ServerParam::instance().maxPower()
                * dash_power_rate
                * effort_max )
            / ( 1.0 - player_decay );

        if ( player_speed_max - ServerParam::instance().playerSpeedMaxRange() - EPS
             < real_speed_max
             && real_speed_max < player_speed_max + EPS )
        {
            break;
        }
    }

    if ( ServerParam::instance().verboseMode() )
    {
        std::cout << "HetroPlayer creation trial = " << trial << std::endl;
    }

    // TODO: print error message and exit the simulator.
    if ( trial > MAX_TRIAL )
    {
        std::cout << "HetroPlayer set default parameters." << std::endl;
        // set the default type parameters
        player_speed_max = ServerParam::instance().playerSpeedMax();
        stamina_inc_max = ServerParam::instance().staminaInc();

        player_decay = ServerParam::instance().playerDecay();
        inertia_moment = ServerParam::instance().inertiaMoment();

        dash_power_rate = ServerParam::instance().dashPowerRate();
        player_size = ServerParam::instance().playerSize();

        kickable_margin = ServerParam::instance().kickableMargin();
        kick_rand = ServerParam::instance().kickRand();

        extra_stamina = ServerParam::instance().extraStamina();
        effort_max = ServerParam::instance().effortInit();
        effort_min = ServerParam::instance().effortMin();
    }
}

HetroPlayer::HetroPlayer( int )
{
    player_speed_max = ServerParam::instance().playerSpeedMax();
    stamina_inc_max = ServerParam::instance().staminaInc();

    player_decay = ServerParam::instance().playerDecay();
    inertia_moment = ServerParam::instance().inertiaMoment();

    dash_power_rate = ServerParam::instance().dashPowerRate();
    player_size = ServerParam::instance().playerSize();

    kickable_margin = ServerParam::instance().kickableMargin();
    kick_rand = ServerParam::instance().kickRand();

    extra_stamina = ServerParam::instance().extraStamina();
    effort_max = ServerParam::instance().effortInit();
    effort_min = ServerParam::instance().effortMin();
}

HetroPlayer::~HetroPlayer()
{
}

std::ostream&
HetroPlayer::print( std::ostream & o ) const
{
    o << "Hetro Player Type:\n";
    o << "\tPlayer speed max = " << playerSpeedMax () << '\n';
    o << "\tStamina inc = " << staminaIncMax () <<  '\n';
    o << "\tPlayer decay = " << playerDecay () << '\n';
    o << "\tInertia moment = " << inertiaMoment () << '\n';
    o << "\tDash power rate = " << dashPowerRate () << '\n';
    o << "\tPlayer size = " << playerSize () << '\n';
    o << "\tKickable margin = " << kickableMargin () << '\n';;
    o << "\tKick rand = " << kickRand () << '\n';
    o << "\tExtra stamina = " << extraStamina () << '\n';
    o << "\tEffort max = " << effortMax () << '\n';
    o << "\tEffort min = " << effortMin () << '\n';
    double power_cons_at_max_vel
        = ( ( playerSpeedMax() * ( 1 - playerDecay() ) )
            / dashPowerRate() )
        - staminaIncMax();
    o << "\tPower Cons @ max vel = " << power_cons_at_max_vel << std::endl;
    return o;
}

player_type_t
HetroPlayer::convertToStruct( int id )
{
    player_type_t tmp;

    tmp.id = htons ( id );
    tmp.player_speed_max = htonl ( static_cast< Int32 >( rint( ( player_speed_max * SHOWINFO_SCALE2 ) ) ) );
    tmp.stamina_inc_max = htonl ( static_cast< Int32 >( rint( ( stamina_inc_max * SHOWINFO_SCALE2 ) ) ) );
    tmp.player_decay = htonl ( static_cast< Int32 >( rint( ( player_decay * SHOWINFO_SCALE2 ) ) ) );
    tmp.inertia_moment = htonl ( static_cast< Int32 >( rint( ( inertia_moment * SHOWINFO_SCALE2 ) ) ) );
    tmp.dash_power_rate = htonl ( static_cast< Int32 >( rint( ( dash_power_rate * SHOWINFO_SCALE2 ) ) ) );
    tmp.player_size = htonl ( static_cast< Int32 >( rint( ( player_size * SHOWINFO_SCALE2 ) ) ) );
    tmp.kickable_margin = htonl ( static_cast< Int32 >( rint( ( kickable_margin * SHOWINFO_SCALE2 ) ) ) );
    tmp.kick_rand = htonl ( static_cast< Int32 >( rint( ( kick_rand * SHOWINFO_SCALE2 ) ) ) );
    tmp.extra_stamina = htonl ( static_cast< Int32 >( rint( ( extra_stamina * SHOWINFO_SCALE2 ) ) ) );
    tmp.effort_max = htonl ( static_cast< Int32 >( rint( ( effort_max * SHOWINFO_SCALE2 ) ) ) );
    tmp.effort_min = htonl ( static_cast< Int32 >( rint( ( effort_min * SHOWINFO_SCALE2 ) ) ) );

    return tmp;
}
