// -*-c++-*-

/***************************************************************************
                                   timer.h
              The timer class is an abstract base class for timing
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
 *   Foundation; either version 2 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#ifndef TIMER_H
#define TIMER_H

#include "rcssserverconfig.hpp"

#include "timeable.h"
#include <rcssbase/lib/shared_ptr.hpp>

/** The timer class is an abstract class that has a reference to a timeable
    class. Specific timers can be derived from this class which has to implement
    the 'run' method. In this 'run' method the timeable methods from the
    timeable instance are called. */
class Timer
{
public:
    typedef rcss::lib::shared_ptr< Timer > Ptr;
    typedef Ptr(*Creator)( Timeable& );
    typedef rcss::lib::Factory< Creator > Factory;

    virtual
    ~Timer()
      { }

	RCSSSERVER_API
    static
    Factory&
    factory();

    virtual
    void
    run() = 0;

protected:
	RCSSSERVER_API
    Timer( Timeable &timeable )
        : M_timeable ( timeable )
    {}

	RCSSSERVER_API
    Timeable&
    getTimeableRef()
    { return M_timeable; }

private:
    Timeable& M_timeable;            /**< reference to the timeable object */
};

#endif
