// -*-c++-*-

/***************************************************************************
                          remoteclient.h  -  A class for remote client
                             -------------------
    begin                : 22-NOV-2001
    copyright            : (C) 2001 by The RoboCup Soccer Server
                           Maintainance Group.
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


#ifndef RCSS_REMOTECLIENT_H
#define RCSS_REMOTECLIENT_H

#include "transport/connection.h"

#include <memory>
#include <ostream>


class RemoteClient {

private:
    std::shared_ptr< rcss::Connection > M_connection;

    //! The senders keep a reference to this stream, so it lives as long as
    //! the client. Its buffer is switched when a connection is attached.
    std::ostream M_transport;

    bool M_enforce_dedicated_port;

public:
    RemoteClient();

    virtual
    ~RemoteClient();

    bool setEnforceDedicatedPort( const bool enf = true )
      {
          return M_enforce_dedicated_port = enf;
      }

    int send( const char * msg,
              const size_t & len );

    int recv();

    /*!
      \brief process a message that the client sent to the shared port.
      \param msg the message
     */
    void undedicatedRecv( const std::string & msg );

private:
    void processMsg( const std::string & msg );

protected:
    virtual
    void parseMsg( char * msg,
                   const size_t & len ) = 0;

public:
    std::ostream & getTransport();

protected:
    void close();

public:
    bool connected() const
      {
          return M_connection
              && M_connection->isOpen();
      }

    /*!
      \brief start talking to the client over the given connection.
      \param connection the connection to the client

      The previous connection, if any, is closed first.
     */
    void attach( std::shared_ptr< rcss::Connection > connection );

    const rcss::Connection * connection() const
      {
          return M_connection.get();
      }

};

#endif
