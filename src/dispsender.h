// -*-c++-*-

/***************************************************************************
                                dispsender.h
                     Classes for building display messages
                             -------------------
    begin                : 2007-11-22
    copyright            : (C) 2007 by The RoboCup Soccer Server
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

#ifndef RCSS_DISP_SENDER_H
#define RCSS_DISP_SENDER_H

#include "sender.h"
#include "observer.h"

#include <rcssbase/lib/factory.hpp>

class Stadium;
class Monitor;

namespace rcss {

class SerializerMonitor;

/*!
  \class DispSender
  \brief Base class for the display protocol.
*/
class DispSender
    : protected Sender {
public:

    DispSender( std::ostream & transport );

    virtual
    ~DispSender();

    virtual
    void sendShow() = 0;

    virtual
    void sendMsg() = 0;

};

/*!
  \class DispSenderMonitor
  \brief Base class for the visual protocol for monitors.
*/
class DispSenderMonitor
    : public DispSender {
public:

    class Params {
    public:
        std::ostream & M_transport;
        const Monitor & M_self;
        const SerializerMonitor & M_serializer;
        const Stadium & M_stadium;

        Params( std::ostream & transport,
                const Monitor & self,
                const SerializerMonitor & serializer,
                const Stadium & stadium )
            : M_transport( transport )
            , M_self( self )
            , M_serializer( serializer )
            , M_stadium( stadium )
          { }
    };


private:
    const SerializerMonitor & M_serializer;

    /*:TODO: M_self needs to be replaced with a reference to a
      Observer and Observer should have virtual functions for
      stuff like velocity, stamina, etc */
    const Monitor & M_self;
    const Stadium & M_stadium;

public:
    typedef std::auto_ptr< DispSenderMonitor > Ptr;
    typedef Ptr (*Creator)( const DispSenderMonitor::Params & );
    typedef rcss::lib::Factory< Creator, int > Factory;

    static
    Factory & factory();

    DispSenderMonitor( const Params & params );

    ~DispSenderMonitor();

protected:

    const
    SerializerMonitor & serializer() const
      {
          return M_serializer;
      }

    const
    Monitor & self() const
      { return M_self; }

    const
    Stadium & stadium() const
      {
          return M_stadium;
      }

};


/*!
  \class ObserverMonitor
  \brief Interface for players that receives display information.
*/
class ObserverMonitor
    : protected BaseObserver< DispSenderMonitor >
{
public:

    ObserverMonitor()
      { }

    ObserverMonitor( DispSenderMonitor & sender )
        : BaseObserver< DispSenderMonitor >( sender )
      { }

    ObserverMonitor( std::auto_ptr< DispSenderMonitor > sender )
        : BaseObserver< DispSenderMonitor >( sender )
      { }

    ~ObserverMonitor()
      { }

    void setDispSender( DispSenderMonitor & sender )
      {
          BaseObserver< DispSenderMonitor >::setSender( sender );
      }

    void setDispSender( std::auto_ptr< DispSenderMonitor > sender )
      {
          BaseObserver< DispSenderMonitor >::setSender( sender );
      }

    void sendShow()
      {
          BaseObserver< DispSenderMonitor >::sender().sendShow();
      }

    void sendMsg()
      {
          BaseObserver< DispSenderMonitor >::sender().sendMsg();
      }

};


/*!
  \class DispSenderMonitorV1
  \brief class for the version 1 display protocol.
*/
class DispSenderMonitorV1
    : public DispSenderMonitor {
public:

    DispSenderMonitorV1( const Params & params );

    virtual
    ~DispSenderMonitorV1();

    virtual
    void sendShow();

    virtual
    void sendMsg();

};


/*!
  \class DispSenderMonitorV2
  \brief class for the version 2 display protocol.
*/
class DispSenderMonitorV2
    : public DispSenderMonitorV1 {
public:

    DispSenderMonitorV2( const Params & params );

    virtual
    ~DispSenderMonitorV2();

    virtual
    void sendShow();

    virtual
    void sendMsg();

};

/*!
  \class DispSenderMonitorV2
  \brief class for the version 3 display protocol.
*/
class DispSenderMonitorV3
    : public DispSenderMonitorV2 {
public:

    DispSenderMonitorV3( const Params & params );

    virtual
    ~DispSenderMonitorV3();

    virtual
    void sendShow();

    virtual
    void sendMsg();

};


}

#endif
