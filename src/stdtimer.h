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

#include "timer.h"

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

    StandardTimer( const StandardTimer& t ) = delete;
public:

    explicit
    StandardTimer( Timeable &timeable );

    void run() override;

};

#endif
