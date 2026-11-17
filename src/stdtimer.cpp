// -*-c++-*-

/***************************************************************************
                                   stdtimer.cc
                   The standard timer used by the simulator
                             -------------------
    begin                : AUG-2002
    copyright            : (C) 2002 by The RoboCup Soccer Server
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

#include "stdtimer.h"

#include "timeable.h"
#include "param.h"          // needed for TIMEDELTA
#include "serverparam.h"    // needed for ServerParam

#include <chrono>

StandardTimer::StandardTimer( Timeable & timeable )
    : Timer( timeable )
{

}


/** This method controls the standard timer.
    In the mainloop, the timer waits for the next tick, which comes every
    default delta on a fixed schedule, so that the cycles keep up with the
    real time. The I/O of the clients goes on while waiting.
    After the wait, checking which messages should be sent / received by the clients and handles them appropriately. */
void
StandardTimer::run()
{
    double lcmt = 0.0;

    // the sequence of messages will always repeat itself after lcm_st ms.
    // the number of messages that should be processed before the sequence
    // "quits" thus equals (lcm_st / x) where x is the time interval for the
    // specific message
    // Example: lcm_st = 300, sim_st = 100, q_simt = 300 / 100 = 3.
    //          three simulations steps have to occur in one sequence
    // in the default case the listed variables will have the following values
    // q_simt = 3  (simulation step)
    // q_sent = 8  (sending visual information - low quality, narrow width)
    // q_rect = 30 (process arrival of messages)
    // q_sbt  = 3  (sense body step)
    // q_svt  = 3  (coach look interval)
    // c_* will contain the number of processed messages in this sequence
    int c_simt = 1, c_sent = 1, c_rect = 1, c_sbt = 1, c_svt = 1,
        q_simt = ServerParam::instance().lcmStep() / ServerParam::instance().simStep(),
        q_sent = ServerParam::instance().lcmStep() / ServerParam::instance().sendStep()*4,
        q_rect = ServerParam::instance().lcmStep() / ServerParam::instance().recvStep(),
        q_sbt = ServerParam::instance().lcmStep() / ServerParam::instance().senseBodyStep(),
        q_svt = ServerParam::instance().lcmStep() / ServerParam::instance().coachVisualStep();
    int c_synch_see = 1;
    bool sent_synch_see = false;

    const std::chrono::steady_clock::duration default_delta = std::chrono::milliseconds( TIMEDELTA );
    std::chrono::steady_clock::time_point next_tick = std::chrono::steady_clock::now() + default_delta;

    while ( getTimeableRef().alive() )
    {
        getTimeableRef().waitUntil( next_tick );
        if ( ! getTimeableRef().alive() )
        {
            break;
        }

        // If the previous tick took longer than the default delta, the ticks
        // that have already passed are skipped, not caught up with. lcmt
        // still counts the time that has passed.
        const std::chrono::steady_clock::duration late = std::chrono::steady_clock::now() - next_tick;
        const int ticks = 1 + static_cast< int >( late / default_delta );
        next_tick += ticks * default_delta;
        lcmt += TIMEDELTA * ticks;

        if ( lcmt >= ServerParam::instance().simStep() * c_simt )
        {
            c_simt = static_cast< int >( std::floor( lcmt / ServerParam::instance().simStep() ) );
            lcmt = ServerParam::instance().simStep() * c_simt;
            // the above lines are needed to
            // prevent short "catch up" cycles if
            // the current cycle was slow
        }

        // receive messages
        if ( lcmt >= ServerParam::instance().recvStep() * c_rect )
        {
            getTimeableRef().recvFromClients();
            c_rect = static_cast< int >( std::floor( lcmt / ServerParam::instance().recvStep() ) );
            if ( q_rect <= c_rect )
                c_rect = 1;
            else
                c_rect++;
        }

        // update after new simulation step
        if ( lcmt >= ServerParam::instance().simStep() * c_simt )
        {
            getTimeableRef().newSimulatorStep();
            if ( q_simt <= c_simt )
                c_simt = 1;
            else
                c_simt++;
            sent_synch_see = false;
        }

        // send sense body
        if ( lcmt >= ServerParam::instance().senseBodyStep() * c_sbt )
        {
            getTimeableRef().sendSenseBody();
            c_sbt = static_cast< int >( std::floor( lcmt / ServerParam::instance().senseBodyStep() ) );
            if ( q_sbt <= c_sbt )
                c_sbt = 1;
            else
                c_sbt++;
        }

        // send visual messages
        if ( lcmt >= ( ServerParam::instance().sendStep() * 0.25 ) * c_sent )
        {
            getTimeableRef().sendVisuals();
            c_sent = static_cast< int >( std::floor( lcmt/(ServerParam::instance().sendStep() * 0.25 ) ) );
            if ( q_sent <= c_sent )
                c_sent = 1;
            else
                c_sent++;
        }

        // send synch visual message
        if ( ! sent_synch_see
             && lcmt >= ( ServerParam::instance().simStep() * ( c_synch_see - 1 )
                          + ServerParam::instance().synchSeeOffset() ) )
        {
            //std::cerr << "lcmt=" << lcmt << "  c_synch_see=" << c_synch_see << '\n';
            getTimeableRef().sendSynchVisuals();
            ++c_synch_see;
            sent_synch_see = true;
        }

        // send coach look messages
        if ( lcmt >= ServerParam::instance().coachVisualStep() * c_svt )
        {
            getTimeableRef().sendCoachMessages();
            c_svt = static_cast< int >( std::floor( lcmt / ServerParam::instance().coachVisualStep() ) );
            if ( q_svt <= c_svt )
                c_svt = 1;
            else
                c_svt++;
        }

        if ( lcmt >= ServerParam::instance().lcmStep() )
        {
            lcmt = 0;
            c_synch_see = 1;
        }
    }

    getTimeableRef().quit();
}
