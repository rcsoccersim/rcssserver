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
 *   Foundation; either version 3 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#ifndef RCSSSERVER_TIMER_H
#define RCSSSERVER_TIMER_H

class Timeable;

/** The timer class is an abstract class that has a reference to a timeable
    class. Specific timers can be derived from this class which has to implement
    the 'run' method. In this 'run' method the timeable methods from the
    timeable instance are called. */
class Timer {
public:

    virtual
    ~Timer()
      { }

    virtual
    void run() = 0;

protected:

    Timer( Timeable & timeable )
        : M_timeable ( timeable )
      { }

    Timeable & getTimeableRef()
      {
          return M_timeable;
      }

private:
    Timeable & M_timeable;            /**< reference to the timeable object */
};

#endif
