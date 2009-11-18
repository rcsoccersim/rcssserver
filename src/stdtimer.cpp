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

#include <csignal>          // needed for sigaction
#include <cstdlib>          // needed for NULL

#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>       // needed for itimerval
#endif

#ifdef __CYGWIN__
// cygwin is not win32
#elif defined(_WIN32) || defined(__WIN32__) || defined (WIN32)
#define RCSS_WIN
#endif


bool StandardTimer::gotsig = false;
int StandardTimer::timedelta = 0;
bool StandardTimer::lock_timedelta = false;

StandardTimer::StandardTimer( Timeable & timeable )
    : Timer( timeable )
{
    gotsig         = false;
    timedelta      = 0;
    lock_timedelta = false;
}


#ifdef RCSS_WIN
VOID
CALLBACK
StandardTimer::check( PVOID ptr, BOOL )
{
    static int td_mult = 1;
    if ( lock_timedelta )
    {
        td_mult += 1;
    }
    else
    {
        timedelta += td_mult * TIMEDELTA;
        td_mult = 1;
    }
    gotsig = true;
    SetEvent((HANDLE)ptr);
}
#else
void
StandardTimer::check( void )
{
    static int td_mult = 1;
    if ( lock_timedelta )
    {
        td_mult += 1;
    }
    else
    {
        timedelta += td_mult * TIMEDELTA;
        td_mult = 1;
    }
    gotsig = true;
}
#endif

#ifdef RCSS_WIN
static
void
initTimer( HANDLE gDoneEvent )
{
    HANDLE hTimer = NULL;
    HANDLE hTimerQueue = NULL;
    int arg = 123;

    // Use an event object to track the TimerRoutine execution
    gDoneEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
    if ( ! gDoneEvent )
    {
        printf( "CreateEvent failed (%d)\n", GetLastError() );
        //         return 1;
    }

    // Create the timer queue.
    hTimerQueue = CreateTimerQueue();
    if ( ! hTimerQueue )
    {
        printf( "CreateTimerQueue failed (%d)\n", GetLastError() );
        //         return 2;
    }

    // Set a timer to call the timer routine in 10 seconds.
    if ( ! CreateTimerQueueTimer( &hTimer, hTimerQueue,
                                  (WAITORTIMERCALLBACK)&StandardTimer::check,
                                  &gDoneEvent , TIMEDELTA, TIMEDELTA, 0 ) )
    {
        printf( "CreateTimerQueueTimer failed (%d)\n", GetLastError() );
        //         return 3;
    }

    // TODO: Do other useful work here

    printf( "Call timer routine in 10 seconds...\n" );

    //     // Wait for the timer-queue thread to complete using an event
    //     // object. The thread will signal the event at that time.

    //     if (WaitForSingleObject(gDoneEvent, INFINITE) != WAIT_OBJECT_0)
    //         printf("WaitForSingleObject failed (%d)\n", GetLastError());

    //     // Delete all timers in the timer queue.
    //     if (!DeleteTimerQueue(hTimerQueue))
    //         printf("DeleteTimerQueue failed (%d)\n", GetLastError());
}
#else
static
void
initTimer( struct itimerval & itv_prev,
           struct sigaction & alarm_prev )
{
    struct itimerval itv;
    struct sigaction alarm_action;
    itv.it_interval.tv_sec = 0 ;
    itv.it_interval.tv_usec = TIMEDELTA * 1000 ;
    itv.it_value.tv_sec = 0 ;
    itv.it_value.tv_usec = TIMEDELTA * 1000 ;

    alarm_action.sa_handler = (void (*)(int))StandardTimer::check ;
    alarm_action.sa_flags = 0; // [2000/11/20.frehberg.cs.tu-berlin.de]

    sigaction( SIGALRM, &alarm_action, &alarm_prev );
    setitimer( ITIMER_REAL, &itv, &itv_prev );
}
#endif

#ifdef RCSS_WIN
#else
static
void
restoreTimer( struct itimerval & itv_prev,
              struct sigaction & alarm_prev )
{
    setitimer( ITIMER_REAL, &itv_prev, NULL );   // restore the old timer
    sigaction( SIGALRM, &alarm_prev, NULL );     // restore the old alaram handler
}
#endif

/** This method controls the standard timer. After the arrival of a signal it
    checks which messages should be sent / received by the clients and handles
    them appropriately. */
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

    // create a timer that will be called every TIMEDELTA msec. Each time
    // this timer is called, lcmt is raised and it is checked which message
    // should be processed according to the part of the sequence we are in

#ifdef RCSS_WIN
    HANDLE gDoneEvent = NULL;
    initTimer(gDoneEvent);
#else
    struct itimerval itv_prev;
    struct sigaction alarm_prev;
    initTimer( itv_prev, alarm_prev );
#endif
    //for (;;)
    while ( getTimeableRef().alive() )
    {
#ifdef RCSS_WIN
        if ( WaitForSingleObject(gDoneEvent, INFINITE) != WAIT_OBJECT_0 )
            printf( "WaitForSingleObject failed (%d)\n", GetLastError() );
#else
        if ( ! gotsig )
            sigpause( SIGUSR1 );
#endif
        gotsig = false;

        lock_timedelta = true;
        lcmt += timedelta;
        timedelta = 0;
        lock_timedelta = false;

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
#ifdef RCSS_WIN
#else
    restoreTimer( itv_prev, alarm_prev );
#endif
    getTimeableRef().quit();
}
