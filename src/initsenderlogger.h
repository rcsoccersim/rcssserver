// -*-c++-*-

/***************************************************************************
                             initsenderlogger.h
                  Classes for writing init messages for logger
                             -------------------
    begin                : 2008-02-16
    copyright            : (C) 2008 by The RoboCup Soccer Simulator
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


#ifndef RCSS_INITSENDER_LOGGER_H
#define RCSS_INITSENDER_LOGGER_H

#include "initsender.h"

#include <rcssbase/factory.hpp>

#include <boost/shared_ptr.hpp>

class Logger;

namespace rcss {

class SerializerMonitor;

class InitSenderLogger
    : public InitSender {
public:
    class Params {
    public:
        std::ostream & M_transport;
        const Logger & M_self;
        const boost::shared_ptr< SerializerMonitor > M_serializer;
        const Stadium & M_stadium;

        Params( std::ostream & transport,
                const Logger & self,
                const boost::shared_ptr< SerializerMonitor > serializer,
                const Stadium & stadium )
            : M_transport( transport ),
              M_self( self ),
              M_serializer( serializer ),
              M_stadium( stadium )
          { }
    };

    typedef std::auto_ptr< InitSenderLogger > Ptr;
    typedef Ptr (*Creator)( const Params & );
    typedef rcss::Factory< Creator, int > FactoryHolder;

private:

    const boost::shared_ptr< SerializerMonitor > M_serializer;
    /*:TODO: M_self needs to be replaced with a reference to a
      InitObserver and InitObserver should have virtual functions for
      stuff like velocity, stamina, etc */
    const Logger & M_self;
    const Stadium & M_stadium;

public:
    static
    FactoryHolder & factory();

    virtual
    ~InitSenderLogger();

protected:

    InitSenderLogger( const Params & params,
                      const boost::shared_ptr< InitSenderCommon > common );

    const
    SerializerMonitor & serializer() const
      {
          return *M_serializer;
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

public:

    void sendInit()
      { }
    void sendChangedPlayers()
      { }
    void sendScore()
      { }

    virtual
    void sendHeader() = 0;

    virtual
    void sendPlayMode() = 0;

    virtual
    void sendTeam() = 0;

};


/*!
  \class InitObserverLogger
 */
class InitObserverLogger
    : protected BaseObserver< InitSenderLogger > {
public:

    InitObserverLogger()
      { }

    InitObserverLogger( InitSenderLogger & sender )
        : BaseObserver< InitSenderLogger >( sender )
      { }

    ~InitObserverLogger()
      { }

    void setInitSender( InitSenderLogger & sender )
      {
          BaseObserver< InitSenderLogger >::setSender( sender );
      }

    void setInitSender( std::auto_ptr< InitSenderLogger > sender )
      {
          BaseObserver< InitSenderLogger >::setSender( sender );
      }

    void sendHeader()
      {
          BaseObserver< InitSenderLogger >::sender().sendHeader();
      }

    void sendServerParams()
      {
          BaseObserver< InitSenderLogger >::sender().sendServerParams();
      }

    void sendPlayerParams()
      {
          BaseObserver< InitSenderLogger >::sender().sendPlayerParams();
      }

    void sendPlayerTypes()
      {
          BaseObserver< InitSenderLogger >::sender().sendPlayerTypes();
      }

    void sendPlayMode()
      {
          BaseObserver< InitSenderLogger >::sender().sendPlayMode();
      }

    void sendTeam()
      {
          BaseObserver< InitSenderLogger >::sender().sendTeam();
      }

};


class InitSenderLoggerV1
    : public InitSenderLogger {
public:
    InitSenderLoggerV1( const Params & params );

protected:
    InitSenderLoggerV1( const Params & params,
                        const boost::shared_ptr< InitSenderCommon > common );

public:
    virtual
    ~InitSenderLoggerV1();

    virtual
    void sendHeader();

    virtual
    void sendServerParams();

    virtual
    void sendPlayerParams();

    virtual
    void sendPlayerTypes();

    virtual
    void sendPlayMode();

    virtual
    void sendTeam();

};

class InitSenderLoggerV2
    : public InitSenderLoggerV1 {
public:
    InitSenderLoggerV2( const Params & params );

protected:
    InitSenderLoggerV2( const Params & params,
                        const boost::shared_ptr< InitSenderCommon > common );

public:
    virtual
    ~InitSenderLoggerV2();

    virtual
    void sendHeader();

    virtual
    void sendServerParams();

    virtual
    void sendPlayerParams();

    virtual
    void sendPlayerTypes();

    virtual
    void sendPlayMode();

    virtual
    void sendTeam();

};

class InitSenderLoggerV3
    : public InitSenderLoggerV2 {
public:
    InitSenderLoggerV3( const Params & params );

protected:
    InitSenderLoggerV3( const Params & params,
                        const boost::shared_ptr< InitSenderCommon > common );

public:
    virtual
    ~InitSenderLoggerV3();

    virtual
    void sendHeader();

    virtual
    void sendServerParams();

    virtual
    void sendPlayerParams();

    virtual
    void sendPlayerTypes();

    virtual
    void sendPlayMode();

    virtual
    void sendTeam();

};


class InitSenderLoggerV4
    : public InitSenderLoggerV3 {
public:
    InitSenderLoggerV4( const Params & params );

protected:
    InitSenderLoggerV4( const Params & params,
                        const boost::shared_ptr< InitSenderCommon > common );

public:
    virtual
    ~InitSenderLoggerV4();

    virtual
    void sendHeader();

    virtual
    void sendServerParams();

    virtual
    void sendPlayerParams();

    virtual
    void sendPlayerTypes();

    virtual
    void sendPlayMode();

    virtual
    void sendTeam();

};


class InitSenderLoggerV5
    : public InitSenderLoggerV4 {
public:
    InitSenderLoggerV5( const Params & params );

protected:
    InitSenderLoggerV5( const Params & params,
                        const boost::shared_ptr< InitSenderCommon > common );

public:
    virtual
    ~InitSenderLoggerV5();

    virtual
    void sendHeader();

};

}

#endif
