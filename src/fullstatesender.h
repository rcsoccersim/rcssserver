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
 *   Foundation; either version 2 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/


#ifndef RCSSFULLSTATESENDER_H
#define RCSSFULLSTATESENDER_H

#include <iostream>
#include <memory>
#include <map>
#include <rcssbase/lib/factory.hpp>

class Stadium;
class Player;

namespace rcss
{
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
{
private:
    std::ostream& M_transport;

protected:
    std::ostream&
    transport()
      { return M_transport; }

public:
    typedef std::auto_ptr< FullStateSender > Ptr;

    FullStateSender( std::ostream& transport );

    virtual
    ~FullStateSender();

    virtual
    void
    sendFullState() = 0;
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
{
private:
    FullStateSender* M_sender;
    bool M_owns_sender;

public:
    FullStateObserver();
    FullStateObserver( FullStateSender& sender );
    FullStateObserver( std::auto_ptr< FullStateSender > sender );

    ~FullStateObserver();

    void
    setFullStateSender( FullStateSender& sender );

    void
    setFullStateSender( std::auto_ptr< FullStateSender > sender );

    void
    sendFullState();

private:
    void
    clear();
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
    : public FullStateSender
{
private:
    const SerializerPlayer& M_serializer;

    /*:TODO: M_self needs to be replaced with a reference to a
      FullStateObserver and FullStateObserver should have virtual functions for
      stuff like velocity, stamina, etc */
    const Player& M_self;
    const Stadium& M_stadium;

protected:
    const SerializerPlayer&
    serializer() const
      { return M_serializer; }

    const Player&
    self() const
      { return M_self; }

    const Stadium&
    stadium() const
      { return M_stadium; }

public:
    class Params
    {
    public:
        Params( std::ostream& transport,
                const Player& self,
                const Stadium& stadium,
                const SerializerPlayer& serializer )
            : m_transport( transport ),
              m_self( self ),
              m_stadium( stadium ),
              m_serializer( serializer )
          {}

        std::ostream& m_transport;
        const Player& m_self;
        const Stadium& m_stadium;
        const SerializerPlayer& m_serializer;
    };

    typedef Ptr(*Creator)( const Params& );
    typedef rcss::lib::Factory< Creator, int > Factory;

    static
    Factory&
    factory();

    FullStateSenderPlayer( const Params& params );

    virtual
    ~FullStateSenderPlayer();
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
    : public FullStateSenderPlayer
{
public:
    FullStateSenderPlayerV1( const Params& );

    virtual
    ~FullStateSenderPlayerV1();

    virtual
    void
    sendFullState();
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
    : public FullStateSenderPlayerV1
{
public:
    FullStateSenderPlayerV5( const Params& );

    virtual
    ~FullStateSenderPlayerV5();

    virtual
    void
    sendFullState();

protected:
    virtual
    void
    sendSelf();

    virtual
    void
    sendScore();

    virtual
    void
    sendBall();

    virtual
    void
    sendPlayer( const Player& p );
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
    FullStateSenderPlayerV8( const Params& );

    virtual
    ~FullStateSenderPlayerV8();

protected:
    virtual
    void
    sendSelf();

    virtual
    void
    sendScore();

    virtual
    void
    sendBall();

    virtual
    void
    sendPlayer( const Player& p );
};
}

#endif
