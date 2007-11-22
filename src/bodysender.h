// -*-c++-*-

/***************************************************************************
                                bodysender.h
                  Classes for sending sense body messages
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


#ifndef RCSSBODYSENDER_H
#define RCSSBODYSENDER_H

#include "sender.h"
#include "observer.h"
#include <rcssbase/lib/factory.hpp>

class Player;

namespace rcss
{
class SerializerPlayer;

/*!
//===================================================================
//
//  CLASS: BodySender
//
//  DESC: Base class for the sense body protocol.
//
//===================================================================
*/

class BodySender
    : protected Sender
{
public:
    BodySender( std::ostream& transport );

    virtual
    ~BodySender();

    virtual
    void sendBody() = 0;
};

/*!
//===================================================================
//
//  CLASS: BodyObserver
//
//  DESC: Interface for an object that receives sense body information.
//
//===================================================================
*/

// template< typename S >
// class BodyObserver
//     : protected BaseObserver< S >
// {
// public:
//     typedef S BodySender;

//     BodyObserver()
//       {}

//     BodyObserver( BodySender& sender )
//         : BaseObserver< BodySender >( sender )
//       {}

//     BodyObserver( std::auto_ptr< BodySender > sender )
//         : BaseObserver< BodySender >( sender )
//       {}

//     ~BodyObserver()
//       {}

//     void
//     setBodySender( BodySender& sender )
//       { BaseObserver< BodySender >::setSender( sender ); }

//     void
//     setBodySender( std::auto_ptr< BodySender > sender )
//       { BaseObserver< BodySender >::setSender( sender ); }

//     void
//     sendBody()
//       { BaseObserver< BodySender >::sender().sendBody(); }
// };

/*!
//===================================================================
//
//  CLASS: BodySenderPlayer
//
//  DESC: Base class for the sense body protocol for players.
//
//===================================================================
*/

class BodySenderPlayer
    : public BodySender
{
public:
    typedef BodySenderPlayer Base;

    class Params {
    public:
        std::ostream & m_transport;
        const Player & m_self;
        const SerializerPlayer & m_ser;

        Params( std::ostream & transport,
                const Player & self,
                const SerializerPlayer & ser )
            : m_transport( transport )
            , m_self( self )
            , m_ser( ser )
          {}
    };

    typedef std::auto_ptr< BodySenderPlayer > Ptr;
    typedef Ptr (*Creator)( const Params & );
    typedef rcss::lib::Factory< Creator, int > Factory;

    static
    Factory & factory();

    BodySenderPlayer( const Params & params );

    virtual
    ~BodySenderPlayer();

protected:
    const
    SerializerPlayer & serializer() const
      {
          return M_serializer;
      }

    const
    Player & self() const
      {
          return M_self;
      }

private:
    const SerializerPlayer & M_serializer;

    /*:TODO: M_self needs to be replaced with a reference to a
      BodyObserver and BodyObserver should have virtual functions for
      stuff like velocity, stamina, etc */
    const Player & M_self;

};


/*!
//===================================================================
//
//  CLASS: BodyObserverPlayer
//
//  DESC: Interface for an object that receives sense body information.
//
//===================================================================
*/

class BodyObserverPlayer
    : protected BaseObserver< BodySenderPlayer >
{
public:

    BodyObserverPlayer()
      { }

    BodyObserverPlayer( BodySenderPlayer & sender )
        : BaseObserver< BodySenderPlayer >( sender )
      { }

    BodyObserverPlayer( std::auto_ptr< BodySenderPlayer > sender )
        : BaseObserver< BodySenderPlayer >( sender )
      { }

    ~BodyObserverPlayer()
      { }

    void
    setBodySender( BodySenderPlayer & sender )
      {
          BaseObserver< BodySenderPlayer >::setSender( sender );
      }

    void
    setBodySender( std::auto_ptr< BodySenderPlayer > sender )
      {
          BaseObserver< BodySenderPlayer >::setSender( sender );
      }

    void
    sendBody()
      {
          BaseObserver< BodySenderPlayer >::sender().sendBody();
      }
};


/*!
//===================================================================
//
//  CLASS: BodySenderPlayerV1
//
//  DESC: version 1 of the sense body protocol.
//
//===================================================================
*/

class BodySenderPlayerV1
    : public BodySenderPlayer
{
public:

    BodySenderPlayerV1( const Params & params );

    virtual
    ~BodySenderPlayerV1();

    virtual
    void sendBody();

protected:
    virtual
    void sendBodyData();

    virtual
    void sendVelocity();

    virtual
    void sendNeck();

    virtual
    void sendCounts();

public:

};

/*!
//===================================================================
//
//  CLASS: BodySenderPlayerV5
//
//  DESC: version 5 of the sense body protocol. Head angle and neck
//        turn count added
//
//===================================================================
*/

class BodySenderPlayerV5
    : public BodySenderPlayerV1
{
public:
    BodySenderPlayerV5( const Params& params );

    virtual
    ~BodySenderPlayerV5();

protected:
    virtual
    void sendNeck();

    virtual
    void sendCounts();
};

/*!
//===================================================================
//
//  CLASS: BodySenderPlayerV6
//
//  DESC: version 6 of the sense body protocol. Velocity direction
//        added
//
//===================================================================
*/

class BodySenderPlayerV6
    : public BodySenderPlayerV5
{
public:
    BodySenderPlayerV6( const Params& params );

    virtual
    ~BodySenderPlayerV6();

protected:
    virtual
    void sendVelocity();
};

/*!
//===================================================================
//
//  CLASS: BodySenderPlayerV7
//
//  DESC: version 7 of the sense body protocol. Extra counts added
//
//===================================================================
*/

class BodySenderPlayerV7
    : public BodySenderPlayerV6
{
public:
    BodySenderPlayerV7( const Params& params );

    virtual
    ~BodySenderPlayerV7();

protected:
    virtual
    void sendCounts();

};

/*!
//===================================================================
//
//  CLASS: BodySenderPlayerV8
//
//  DESC: version 8 of the sense body protocol. Added arm, focus and
//        tackle details
//
//===================================================================
*/

class BodySenderPlayerV8
    : public BodySenderPlayerV7
{
public:
    BodySenderPlayerV8( const Params& params );

    virtual
    ~BodySenderPlayerV8();

protected:
    virtual
    void sendBodyData();

};

/*!
//===================================================================
//
//  CLASS: BodySenderPlayerV12
//
//  DESC: version 12 of the sense body protocol. Added collision info.
//
//===================================================================
*/

class BodySenderPlayerV12
    : public BodySenderPlayerV8
{
public:
    BodySenderPlayerV12( const Params & params );

    virtual
    ~BodySenderPlayerV12();

protected:
    virtual
    void sendBodyData();

};

}

#endif
