// -*-c++-*-

/***************************************************************************
                  isocketstream.hpp  -  An istream for sockets
                             -------------------
    begin                : 08-JAN-2003
    copyright            : (C) 2003 by The RoboCup Soccer Server
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

#ifndef RCSS_NET_ISOCKETSTREAM_HPP
#define RCSS_NET_ISOCKETSTREAM_HPP

#include "socketstreambuf.hpp"

namespace rcss {
namespace net {

class ISocketStream
    : public SocketStreamBuf,
      public std::istream {
public:
    ISocketStream( Socket & socket,
                   const Addr & dest,
                   ConnType conn = CONN_ON_READ,
                   int buffer_size = 8192 )
        : SocketStreamBuf( socket, dest, conn, buffer_size ),
          std::istream( this )
      {}

    ISocketStream( Socket & socket,
                   ConnType conn = NO_CONN,
                   int buffer_size = 8192 )
        : SocketStreamBuf( socket, conn, buffer_size ),
          std::istream( this )
      {}

private:
    // not for use
    ISocketStream(const ISocketStream & );
    ISocketStream & operator=( const ISocketStream & );
};

}
}

#endif
