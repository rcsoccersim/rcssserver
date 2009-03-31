// -*-c++-*-

/***************************************************************************
                             initsenderplayer.h
                  Classes for sending sense init messages to players
                             -------------------
    begin                : 2008-02-17
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


#ifndef RCSS_INITSENDER_PLAYER_H
#define RCSS_INITSENDER_PLAYER_H

#include "initsender.h"

#include <rcssbase/factory.hpp>

#include <boost/shared_ptr.hpp>

class Player;

namespace rcss {

class SerializerPlayer;

/*!
//===================================================================
//
//  CLASS: InitSenderPlayer
//
//  DESC: Base class for the init protocol for players.
//
//===================================================================
*/

class InitSenderPlayer
    : public InitSender {
public:
    class Params {
    public:
        std::ostream & M_transport;
        const Player & M_self;
        const boost::shared_ptr< SerializerPlayer > M_serializer;
        const Stadium & M_stadium;

        Params( std::ostream & transport,
                const Player & self,
                const boost::shared_ptr< SerializerPlayer > serializer,
                const Stadium & stadium )
            : M_transport( transport )
            , M_self( self )
            , M_serializer( serializer )
            , M_stadium( stadium )
          { }
    };

public:
    typedef std::auto_ptr< InitSenderPlayer > Ptr;
    typedef Ptr (*Creator)( const Params & );
    typedef rcss::Factory< Creator, int > FactoryHolder;

    static
    FactoryHolder & factory();

    virtual
    ~InitSenderPlayer();

protected:
    InitSenderPlayer( const Params & params,
                      const boost::shared_ptr< InitSenderCommon > common );

    const
    SerializerPlayer & serializer() const
      {
          return *M_serializer;
      }

    const
    Player & self() const
      {
          return M_self;
      }

    const
    Stadium & stadium() const
      {
          return M_stadium;
      }

private:
    const boost::shared_ptr< SerializerPlayer > M_serializer;

    /*:TODO: M_self needs to be replaced with a reference to a
      InitObserver and InitObserver should have virtual functions for
      stuff like velocity, stamina, etc */
    const Player & M_self;
    const Stadium & M_stadium;

public:

    virtual
    void sendReconnect() = 0;

};

/*!
//===================================================================
//
//  CLASS: InitObserverPlayer
//
//  DESC: Interface for an object that receives init information.
//
//===================================================================
*/

class InitObserverPlayer
    : protected BaseObserver< InitSenderPlayer > {
public:

    InitObserverPlayer()
      { }

    InitObserverPlayer( InitSenderPlayer & sender )
        : BaseObserver< InitSenderPlayer >( sender )
      { }

    InitObserverPlayer( std::auto_ptr< InitSenderPlayer > sender )
        : BaseObserver< InitSenderPlayer >( sender )
      { }

    ~InitObserverPlayer()
      { }

    void setInitSender( InitSenderPlayer & sender )
      {
          BaseObserver< InitSenderPlayer >::setSender( sender );
      }

    void setInitSender( std::auto_ptr< InitSenderPlayer > sender )
      {
          BaseObserver< InitSenderPlayer >::setSender( sender );
      }

    void sendInit()
      {
          BaseObserver< InitSenderPlayer >::sender().sendInit();
      }

    void sendReconnect()
      {
          BaseObserver< InitSenderPlayer >::sender().sendReconnect();
      }

    void sendServerParams()
      {
          BaseObserver< InitSenderPlayer >::sender().sendServerParams();
      }

    void sendPlayerParams()
      {
          BaseObserver< InitSenderPlayer >::sender().sendPlayerParams();
      }

    void sendPlayerTypes()
      {
          BaseObserver< InitSenderPlayer >::sender().sendPlayerTypes();
      }

    void sendChangedPlayers()
      {
          BaseObserver< InitSenderPlayer >::sender().sendChangedPlayers();
      }

    void sendScore()
      {
          BaseObserver< InitSenderPlayer >::sender().sendScore();
      }

};

/*!
//===================================================================
//
//  CLASS: InitSenderPlayerV1
//
//  DESC: version 1 of the init protocol.
//
//===================================================================
*/

class InitSenderPlayerV1
    : public InitSenderPlayer {
public:
    InitSenderPlayerV1( const Params & params );

protected:
    InitSenderPlayerV1( const Params & params,
                        const boost::shared_ptr< InitSenderCommon > common );

public:
    virtual
    ~InitSenderPlayerV1();

    virtual
    void sendInit();

    virtual
    void sendReconnect();

    virtual
    void sendScore();

    virtual
    void sendChangedPlayers();
};

/*!
//===================================================================
//
//  CLASS: InitSenderPlayerV7
//
//  DESC: version 7 of the init protocol.
//
//===================================================================
*/

class InitSenderPlayerV7
    : public InitSenderPlayerV1 {
public:
    InitSenderPlayerV7( const Params & params );

protected:
    InitSenderPlayerV7( const Params & params,
                        const boost::shared_ptr< InitSenderCommon > common );

public:
    virtual
    ~InitSenderPlayerV7();

    virtual
    void sendChangedPlayers();
};

/*!
//===================================================================
//
//  CLASS: InitSenderPlayerV8
//
//  DESC: version 8 of the init protocol.
//
//===================================================================
*/

class InitSenderPlayerV8
    : public InitSenderPlayerV7 {
public:
    InitSenderPlayerV8( const Params & params );

protected:
    InitSenderPlayerV8( const Params & params,
                        const boost::shared_ptr< InitSenderCommon > common );

public:
    virtual
    ~InitSenderPlayerV8();
};

}

#endif
