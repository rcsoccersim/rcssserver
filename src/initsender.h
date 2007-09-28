// -*-c++-*-

/***************************************************************************
                                initsender.h
                  Classes for sending sense init messages
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


#ifndef RCSSINITSENDER_H
#define RCSSINITSENDER_H

//#include "paramreader.h"
#include "sender.h"
#include "observer.h"
#include <rcssbase/lib/factory.hpp>
#include "serverparam.h"
#include "playerparam.h"

class Stadium;
class Player;
class Coach;
class OnlineCoach;

namespace rcss
{
class Serializer;
class SerializerPlayer;
class SerializerCommon;
class SerializerOnlineCoach;
class SerializerCoach;


/*!
//===================================================================
//
//  CLASS: InitSenderCommon
//
//  DESC: base sender for init protocol for all clients.
//
//===================================================================
*/

class InitSenderCommon
{
public:
    InitSenderCommon( std::ostream& transport,
                      const Serializer& serializer,
                      const Stadium& stad,
                      unsigned int version )
        : m_transport( transport ),
          m_serializer( serializer ),
          m_stad( stad ),
          m_ver( version )
      {}

    virtual
    ~InitSenderCommon()
      {}

    virtual
    void
    sendServerParams() = 0;

    virtual
    void
    sendPlayerParams() = 0;

    virtual
    void
    sendPlayerTypes() = 0;

    std::ostream&
    transport()
      { return m_transport; }

    const Serializer&
    serializer()
      { return m_serializer; }

    const Stadium&
    stadium()
      { return m_stad; }

    unsigned int
    version()
      { return m_ver; }

private:
    std::ostream& m_transport;
    const Serializer& m_serializer;
    const Stadium& m_stad;
    unsigned int m_ver;
};

/*!
//===================================================================
//
//  CLASS: InitSender
//
//  DESC: Base class for the init protocol.
//
//===================================================================
*/

class InitSender
    : protected Sender
{
protected:

    InitSender( std::ostream& transport,
                const boost::shared_ptr< InitSenderCommon >& common );

public:
    virtual
    ~InitSender();

    virtual
    void
    sendInit() = 0;

    virtual
    void
    sendReconnect() = 0;

    virtual
    void
    sendServerParams()
      { commonSender().sendServerParams(); }

    virtual
    void
    sendPlayerParams()
      { commonSender().sendPlayerParams(); }

    virtual
    void
    sendPlayerTypes()
      { commonSender().sendPlayerTypes(); }

    virtual
    void
    sendChangedPlayers() = 0;

    virtual
    void
    sendScore() = 0;

    InitSenderCommon&
    commonSender()
      { return *m_common_sender; }

private:
    boost::shared_ptr< InitSenderCommon > m_common_sender;
};

/*!
//===================================================================
//
//  CLASS: InitObserver
//
//  DESC: Interface for an object that receives init information.
//
//===================================================================
*/

// template< typename S >
// class InitObserver
//     : protected BaseObserver< S >
// {
// public:
//     typedef S InitSender;

//     InitObserver()
//       {}

//     InitObserver( InitSender& sender )
//         : BaseObserver< InitSender >( sender )
//       {}

//     InitObserver( std::auto_ptr< InitSender > sender )
//         : BaseObserver< InitSender >( sender )
//       {}

//     ~InitObserver()
//       {}

//     void
//     setInitSender( InitSender& sender )
//       {
//           BaseObserver< InitSender >::setSender( sender );
//       }

//     void
//     setInitSender( std::auto_ptr< InitSender > sender )
//       {
//           BaseObserver< InitSender >::setSender( sender );
//       }

//     void
//     sendInit()
//       { BaseObserver< InitSender >::sender().sendInit(); }

//     void
//     sendReconnect()
//       { BaseObserver< InitSender >::sender().sendReconnect(); }

//     void
//     sendServerParams()
//       { BaseObserver< InitSender >::sender().sendServerParams(); }

//     void
//     sendPlayerParams()
//       { BaseObserver< InitSender >::sender().sendPlayerParams(); }

//     void
//     sendPlayerTypes()
//       { BaseObserver< InitSender >::sender().sendPlayerTypes(); }

//     void
//     sendChangedPlayers()
//       { BaseObserver< InitSender >::sender().sendChangedPlayers(); }

//     void
//     sendScore()
//       { BaseObserver< InitSender >::sender().sendScore(); }

// };


/*!
//===================================================================
//
//  CLASS: InitSenderCommonV1
//
//  DESC: version 1 of the init protocol for all clients.
//
//===================================================================
*/

class InitSenderCommonV1
    : public InitSenderCommon
{
public:
    InitSenderCommonV1( std::ostream& transport,
                        const Serializer& serializer,
                        const Stadium& stad,
                        unsigned int version )
        : InitSenderCommon( transport, serializer, stad,
                            version )
      {}

    virtual
    ~InitSenderCommonV1()
      {}

    virtual
    void
    sendServerParams() {}

    virtual
    void
    sendPlayerParams() {}

    virtual
    void
    sendPlayerTypes() {}
};


/*!
//===================================================================
//
//  CLASS: InitSenderCommonV7
//
//  DESC: version 7 of the init protocol for all clients.
//
//===================================================================
*/

class InitSenderCommonV7
    : public InitSenderCommonV1
{
public:
    InitSenderCommonV7( std::ostream& transport,
                        const Serializer& serializer,
                        const Stadium& stad,
                        unsigned int version )
        : InitSenderCommonV1( transport, serializer, stad, version )
      {}

    virtual
    ~InitSenderCommonV7()
      {}

    virtual
    void
    sendServerParams();

    virtual
    void
    sendPlayerParams();

    virtual
    void
    sendPlayerTypes();
};

/*!
//===================================================================
//
//  CLASS: InitSenderCommonV8
//
//  DESC: version 8 of the init protocol for all clients.
//
//===================================================================
*/

class InitSenderCommonV8
    : public InitSenderCommonV7
{
protected:
    class LocalSerializer
    {
    private:
        const Serializer& M_ser;
        std::ostream& M_out;
        unsigned int M_version;
    public:
        LocalSerializer( const Serializer& ser,
                         std::ostream& out,
                         unsigned int version )
            : M_ser( ser ),
              M_out( out ),
              M_version( version )
          {}

    };

public:
    InitSenderCommonV8( std::ostream& transport,
                        const Serializer& serializer,
                        const Stadium& stad,
                        unsigned int version )
        : InitSenderCommonV7( transport, serializer, stad, version )
      {}

    virtual
    ~InitSenderCommonV8()
      {}

    virtual
    void
    sendServerParams();

    void
    sendServerParam( ServerParam::VerMap::value_type param )
      {
          doSendServerParam( param );
      }

    virtual
    void
    doSendServerParam( ServerParam::VerMap::value_type param );

    void
    sendPlayerParam( PlayerParam::VerMap::value_type param )
      {
          doSendPlayerParam( param );
      }

    virtual
    void
    doSendPlayerParam( PlayerParam::VerMap::value_type param );

    virtual
    void
    sendPlayerParams();

    virtual
    void
    sendPlayerTypes();

};


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
    : public InitSender
{
public:
    typedef InitSenderPlayer Base;

    class Params
    {
    public:
        std::ostream& m_transport;
        const Player& m_self;
        const Stadium& m_stadium;
        const SerializerPlayer& m_ser;

        Params( std::ostream& transport,
                const Player& self,
                const Stadium& stadium,
                const SerializerPlayer& ser )
            : m_transport( transport ),
              m_self( self ),
              m_stadium( stadium ),
              m_ser( ser )
          {}
    };

public:
    typedef std::auto_ptr< InitSenderPlayer > Ptr;
    typedef Ptr(*Creator)( const Params& );
    typedef rcss::lib::Factory< Creator, int > Factory;

    static
    Factory&
    factory();

    virtual
    ~InitSenderPlayer();

protected:
    InitSenderPlayer( const Params& params,
                      const boost::shared_ptr< InitSenderCommon > common );

    const SerializerPlayer&
    serializer() const
      { return M_serializer; }

    const Player&
    self() const
      { return M_self; }

    const Stadium&
    stadium() const
      { return M_stadium; }

private:
    const SerializerPlayer& M_serializer;

    /*:TODO: M_self needs to be replaced with a reference to a
      InitObserver and InitObserver should have virtual functions for
      stuff like velocity, stamina, etc */
    const Player& M_self;
    const Stadium& M_stadium;
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
    : protected BaseObserver< InitSenderPlayer >
{
public:

    InitObserverPlayer()
      {}

    InitObserverPlayer( InitSenderPlayer & sender )
        : BaseObserver< InitSenderPlayer >( sender )
      {}

    InitObserverPlayer( std::auto_ptr< InitSenderPlayer > sender )
        : BaseObserver< InitSenderPlayer >( sender )
      {}

    ~InitObserverPlayer()
      {}

    void
    setInitSender( InitSenderPlayer & sender )
      {
          BaseObserver< InitSenderPlayer >::setSender( sender );
      }

    void
    setInitSender( std::auto_ptr< InitSenderPlayer > sender )
      {
          BaseObserver< InitSenderPlayer >::setSender( sender );
      }

    void
    sendInit()
      {
          BaseObserver< InitSenderPlayer >::sender().sendInit();
      }

    void
    sendReconnect()
      {
          BaseObserver< InitSenderPlayer >::sender().sendReconnect();
      }

    void
    sendServerParams()
      {
          BaseObserver< InitSenderPlayer >::sender().sendServerParams();
      }

    void
    sendPlayerParams()
      {
          BaseObserver< InitSenderPlayer >::sender().sendPlayerParams();
      }

    void
    sendPlayerTypes()
      {
          BaseObserver< InitSenderPlayer >::sender().sendPlayerTypes();
      }

    void
    sendChangedPlayers()
      {
          BaseObserver< InitSenderPlayer >::sender().sendChangedPlayers();
      }

    void
    sendScore()
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
    : public InitSenderPlayer
{
public:
    InitSenderPlayerV1( const Params& params );

protected:
    InitSenderPlayerV1( const Params& params,
                        const boost::shared_ptr< InitSenderCommon > common );

public:
    virtual
    ~InitSenderPlayerV1();

    virtual
    void
    sendInit();

    virtual
    void
    sendReconnect();

    virtual
    void
    sendScore();

    virtual
    void
    sendChangedPlayers();
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
    : public InitSenderPlayerV1
{
public:
    InitSenderPlayerV7( const Params& params );

protected:
    InitSenderPlayerV7( const Params& params,
                        const boost::shared_ptr< InitSenderCommon > common );

public:
    virtual
    ~InitSenderPlayerV7();

    virtual
    void
    sendChangedPlayers();
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
    : public InitSenderPlayerV7
{
public:
    InitSenderPlayerV8( const Params& params );

protected:
    InitSenderPlayerV8( const Params& params,
                        const boost::shared_ptr< InitSenderCommon > common );

public:
    virtual
    ~InitSenderPlayerV8();
};



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
    : public InitSender
{
public:
    typedef InitSenderOnlineCoach Base;

    class Params
    {
    public:
        std::ostream& m_transport;
        const OnlineCoach& m_self;
        const Stadium& m_stadium;
        const SerializerOnlineCoach& m_ser;

        Params( std::ostream& transport,
                const OnlineCoach& self,
                const Stadium& stadium,
                const SerializerOnlineCoach& ser )
            : m_transport( transport ),
              m_self( self ),
              m_stadium( stadium ),
              m_ser( ser )
          {}
    };

public:
    typedef std::auto_ptr< rcss::InitSenderOnlineCoach > Ptr;
    typedef Ptr(*Creator)( const rcss::InitSenderOnlineCoach::Params& );
    typedef rcss::lib::Factory< Creator, int > Factory;

    static
    Factory&
    factory();

    InitSenderOnlineCoach( const Params& params,
                           const boost::shared_ptr< InitSenderCommon > common );

    virtual
    ~InitSenderOnlineCoach();

protected:
    const
    SerializerOnlineCoach & serializer() const
      {
          return M_serializer;
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
    const SerializerOnlineCoach & M_serializer;

    /*:TODO: M_self needs to be replaced with a reference to a
      InitObserver and InitObserver should have virtual functions for
      stuff like velocity, stamina, etc */
    const OnlineCoach& M_self;
    const Stadium& M_stadium;
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
    : protected BaseObserver< InitSenderOnlineCoach >
{
public:

    InitObserverOnlineCoach()
      {}

    InitObserverOnlineCoach( InitSenderOnlineCoach & sender )
        : BaseObserver< InitSenderOnlineCoach >( sender )
      {}

    InitObserverOnlineCoach( std::auto_ptr< InitSenderOnlineCoach > sender )
        : BaseObserver< InitSenderOnlineCoach >( sender )
      {}

    ~InitObserverOnlineCoach()
      {}

    void
    setInitSender( InitSenderOnlineCoach & sender )
      {
          BaseObserver< InitSenderOnlineCoach >::setSender( sender );
      }

    void
    setInitSender( std::auto_ptr< InitSenderOnlineCoach > sender )
      {
          BaseObserver< InitSenderOnlineCoach >::setSender( sender );
      }

    void
    sendInit()
      {
          BaseObserver< InitSenderOnlineCoach >::sender().sendInit();
      }

    void
    sendReconnect()
      {
          BaseObserver< InitSenderOnlineCoach >::sender().sendReconnect();
      }

    void
    sendServerParams()
      {
          BaseObserver< InitSenderOnlineCoach >::sender().sendServerParams();
      }

    void
    sendPlayerParams()
      {
          BaseObserver< InitSenderOnlineCoach >::sender().sendPlayerParams();
      }

    void
    sendPlayerTypes()
      {
          BaseObserver< InitSenderOnlineCoach >::sender().sendPlayerTypes();
      }

    void
    sendChangedPlayers()
      {
          BaseObserver< InitSenderOnlineCoach >::sender().sendChangedPlayers();
      }

    void
    sendScore()
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
    : public InitSenderOnlineCoach
{
public:
    InitSenderOnlineCoachV1( const Params& params );

protected:
    InitSenderOnlineCoachV1( const Params& params,
                             const boost::shared_ptr< InitSenderCommon > common );

public:
    virtual
    ~InitSenderOnlineCoachV1();

    virtual
    void
    sendInit();

    virtual
    void
    sendReconnect();

    virtual
    void
    sendScore();

    virtual
    void
    sendChangedPlayers();

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
    : public InitSenderOnlineCoachV1
{
public:
    InitSenderOnlineCoachV6( const Params& params );

protected:
    InitSenderOnlineCoachV6( const Params& params,
                             const boost::shared_ptr< InitSenderCommon > common );

public:
    virtual
    ~InitSenderOnlineCoachV6();

    virtual
    void
    sendInit();
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
    : public InitSenderOnlineCoachV6
{
public:
    InitSenderOnlineCoachV7( const Params& params );

protected:
    InitSenderOnlineCoachV7( const Params& params,
                             const boost::shared_ptr< InitSenderCommon > common );

public:
    virtual
    ~InitSenderOnlineCoachV7();

    virtual
    void
    sendChangedPlayers();
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
    : public InitSenderOnlineCoachV7
{
public:
    InitSenderOnlineCoachV8( const Params& params );

protected:
    InitSenderOnlineCoachV8( const Params& params,
                             const boost::shared_ptr< InitSenderCommon > common );

public:
    virtual
    ~InitSenderOnlineCoachV8();

    virtual
    void sendPlayerClangVer( const Player & player );
};


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
    : public InitSender
{
public:
    typedef InitSenderOfflineCoach Base;

    class Params
    {
    public:
        std::ostream& m_transport;
        const Coach& m_self;
        const Stadium& m_stadium;
        const SerializerCoach& m_ser;

        Params( std::ostream& transport,
                const Coach& self,
                const Stadium& stadium,
                const SerializerCoach& ser )
            : m_transport( transport ),
              m_self( self ),
              m_stadium( stadium ),
              m_ser( ser )
          {}
    };

    typedef std::auto_ptr< rcss::InitSenderOfflineCoach > Ptr;
    typedef Ptr(*Creator)( const rcss::InitSenderOfflineCoach::Params& );
    typedef rcss::lib::Factory< Creator, int > Factory;

    static
    Factory&
    factory();

public:
    InitSenderOfflineCoach( const Params& params,
                            const boost::shared_ptr< InitSenderCommon > common );
public:
    virtual
    ~InitSenderOfflineCoach();

protected:
    const SerializerCoach&
    serializer() const
      { return M_serializer; }

    const Coach&
    self() const
      { return M_self; }

    const Stadium&
    stadium() const
      { return M_stadium; }

private:
    const SerializerCoach& M_serializer;

    /*:TODO: M_self needs to be replaced with a reference to a
      InitObserver and InitObserver should have virtual functions for
      stuff like velocity, stamina, etc */
    const Coach& M_self;
    const Stadium& M_stadium;
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
    : protected BaseObserver< InitSenderOfflineCoach >
{
public:

    InitObserverOfflineCoach()
      {}

    InitObserverOfflineCoach( InitSenderOfflineCoach & sender )
        : BaseObserver< InitSenderOfflineCoach >( sender )
      {}

    InitObserverOfflineCoach( std::auto_ptr< InitSenderOfflineCoach > sender )
        : BaseObserver< InitSenderOfflineCoach >( sender )
      {}

    ~InitObserverOfflineCoach()
      {}

    void
    setInitSender( InitSenderOfflineCoach & sender )
      {
          BaseObserver< InitSenderOfflineCoach >::setSender( sender );
      }

    void
    setInitSender( std::auto_ptr< InitSenderOfflineCoach > sender )
      {
          BaseObserver< InitSenderOfflineCoach >::setSender( sender );
      }

    void
    sendInit()
      {
          BaseObserver< InitSenderOfflineCoach >::sender().sendInit();
      }

    void
    sendReconnect()
      {
          BaseObserver< InitSenderOfflineCoach >::sender().sendReconnect();
      }

    void
    sendServerParams()
      {
          BaseObserver< InitSenderOfflineCoach >::sender().sendServerParams();
      }

    void
    sendPlayerParams()
      {
          BaseObserver< InitSenderOfflineCoach >::sender().sendPlayerParams();
      }

    void
    sendPlayerTypes()
      {
          BaseObserver< InitSenderOfflineCoach >::sender().sendPlayerTypes();
      }

    void
    sendChangedPlayers()
      {
          BaseObserver< InitSenderOfflineCoach >::sender().sendChangedPlayers();
      }

    void
    sendScore()
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
    : public InitSenderOfflineCoach
{
public:
    InitSenderOfflineCoachV1( const Params& params );

protected:
    InitSenderOfflineCoachV1( const Params& params,
                              const boost::shared_ptr< InitSenderCommon > common );

public:
    virtual
    ~InitSenderOfflineCoachV1();

    virtual
    void
    sendInit();

    virtual
    void
    sendReconnect();

    virtual
    void
    sendScore();

    virtual
    void
    sendChangedPlayers();
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
    : public InitSenderOfflineCoachV1
{
public:
    InitSenderOfflineCoachV7( const Params& params );

protected:
    InitSenderOfflineCoachV7( const Params& params,
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
    : public InitSenderOfflineCoachV7
{
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
