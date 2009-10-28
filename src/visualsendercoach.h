// -*-c++-*-

/***************************************************************************
                             visualsendercoach.h
                  Classes for building visual messages for coaches
                             -------------------
    begin                : 2008-02-17
    copyright            : (C) 2008 by The RoboCup Soccer Server
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

#ifndef RCSS_VISUALSENDER_COACH_H
#define RCSS_VISUALSENDER_COACH_H

#include "visual.h"

#include "observer.h"
#include "object.h"
#include "utility.h"

#include <rcssbase/factory.hpp>

#include <boost/shared_ptr.hpp>

class Stadium;
class Player;
class Coach;

namespace rcss {

class SerializerCoach;

/*!
//===================================================================
//
//  CLASS: VisualSenderCoach
//
//  DESC: Base class for the visual protocol for coaches.
//
//===================================================================
*/

class VisualSenderCoach
    : public VisualSender {
public:

    class Params {
    public:
        std::ostream & M_transport;
        const Coach & M_self;
        const boost::shared_ptr< SerializerCoach > M_serializer;
        const Stadium & M_stadium;

        Params( std::ostream & transport,
                const Coach & self,
                const boost::shared_ptr< SerializerCoach > serializer,
                const Stadium & stadium )
            : M_transport( transport ),
              M_self( self ),
              M_serializer( serializer ),
              M_stadium( stadium )
          { }
    };

public:
    typedef std::auto_ptr< VisualSenderCoach > Ptr;
    typedef Ptr (*Creator)( const VisualSenderCoach::Params & );
    typedef rcss::Factory< Creator, int > FactoryHolder;

    static
    FactoryHolder & factory();

    VisualSenderCoach( const Params & params );

    ~VisualSenderCoach();

protected:

    const
    SerializerCoach & serializer() const
      {
          return *M_serializer;
      }

    const
    Coach & self() const
      {
          return M_self;
      }

    const
    Stadium & stadium() const
      {
          return M_stadium;
      }

public:
    virtual
    void sendLook() = 0;

    virtual
    void sendOKEye() = 0;

private:

    const boost::shared_ptr< SerializerCoach > M_serializer;

    const Coach & M_self;
    const Stadium & M_stadium;

};

/*!
//===================================================================
//
//  CLASS: ObserverCoach
//
//  DESC: Interface for the coach that receives visual information.
//
//===================================================================
*/

class ObserverCoach
    : protected BaseObserver< VisualSenderCoach > {
public:

    ObserverCoach()
      { }

    ObserverCoach( VisualSenderCoach & sender )
        : BaseObserver< VisualSenderCoach >( sender )
      { }

    ObserverCoach( std::auto_ptr< VisualSenderCoach > sender )
        : BaseObserver< VisualSenderCoach >( sender )
      { }

    ~ObserverCoach()
      {}

    void setVisualSender( VisualSenderCoach & sender )
      {
          BaseObserver< VisualSenderCoach >::setSender( sender );
      }

    void setVisualSender( std::auto_ptr< VisualSenderCoach > sender )
      {
          BaseObserver< VisualSenderCoach >::setSender( sender );
      }

    void sendVisual()
      {
          BaseObserver< VisualSenderCoach >::sender().sendVisual();
      }

    void sendLook()
      {
          BaseObserver< VisualSenderCoach >::sender().sendLook();
      }

    void sendOKEye()
      {
          sender().sendOKEye();
      }
};


/*!
//===================================================================
//
//  CLASS: VisualSenderCoachV1
//
//  DESC:
//
//===================================================================
*/

class VisualSenderCoachV1
    : public VisualSenderCoach {
public:

    VisualSenderCoachV1( const Params & params );

    virtual
    ~VisualSenderCoachV1();

    void sendVisual();

    void sendLook();

    virtual
    void sendOKEye();

private:

    void sendGoals();

    void sendBall();

    void sendGoal( const PObject & goal );



protected:

    virtual
    void serializePlayer( const Player & player );

    virtual
    void serializePlayerLook( const Player & player );

    virtual
    int rad2Deg( const double & rad ) const
      {
          return Rad2IDeg( rad );
      }

    virtual
    const
    std::string & calcName( const PObject & obj ) const
      {
          return obj.name();
      }

};

/*!
//===================================================================
//
//  CLASS: VisualSenderCoachV7
//
//  DESC:
//
//===================================================================
*/

class VisualSenderCoachV7
    : public VisualSenderCoachV1 {
public:

    VisualSenderCoachV7( const Params & params );

    virtual
    ~VisualSenderCoachV7();

protected:

    virtual
    int rad2Deg( const double & rad ) const
      {
          return Rad2IDegRound( rad );
      }

    virtual
    const
    std::string & calcName( const PObject & obj ) const
      {
          return obj.shortName();
      }
};

/*!
//===================================================================
//
//  CLASS: VisualSenderCoachV8
//
//  DESC:
//
//===================================================================
*/

class VisualSenderCoachV8
    : public VisualSenderCoachV7 {
public:

    VisualSenderCoachV8( const Params & params );

    virtual
    ~VisualSenderCoachV8();

protected:

    int calcPointDir( const Player & player );


    virtual
    void serializePlayer( const Player & player );

    virtual
    void serializePlayerLook( const Player & player );

};


/*!
//===================================================================
//
//  CLASS: VisualSenderCoachV13
//
//  DESC: added kick state information.
//
//===================================================================
*/

class VisualSenderCoachV13
    : public VisualSenderCoachV8 {
public:

    VisualSenderCoachV13( const Params & params );

    virtual
    ~VisualSenderCoachV13();


protected:

    virtual
    void serializePlayer( const Player & player );

    virtual
    void serializePlayerLook( const Player & player );
};

}

#endif
