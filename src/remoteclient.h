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

#include "compress.h"
#include "transport/connection.h"

#include <memory>
#include <ostream>

namespace rcss {
namespace gz {
class gzstreambuf;
}
}


class RemoteClient {

private:
    std::shared_ptr< rcss::Connection > M_connection;
    rcss::gz::gzstreambuf * M_gz_buf;

    //! The senders keep a reference to this stream, so it lives as long as
    //! the client. Its buffer is switched when a connection is attached.
    std::ostream M_transport;
    int M_comp_level;

#ifdef HAVE_LIBZ
    //Compressor M_comp;
    Decompressor M_decomp;
#endif

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
    void processMsg( char * msg,
                     const size_t & len );

    virtual
    void parseMsg( char * msg,
                   const size_t & len ) = 0;

public:
    int compressionLevel() const
      {
          return M_comp_level;
      }

    int setCompressionLevel( const int level );

    /*!
      \brief check if the connection can carry compressed messages.
      \return false if no connection is attached or the transport reserves
      some bytes for the message framing
     */
    bool compressionAvailable() const
      {
          return M_connection
              && M_connection->isBinarySafe();
      }

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
