// -*-c++-*-

/***************************************************************************
                                fullstatesender.h
                  Classes for sending sense fullstate messages
                             -------------------
    begin                : 22-AUG-2002
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


#ifndef RCSSFULLSTATESENDER_H
#define RCSSFULLSTATESENDER_H

#include "sender.h"
#include "observer.h"

#include <rcss/factory.hpp>

#include <memory>

class Stadium;
class Player;

namespace rcss {

class SerializerPlayer;

/*!
//===================================================================
//
//  CLASS: FullstateSender
//
//  DESC: Base class for the full state protocol.
//
//===================================================================
*/

class FullStateSender
    : protected Sender {
protected:

    FullStateSender( std::ostream & transport );

public:
    virtual
    ~FullStateSender() override;

    virtual
    void sendFullState() = 0;
};

/*!
//===================================================================
//
//  CLASS: FullStateSenderPlayer
//
//  DESC: Base class for the full state protocol for players.
//
//===================================================================
*/

class FullStateSenderPlayer
    : public FullStateSender {
public:
    class Params {
    public:
        std::ostream & M_transport;
        const Player & M_self;
        const std::shared_ptr< SerializerPlayer > M_serializer;
        const Stadium & M_stadium;

        Params( std::ostream & transport,
                const Player & self,
                const std::shared_ptr< SerializerPlayer > serializer,
                const Stadium & stadium )
            : M_transport( transport ),
              M_self( self ),
              M_serializer( serializer ),
              M_stadium( stadium )
          { }
    };

    typedef std::shared_ptr< FullStateSenderPlayer > Ptr;
    typedef Ptr (*Creator)( const Params & );
    typedef rcss::Factory< Creator, int > FactoryHolder;

private:
    const std::shared_ptr< SerializerPlayer > M_serializer;

    /*:TODO: M_self needs to be replaced with a reference to a
      FullStateObserver and FullStateObserver should have virtual functions for
      stuff like velocity, stamina, etc */
    const Player & M_self;
    const Stadium & M_stadium;

public:
    static
    FactoryHolder & factory();

protected:
    FullStateSenderPlayer( const Params & params );

public:
    virtual
    ~FullStateSenderPlayer() override;

protected:
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

};


/*!
//===================================================================
//
//  CLASS: FullStateObserver
//
//  DESC: Interface for an object that receives full state information.
//
//===================================================================
*/

class FullStateObserver
    : protected BaseObserver< FullStateSenderPlayer > {
public:

    FullStateObserver()
      { }

    FullStateObserver( std::shared_ptr< FullStateSenderPlayer > sender )
        : BaseObserver< FullStateSenderPlayer >( sender )
      { }

    ~FullStateObserver()
      { }

    void setFullStateSender( std::shared_ptr< FullStateSenderPlayer > sender )
      {
          BaseObserver< FullStateSenderPlayer >::setSender( sender );
      }

    void sendFullState()
      {
          BaseObserver< FullStateSenderPlayer >::sender().sendFullState();
      }
};

/*!
//===================================================================
//
//  CLASS: FullStateSenderPlayerV1
//
//  DESC: version 1 of the full state protocol. No actual information
//        is sent in this version as the protocol did not exist until
//        version 5
//
//===================================================================
*/

class FullStateSenderPlayerV1
    : public FullStateSenderPlayer {
public:
    FullStateSenderPlayerV1( const Params & );

    virtual
    ~FullStateSenderPlayerV1() override;

    virtual
    void sendFullState() override;
};

/*!
//===================================================================
//
//  CLASS: FullStateSenderPlayerV5
//
//  DESC: version 5 of the full state protocol. This is the first
//        version in which full state information may be sent to
//        players.
//
//===================================================================
*/

class FullStateSenderPlayerV5
    : public FullStateSenderPlayerV1 {
public:
    FullStateSenderPlayerV5( const Params & );

    virtual
    ~FullStateSenderPlayerV5() override;

    virtual
    void sendFullState() override;

protected:
    virtual
    void sendSelf();

    virtual
    void sendScore();

    virtual
    void sendBall();

    virtual
    void sendPlayer( const Player & p );
};

/*!
//===================================================================
//
//  CLASS: FullStateSenderPlayerV8
//
//  DESC: version 8 of the full state protocol.
//
//===================================================================
*/

class FullStateSenderPlayerV8
    : public FullStateSenderPlayerV5
{
public:
    FullStateSenderPlayerV8( const Params & );

    virtual
    ~FullStateSenderPlayerV8() override;

protected:
    virtual
    void sendSelf() override;

    virtual
    void sendScore() override;

    virtual
    void sendBall() override;

    virtual
    void sendPlayer( const Player & p ) override;
};


/*!
//===================================================================
//
//  CLASS: FullStateSenderPlayerV13
//
//  DESC: version 13 of the full state protocol. The tackling or
//  kikcing information are added.
//
//===================================================================
*/

class FullStateSenderPlayerV13
    : public FullStateSenderPlayerV8
{
public:
    FullStateSenderPlayerV13( const Params & );

    virtual
    ~FullStateSenderPlayerV13() override;

protected:

    virtual
    void sendPlayer( const Player & p ) override;
};


}

#endif
