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
class Logger;
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

//===================================================================
//===================================================================
//===================================================================

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
      {
          return M_self;
      }

    const
    Stadium & stadium() const
      {
          return M_stadium;
      }

};


/*!
  \class ObserverMonitor
  \brief Interface for monitors to send display information.
*/
class ObserverMonitor
    : protected BaseObserver< DispSenderMonitor > {
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


//===================================================================
//===================================================================
//===================================================================

class DispSenderLogger
    : public DispSender {
public:

    class Params {
    public:
        std::ostream & M_transport;
        const Logger & M_self;
        const SerializerMonitor & M_serializer;
        const Stadium & M_stadium;

        Params( std::ostream & transport,
                const Logger & self,
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
    const Logger & M_self;
    const Stadium & M_stadium;

public:
    typedef std::auto_ptr< DispSenderLogger > Ptr;
    typedef Ptr (*Creator)( const DispSenderLogger::Params & );
    typedef rcss::lib::Factory< Creator, int > Factory;

    static
    Factory & factory();

    DispSenderLogger( const Params & params );

    ~DispSenderLogger();

protected:

    const
    SerializerMonitor & serializer() const
      {
          return M_serializer;
      }

    const
    Logger & self() const
      {
          return M_self;
      }

    const
    Stadium & stadium() const
      {
          return M_stadium;
      }

};

/*!
  \class ObserverMonitor
  \brief Interface for logger to recorde the display information.
*/
class ObserverLogger
    : protected BaseObserver< DispSenderLogger > {
public:

    ObserverLogger()
      { }

    ObserverLogger( DispSenderLogger & sender )
        : BaseObserver< DispSenderLogger >( sender )
      { }

    ObserverLogger( std::auto_ptr< DispSenderLogger > sender )
        : BaseObserver< DispSenderLogger >( sender )
      { }

    ~ObserverLogger()
      { }

    void setDispSender( DispSenderLogger & sender )
      {
          BaseObserver< DispSenderLogger >::setSender( sender );
      }

    void setDispSender( std::auto_ptr< DispSenderLogger > sender )
      {
          BaseObserver< DispSenderLogger >::setSender( sender );
      }

    void sendShow()
      {
          BaseObserver< DispSenderLogger >::sender().sendShow();
      }

    void sendMsg()
      {
          BaseObserver< DispSenderLogger >::sender().sendMsg();
      }
};


/*!
  \class DispSenderLoggerV1
  \brief class for the log version 1
 */
class DispSenderLoggerV1
    : public DispSenderLogger {
public:

    DispSenderLoggerV1( const Params & params );

    virtual
    ~DispSenderLoggerV1();

    virtual
    void sendShow();

    virtual
    void sendMsg();

};


/*!
  \class DispSenderLoggerV2
  \brief class for the log version 2
 */
class DispSenderLoggerV2
    : public DispSenderLoggerV1 {
public:

    DispSenderLoggerV2( const Params & params );

    virtual
    ~DispSenderLoggerV2();

    virtual
    void sendShow();

    virtual
    void sendMsg();

};


/*!
  \class DispSenderLoggerV3
  \brief class for the log version 3
 */
class DispSenderLoggerV3
    : public DispSenderLoggerV2 {
public:

    DispSenderLoggerV3( const Params & params );

    virtual
    ~DispSenderLoggerV3();

    virtual
    void sendShow();

    virtual
    void sendMsg();

};

/*!
  \class DispSenderLoggerV4
  \brief class for the log version 4
 */
class DispSenderLoggerV4
    : public DispSenderLoggerV3 {
public:

    DispSenderLoggerV4( const Params & params );

    virtual
    ~DispSenderLoggerV4();

    virtual
    void sendShow();

    virtual
    void sendMsg();

};

}

#endif
