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

#include <rcssbase/net/udpsocket.hpp>

namespace rcss {
namespace net {
class SocketStreamBuf;
}
namespace gz {
class gzstreambuf;
}
}


class RemoteClient {

private:
    rcss::net::UDPSocket M_socket;
    rcss::net::SocketStreamBuf * M_socket_buf;
    rcss::gz::gzstreambuf * M_gz_buf;
    std::ostream * M_transport;
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

    void undedicatedRecv( char * msg,
                          const size_t & len );

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

    std::ostream & getTransport();

protected:
    void close();

public:
    bool connected() const
      {
          return M_socket.isConnected();
      }

    bool connect( const rcss::net::Addr & dest );

    int open();

    rcss::net::Addr getDest() const
      {
          return M_socket.getDest();
      }

};

#endif
