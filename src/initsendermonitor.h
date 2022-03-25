// -*-c++-*-

/***************************************************************************
                             initsendermonitor.h
               Classes for sending sense init messages for monitors
                             -------------------
    begin                : 2007-11-21
    copyright            : (C) 2007 by The RoboCup Soccer Simulator
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


#ifndef RCSS_INITSENDER_MONITOR_H
#define RCSS_INITSENDER_MONITOR_H

#include "initsender.h"

#include <rcss/factory.hpp>

#include <memory>

class Monitor;

namespace rcss {

class SerializerMonitor;

/*!
  \class InitSenderMonitor
  \brief Base class for the init protocol for monitors.
*/
class InitSenderMonitor
    : public InitSender {
public:
    class Params {
    public:
        std::ostream & M_transport;
        const Monitor & M_self;
        const std::shared_ptr< SerializerMonitor > M_serializer;
        const Stadium & M_stadium;

        Params( std::ostream & transport,
                const Monitor & self,
                const std::shared_ptr< SerializerMonitor > serializer,
                const Stadium & stadium )
            : M_transport( transport ),
              M_self( self ),
              M_serializer( serializer ),
              M_stadium( stadium )
          { }
    };

    typedef std::shared_ptr< InitSenderMonitor > Ptr;
    typedef Ptr (*Creator)( const Params & );
    typedef rcss::Factory< Creator, int > FactoryHolder;

private:

    const std::shared_ptr< SerializerMonitor > M_serializer;
    /*:TODO: M_self needs to be replaced with a reference to a
      InitObserver and InitObserver should have virtual functions for
      stuff like velocity, stamina, etc */
    const Monitor & M_self;
    const Stadium & M_stadium;

public:
    static
    FactoryHolder & factory();

   virtual
    ~InitSenderMonitor() override;

protected:

    InitSenderMonitor( const Params & params,
                       const std::shared_ptr< InitSenderCommon > common );

    const
    SerializerMonitor & serializer() const
      {
          return *M_serializer;
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

public:

    // void sendReconnect()
    //   { }

    virtual
    void sendPlayMode() = 0;

};

/*!
  \class InitObserverMonitor
  \brief interface for an object that receives init information
 */
class InitObserverMonitor
    : protected BaseObserver< InitSenderMonitor > {
public:

    InitObserverMonitor()
      { }

    ~InitObserverMonitor()
      { }

    void setInitSender( std::shared_ptr< InitSenderMonitor > sender )
      {
          BaseObserver< InitSenderMonitor >::setSender( sender );
      }

    void sendInit()
      {
          BaseObserver< InitSenderMonitor >::sender().sendInit();
      }

    void sendServerParams()
      {
          BaseObserver< InitSenderMonitor >::sender().sendServerParams();
      }

    void sendPlayerParams()
      {
          BaseObserver< InitSenderMonitor >::sender().sendPlayerParams();
      }

    void sendPlayerTypes()
      {
          BaseObserver< InitSenderMonitor >::sender().sendPlayerTypes();
      }

    void sendChangedPlayers()
      {
          BaseObserver< InitSenderMonitor >::sender().sendChangedPlayers();
      }

    void sendScore()
      {
          BaseObserver< InitSenderMonitor >::sender().sendScore();
      }

    void sendPlayMode()
      {
          BaseObserver< InitSenderMonitor >::sender().sendPlayMode();
      }

};


/*!
  \class InitSenderMonitorV1
  \brief version 1 of the init protocol.
 */
class InitSenderMonitorV1
    : public InitSenderMonitor {
public:
    InitSenderMonitorV1( const Params & params );

protected:
    InitSenderMonitorV1( const Params & params,
                         const std::shared_ptr< InitSenderCommon > common );

public:
    virtual
    ~InitSenderMonitorV1();

    virtual
    void sendInit() override;

    virtual
    void sendServerParams() override;

    virtual
    void sendPlayerParams() override;

    virtual
    void sendPlayerTypes() override;

    virtual
    void sendChangedPlayers() override;

    virtual
    void sendScore() override;

    virtual
    void sendPlayMode() override;
};


/*!
  \class InitSenderMonitorV2
  \brief version 2 of the init protocol.
 */
class InitSenderMonitorV2
    : public InitSenderMonitorV1 {
public:
    InitSenderMonitorV2( const Params & params );

protected:
    InitSenderMonitorV2( const Params & params,
                         const std::shared_ptr< InitSenderCommon > common );

public:
    virtual
    ~InitSenderMonitorV2() override;

    virtual
    void sendServerParams() override;

    virtual
    void sendPlayerParams() override;

    virtual
    void sendPlayerTypes() override;
};


/*!
  \class InitSenderMonitorV3
  \brief version 3 of the init protocol.

  The version of the common sender is "8".
 */
class InitSenderMonitorV3
    : public InitSenderMonitorV2 {
public:
    InitSenderMonitorV3( const Params & params );

protected:
    InitSenderMonitorV3( const Params & params,
                         const std::shared_ptr< InitSenderCommon > common );

public:
    virtual
    ~InitSenderMonitorV3() override;

    virtual
    void sendServerParams() override;

    virtual
    void sendPlayerParams() override;

    virtual
    void sendPlayerTypes() override;

    virtual
    void sendScore() override;

    virtual
    void sendPlayMode() override;
};


/*!
  \class InitSenderMonitorJSON
  \brief version 5 of the init protocol (JSON format).

  The version of the common sender is "-1".
 */
class InitSenderMonitorJSON
    : public InitSenderMonitor {
public:
    InitSenderMonitorJSON( const Params & params );

protected:
    InitSenderMonitorJSON( const Params & params,
                           const std::shared_ptr< InitSenderCommon > common );

public:
    virtual
    ~InitSenderMonitorJSON() override;

    virtual
    void sendInit() override;

    virtual
    void sendServerParams() override;

    virtual
    void sendPlayerParams() override;

    virtual
    void sendPlayerTypes() override;

    virtual
    void sendChangedPlayers() override;

    virtual
    void sendScore() override;

    virtual
    void sendPlayMode() override;
};


}

#endif
