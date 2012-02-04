// -*-c++-*-

/***************************************************************************
                                   synctimer.h
                   The synchronous timer used by the simulator
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

#include "synctimer.h"

#include "timeable.h"
#include "serverparam.h"    // needed for ServerParam

void
SyncTimer::run()
{
    // for explanation different counters, see StandardTimer::run
    // big difference is that the loop is not started when a new signal comes,
    // but rather immediately when the previous loop has ended. Each loop
    // represents synch_offset msec (default 60 msec). So after the loop has been
    // traversed 6 times and the appropriate message are sent, the think command
    // is sent and the program waits for all the client instructions and the final
    // "done". When a done message is received for each client, the loop continues

    double lcmt= 0;
    int c_simt = 1,
        c_sent = 1,
        c_sbt  = 1,
        c_svt  = 1,
        c_synch= 1,
        c_synch_see = 1,
        q_simt = ServerParam::instance().lcmStep()/ServerParam::instance().simStep(),
        q_sent = ServerParam::instance().lcmStep()/ServerParam::instance().sendStep()*4,
        q_sbt  = ServerParam::instance().lcmStep()/ServerParam::instance().senseBodyStep(),
        q_svt  = ServerParam::instance().lcmStep()/ServerParam::instance().coachVisualStep();
    bool sent_synch_see = false;

    while ( getTimeableRef().alive() )
    {
        lcmt += TIMEDELTA;

        // new simulation step
        if ( lcmt >= ServerParam::instance().simStep() * c_simt )
        {
            getTimeableRef().newSimulatorStep();
            if ( q_simt <= c_simt )
            {
                c_simt = 1;
            }
            else
            {
                c_simt++;
            }
            sent_synch_see = false;
        }

        // new sense body step
        if ( lcmt >= ServerParam::instance().senseBodyStep() * c_sbt )
        {
            getTimeableRef().sendSenseBody();
            c_sbt = static_cast< int >( std::floor( lcmt / ServerParam::instance().senseBodyStep() ) );
            if ( q_sbt <= c_sbt )
            {
                c_sbt = 1;
            }
            else
            {
                c_sbt++;
            }
        }

        // send visuals
        if ( lcmt >= (ServerParam::instance().sendStep() * 0.25 ) * c_sent )
        {
            getTimeableRef().sendVisuals();
            c_sent = static_cast< int >( std::floor( lcmt / (ServerParam::instance().sendStep() * 0.25 ) ) );
            if ( q_sent <= c_sent )
            {
                c_sent = 1;
            }
            else
            {
                c_sent++;
            }
        }

        // send synch visual message
        if ( ! sent_synch_see
             && lcmt >= ( ServerParam::instance().simStep() * ( c_synch_see - 1 )
                          + ServerParam::instance().synchSeeOffset() ) )
        {
            getTimeableRef().sendSynchVisuals();
            ++c_synch_see;
            sent_synch_see = true;
        }

        // send coach messages
        if ( lcmt >= ServerParam::instance().coachVisualStep() * c_svt )
        {
            getTimeableRef().sendCoachMessages();
            c_svt = static_cast< int >( std::floor( lcmt / ServerParam::instance().coachVisualStep() ) );
            if ( q_svt <= c_svt )
            {
                c_svt = 1;
            }
            else
            {
                c_svt++;
            }
        }

        //we do a c_synch-1 because of the offset
        if ( lcmt >= ServerParam::instance().simStep() * ( c_synch - 1 ) + ServerParam::instance().synchOffset() )
        {
            getTimeableRef().sendThink();
            /* because of the strange offset nature here,
               we let the lcmt advancement below handle the resetting to 1 */
            c_synch++;
        }

        if ( lcmt >= ServerParam::instance().lcmStep() )
        {
            lcmt = 0;
            c_synch = 1;
            c_synch_see = 1;
        }
    }

    getTimeableRef().quit();
}
