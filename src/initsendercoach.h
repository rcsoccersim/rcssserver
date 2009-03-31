// -*-c++-*-

/***************************************************************************
                                initsendercoach.h
                  Classes for sending sense init messages
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


#ifndef RCSS_INITSENDER_COACH_H
#define RCSS_INITSENDER_COACH_H

#include "initsender.h"

#include <rcssbase/factory.hpp>

#include <boost/shared_ptr.hpp>

class Coach;

namespace rcss {

class SerializerCoach;

/*!
//===================================================================
//
//  CLASS: InitSenderOfflineCoach
//
//  DESC: Base class for the init protocol for offline coaches.
//
//===================================================================
*/

class InitSenderOfflineCoach
    : public InitSender {
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

    typedef std::auto_ptr< rcss::InitSenderOfflineCoach > Ptr;
    typedef Ptr (*Creator)( const Params& );
    typedef rcss::Factory< Creator, int > FactoryHolder;

    static
    FactoryHolder & factory();

    virtual
    ~InitSenderOfflineCoach();

protected:
    InitSenderOfflineCoach( const Params & params,
                            const boost::shared_ptr< InitSenderCommon > common );

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

private:
    const boost::shared_ptr< SerializerCoach > M_serializer;

    /*:TODO: M_self needs to be replaced with a reference to a
      InitObserver and InitObserver should have virtual functions for
      stuff like velocity, stamina, etc */
    const Coach & M_self;
    const Stadium & M_stadium;
};



/*!
//===================================================================
//
//  CLASS: InitObserverOfflineCoach
//
//  DESC: Interface for an object that receives init information.
//
//===================================================================
*/

class InitObserverOfflineCoach
    : protected BaseObserver< InitSenderOfflineCoach > {
public:

    InitObserverOfflineCoach()
      { }

    InitObserverOfflineCoach( InitSenderOfflineCoach & sender )
        : BaseObserver< InitSenderOfflineCoach >( sender )
      { }

    InitObserverOfflineCoach( std::auto_ptr< InitSenderOfflineCoach > sender )
        : BaseObserver< InitSenderOfflineCoach >( sender )
      { }

    ~InitObserverOfflineCoach()
      { }

    void setInitSender( InitSenderOfflineCoach & sender )
      {
          BaseObserver< InitSenderOfflineCoach >::setSender( sender );
      }

    void setInitSender( std::auto_ptr< InitSenderOfflineCoach > sender )
      {
          BaseObserver< InitSenderOfflineCoach >::setSender( sender );
      }

    void sendInit()
      {
          BaseObserver< InitSenderOfflineCoach >::sender().sendInit();
      }

    void sendServerParams()
      {
          BaseObserver< InitSenderOfflineCoach >::sender().sendServerParams();
      }

    void sendPlayerParams()
      {
          BaseObserver< InitSenderOfflineCoach >::sender().sendPlayerParams();
      }

    void sendPlayerTypes()
      {
          BaseObserver< InitSenderOfflineCoach >::sender().sendPlayerTypes();
      }

    void sendChangedPlayers()
      {
          BaseObserver< InitSenderOfflineCoach >::sender().sendChangedPlayers();
      }

    void sendScore()
      {
          BaseObserver< InitSenderOfflineCoach >::sender().sendScore();
      }

};


/*!
//===================================================================
//
//  CLASS: InitSenderOfflineCoachV1
//
//  DESC: version 1 of the init protocol.
//
//===================================================================
*/

class InitSenderOfflineCoachV1
    : public InitSenderOfflineCoach {
public:
    InitSenderOfflineCoachV1( const Params & params );

protected:
    InitSenderOfflineCoachV1( const Params & params,
                              const boost::shared_ptr< InitSenderCommon > common );

public:
    virtual
    ~InitSenderOfflineCoachV1();

    virtual
    void sendInit();

    virtual
    void sendScore();

    virtual
    void sendChangedPlayers();
};

/*!
//===================================================================
//
//  CLASS: InitSenderOfflineCoachV7
//
//  DESC: version 7 of the init protocol.
//
//===================================================================
*/

class InitSenderOfflineCoachV7
    : public InitSenderOfflineCoachV1 {
public:
    InitSenderOfflineCoachV7( const Params & params );

protected:
    InitSenderOfflineCoachV7( const Params & params,
                              const boost::shared_ptr< InitSenderCommon > common );

public:
    virtual
    ~InitSenderOfflineCoachV7();

};

/*!
//===================================================================
//
//  CLASS: InitSenderOfflineCoachV8
//
//  DESC: version 8 of the init protocol.
//
//===================================================================
*/

class InitSenderOfflineCoachV8
    : public InitSenderOfflineCoachV7 {
public:
    InitSenderOfflineCoachV8( const Params& params );

protected:
    InitSenderOfflineCoachV8( const Params& params,
                              const boost::shared_ptr< InitSenderCommon > common );

public:
    virtual
    ~InitSenderOfflineCoachV8();

};

}

#endif
