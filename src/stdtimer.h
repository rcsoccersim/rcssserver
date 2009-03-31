// -*-c++-*-

/***************************************************************************
                                   stdtimer.h
                    The stadard timer used by the simualtor
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

#ifndef STDTIMER_H
#define STDTIMER_H

#ifdef __CYGWIN__
// cygwin is not win32
#elif defined(_WIN32) || defined(__WIN32__) || defined (WIN32)
#define _WIN32_WINNT 0x0500
#include <winsock2.h>
#include <windows.h>
#endif

#include "timer.h"
#include "rcssexceptions.h"

/** This is a subclass of the timer class. The run method specifes the
    standard timer. This timer is controlled by the different server
    configuration parameters that specify when the different messages
    will arrive. It is a singleton class, which means it can only be
    initialized once. To initialize the class, use the static instance
    method with as argument the timeable object. To use this class at
    a later time use the instance method with no argument. <BR>
    StandardTimer::instance( Std );<BR>
    StnadardTimer::instance().run(). */
class StandardTimer
    : public Timer {
private:

    static bool gotsig;            // variables needed to keep track
    static int  timedelta;         // of amount of arrived signals
    static bool lock_timedelta;

    StandardTimer( const StandardTimer& t );
public:

    StandardTimer( Timeable &timeable );

    void
    run();

#ifdef __CYGWIN__
// cygwin is not win32
    static
    void check();
#elif defined(_WIN32) || defined(__WIN32__) || defined (WIN32)
    static
    VOID
    CALLBACK
    check( PVOID lpParam, BOOL TimerOrWaitFired );
#else
    static
    void check();
#endif

};

#endif
