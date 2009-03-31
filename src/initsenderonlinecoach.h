// -*-c++-*-

/***************************************************************************
                           initsenderonlinecoach.h
             Classes for sending sense init messages for online coaches
                             -------------------
    begin                : 2008-02-17
    copyright            : (C) 2002 by The RoboCup Soccer Simulator
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


#ifndef RCSS_INITSENDER_ONLINE_COACH_H
#define RCSS_INITSENDER_ONLINE_COACH_H

#include "initsender.h"

#include <rcssbase/factory.hpp>

#include <boost/shared_ptr.hpp>

class Stadium;
class Player;
class OnlineCoach;

namespace rcss {

class SerializerOnlineCoach;

/*!
//===================================================================
//
//  CLASS: InitSenderOnlineCoach
//
//  DESC: Base class for the init protocol for online coaches.
//
//===================================================================
*/

class InitSenderOnlineCoach
    : public InitSender {
public:
    class Params {
    public:
        std::ostream & M_transport;
        const OnlineCoach & M_self;
        const boost::shared_ptr< SerializerOnlineCoach > M_serializer;
        const Stadium & M_stadium;

        Params( std::ostream & transport,
                const OnlineCoach & self,
                const boost::shared_ptr< SerializerOnlineCoach > serializer,
                const Stadium & stadium )
            : M_transport( transport )
            , M_self( self )
            , M_serializer( serializer )
            , M_stadium( stadium )
          { }
    };

public:
    typedef std::auto_ptr< rcss::InitSenderOnlineCoach > Ptr;
    typedef Ptr (*Creator)( const Params & );
    typedef rcss::Factory< Creator, int > FactoryHolder;

    static
    FactoryHolder & factory();

    virtual
    ~InitSenderOnlineCoach();

protected:
    InitSenderOnlineCoach( const Params & params,
                           const boost::shared_ptr< InitSenderCommon > common );

    const
    SerializerOnlineCoach & serializer() const
      {
          return *M_serializer;
      }

    const
    OnlineCoach & self() const
      {
          return M_self;
      }

    const
    Stadium & stadium() const
      {
          return M_stadium;
      }

public:

    void sendPlayerClangVer();

    virtual
    void sendPlayerClangVer( const Player & ) = 0;

private:
    const boost::shared_ptr< SerializerOnlineCoach > M_serializer;

    /*:TODO: M_self needs to be replaced with a reference to a
      InitObserver and InitObserver should have virtual functions for
      stuff like velocity, stamina, etc */
    const OnlineCoach & M_self;
    const Stadium & M_stadium;
};


/*!
//===================================================================
//
//  CLASS: InitObserverOnlineCoach
//
//  DESC: Interface for an object that receives init information.
//
//===================================================================
*/

class InitObserverOnlineCoach
    : protected BaseObserver< InitSenderOnlineCoach > {
public:

    InitObserverOnlineCoach()
      { }

    InitObserverOnlineCoach( InitSenderOnlineCoach & sender )
        : BaseObserver< InitSenderOnlineCoach >( sender )
      { }

    InitObserverOnlineCoach( std::auto_ptr< InitSenderOnlineCoach > sender )
        : BaseObserver< InitSenderOnlineCoach >( sender )
      { }

    ~InitObserverOnlineCoach()
      { }

    void setInitSender( InitSenderOnlineCoach & sender )
      {
          BaseObserver< InitSenderOnlineCoach >::setSender( sender );
      }

    void setInitSender( std::auto_ptr< InitSenderOnlineCoach > sender )
      {
          BaseObserver< InitSenderOnlineCoach >::setSender( sender );
      }

    void sendInit()
      {
          BaseObserver< InitSenderOnlineCoach >::sender().sendInit();
      }

    void sendServerParams()
      {
          BaseObserver< InitSenderOnlineCoach >::sender().sendServerParams();
      }

    void sendPlayerParams()
      {
          BaseObserver< InitSenderOnlineCoach >::sender().sendPlayerParams();
      }

    void sendPlayerTypes()
      {
          BaseObserver< InitSenderOnlineCoach >::sender().sendPlayerTypes();
      }

    void sendChangedPlayers()
      {
          BaseObserver< InitSenderOnlineCoach >::sender().sendChangedPlayers();
      }

    void sendScore()
      {
          BaseObserver< InitSenderOnlineCoach >::sender().sendScore();
      }

    void sendPlayerClangVer()
      {
          BaseObserver< InitSenderOnlineCoach >::sender().sendPlayerClangVer();
      }

    void sendPlayerClangVer( const Player & player )
      {
          BaseObserver< InitSenderOnlineCoach >::sender().sendPlayerClangVer( player );
      }
};


/*!
//===================================================================
//
//  CLASS: InitSenderOnlineCoachV1
//
//  DESC: version 1 of the init protocol.
//
//===================================================================
*/

class InitSenderOnlineCoachV1
    : public InitSenderOnlineCoach {
public:
    InitSenderOnlineCoachV1( const Params & params );

protected:
    InitSenderOnlineCoachV1( const Params & params,
                             const boost::shared_ptr< InitSenderCommon > common );

public:
    virtual
    ~InitSenderOnlineCoachV1();

    virtual
    void sendInit();

    virtual
    void sendScore();

    virtual
    void sendChangedPlayers();

    virtual
    void sendPlayerClangVer( const Player & )
      { }
};


/*!
//===================================================================
//
//  CLASS: InitSenderOnlineCoachV6
//
//  DESC: version 6 of the init protocol.
//
//===================================================================
*/

class InitSenderOnlineCoachV6
    : public InitSenderOnlineCoachV1 {
public:
    InitSenderOnlineCoachV6( const Params & params );

protected:
    InitSenderOnlineCoachV6( const Params & params,
                             const boost::shared_ptr< InitSenderCommon > common );

public:
    virtual
    ~InitSenderOnlineCoachV6();

    virtual
    void sendInit();
};


/*!
//===================================================================
//
//  CLASS: InitSenderOnlineCoachV7
//
//  DESC: version 7 of the init protocol.
//
//===================================================================
*/

class InitSenderOnlineCoachV7
    : public InitSenderOnlineCoachV6 {
public:
    InitSenderOnlineCoachV7( const Params & params );

protected:
    InitSenderOnlineCoachV7( const Params & params,
                             const boost::shared_ptr< InitSenderCommon > common );

public:
    virtual
    ~InitSenderOnlineCoachV7();

    virtual
    void sendChangedPlayers();
};

/*!
//===================================================================
//
//  CLASS: InitSenderOnlineCoachV8
//
//  DESC: version 8 of the init protocol.
//
//===================================================================
*/

class InitSenderOnlineCoachV8
    : public InitSenderOnlineCoachV7 {
public:
    InitSenderOnlineCoachV8( const Params & params );

protected:
    InitSenderOnlineCoachV8( const Params & params,
                             const boost::shared_ptr< InitSenderCommon > common );

public:
    virtual
    ~InitSenderOnlineCoachV8();

    virtual
    void sendPlayerClangVer( const Player & player );
};


}

#endif
