// -*-c++-*-

/***************************************************************************
                                   timeale.h
           The timeable class is an abstract base class used by a timer
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

#ifndef TIMEABLE_H
#define TIMEABLE_H

/** The timeable class is an abstract class which specifies the virtual methods
    that should be implemented by the subclass. */

class Timeable {
public:
    virtual
    ~Timeable()
      { }

    void recvFromClients()   { doRecvFromClients();   }
    void newSimulatorStep()  { doNewSimulatorStep();  }
    void sendSenseBody()     { doSendSenseBody();     }
    void sendVisuals()       { doSendVisuals();       }
    void sendSynchVisuals()  { doSendSynchVisuals();  }
    void sendCoachMessages() { doSendCoachMessages(); }
    bool sendThink()         { return doSendThink();  }
    void quit()              { doQuit();              }
    bool alive() { return isAlive(); }

protected:

    // virtual methods that should be defined in subclass
    virtual void doRecvFromClients()   = 0;
    virtual void doNewSimulatorStep()  = 0;
    virtual void doSendSenseBody()     = 0;
    virtual void doSendVisuals()       = 0;
    virtual void doSendSynchVisuals()  = 0;
    virtual void doSendCoachMessages() = 0;
    virtual bool doSendThink()         = 0;
    virtual void doQuit()              = 0;
    virtual bool isAlive() = 0;
};

#endif
