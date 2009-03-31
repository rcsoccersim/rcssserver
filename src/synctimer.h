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

#ifndef SYNCTIMER_H
#define SYNCTIMER_H

#include "timer.h"

/** This is a subclass of the timer class. The run method specifes the
    synchronization timer. This timer waits till all messages are received by
    the client before continuing. */
class SyncTimer
    : public Timer
{
public:
    SyncTimer( Timeable &timeable )
        : Timer( timeable )
    {}

    void
    run();

//     static
//     void
//     destroy( SyncTimer* c );

//     static
//     Ptr
//     create( Timeable& t );
};

#endif
