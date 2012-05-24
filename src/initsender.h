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
 *   Foundation; either version 3 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/


#ifndef RCSS_INITSENDER_H
#define RCSS_INITSENDER_H

#include "sender.h"
#include "observer.h"
#include "serverparam.h"
#include "playerparam.h"

#include <boost/shared_ptr.hpp>

class Stadium;
class HeteroPlayer;

namespace rcss {

class Serializer;

/*!
//===================================================================
//
//  CLASS: InitSenderCommon
//
//  DESC: base sender for init protocol for all clients.
//
//===================================================================
*/

class InitSenderCommon {
private:
    std::ostream & M_transport;
    const boost::shared_ptr< Serializer > M_serializer;
    const Stadium & M_stadium;
    const unsigned int M_version;
    const bool M_new_line;

public:
    InitSenderCommon( std::ostream & transport,
                      const boost::shared_ptr< Serializer > serializer,
                      const Stadium & stadium,
                      unsigned int version,
                      const bool new_line = false )
        : M_transport( transport ),
          M_serializer( serializer ),
          M_stadium( stadium ),
          M_version( version ),
          M_new_line( new_line )
      { }

    virtual
    ~InitSenderCommon()
      { }

    std::ostream & transport()
      {
          return M_transport;
      }

    const Serializer & serializer()
      {
          return *M_serializer;
      }

    const Stadium & stadium()
      {
          return M_stadium;
      }

    unsigned int version()
      {
          return M_version;
      }

    bool newLine() const
      {
          return M_new_line;
      }

    virtual
    void sendServerParams() = 0;

    virtual
    void sendPlayerParams() = 0;

    virtual
    void sendPlayerTypes() = 0;
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
    : protected Sender {
private:
    boost::shared_ptr< InitSenderCommon > m_common_sender;

protected:

    InitSender( std::ostream & transport,
                const boost::shared_ptr< InitSenderCommon > & common );

public:
    virtual
    ~InitSender();

    InitSenderCommon & commonSender()
      {
          return *m_common_sender;
      }

    virtual
    void sendInit() = 0;

    virtual
    void sendServerParams()
      {
          commonSender().sendServerParams();
      }

    virtual
    void sendPlayerParams()
      {
          commonSender().sendPlayerParams();
      }

    virtual
    void sendPlayerTypes()
      {
          commonSender().sendPlayerTypes();
      }

    virtual
    void sendChangedPlayers() = 0;

    virtual
    void sendScore() = 0;
};


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
    InitSenderCommonV1( std::ostream & transport,
                        const boost::shared_ptr< Serializer > serializer,
                        const Stadium & stad,
                        unsigned int version,
                        const bool new_line = false )
        : InitSenderCommon( transport,
                            serializer,
                            stad,
                            version,
                            new_line )
      { }

    virtual
    ~InitSenderCommonV1()
      { }

    virtual
    void sendServerParams()
      { }

    virtual
    void sendPlayerParams()
      { }

    virtual
    void sendPlayerTypes()
      { }
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
    : public InitSenderCommonV1 {
public:
    InitSenderCommonV7( std::ostream & transport,
                        const boost::shared_ptr< Serializer > serializer,
                        const Stadium & stad,
                        unsigned int version,
                        const bool new_line = false )
        : InitSenderCommonV1( transport,
                              serializer,
                              stad,
                              version,
                              new_line )
      { }

    virtual
    ~InitSenderCommonV7()
      { }

    virtual
    void sendServerParams();

    virtual
    void sendPlayerParams();

    virtual
    void sendPlayerTypes();

protected:
    virtual
    void serializePlayerType( const int id,
                              const HeteroPlayer & type );

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
    : public InitSenderCommonV7 {
public:
    InitSenderCommonV8( std::ostream & transport,
                        const boost::shared_ptr< Serializer > serializer,
                        const Stadium & stad,
                        unsigned int version,
                        const bool new_line = false )
        : InitSenderCommonV7( transport,
                              serializer,
                              stad,
                              version,
                              new_line )
      { }

    virtual
    ~InitSenderCommonV8()
      { }

    virtual
    void sendServerParams();

    void sendServerParam( ServerParam::VerMap::value_type param )
      {
          doSendServerParam( param );
      }

    void sendPlayerParam( PlayerParam::VerMap::value_type param )
      {
          doSendPlayerParam( param );
      }

    virtual
    void sendPlayerParams();

    virtual
    void sendPlayerTypes();

protected:
    virtual
    void doSendServerParam( ServerParam::VerMap::value_type param );

    virtual
    void doSendPlayerParam( PlayerParam::VerMap::value_type param );

    virtual
    void serializePlayerType( const int id,
                              const HeteroPlayer & type );

};

}

#endif
