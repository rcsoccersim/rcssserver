// -*-c++-*-

/***************************************************************************
                  iosocketstream.hpp  -  An iostream for sockets
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

#ifndef RCSS_NET_IOSOCKETSTREAM_HPP
#define RCSS_NET_IOSOCKETSTREAM_HPP

#include "socketstreambuf.hpp"
#include "isocketstream.hpp"
#include "osocketstream.hpp"

namespace rcss {
namespace net {

class IOSocketStream
    : public SocketStreamBuf,
      public std::iostream {
    // The IOSocketStream can be used in threads, however if a read
    // blocks waiting for input, then the sending at the same time
    // will also block.  If you want to do this then you will need to
    // either protect the stream with a mutex or create a
    // ISocketStream for reading and a OSocketStream for
    // writing.  You can pass the same socket to both to have the
    // reading and writing performed by the same socket.
public:

    IOSocketStream( Socket & socket,
                    const Addr & dest,
                    ConnType conn = CONN_ON_READ,
                    int buffer_size = 8192 )
        : SocketStreamBuf( socket, dest, conn, buffer_size ),
          std::iostream( this )
      { }

    IOSocketStream( Socket & socket,
                    ConnType conn = NO_CONN,
                    int buffer_size = 8192 )
        : SocketStreamBuf( socket, conn, buffer_size ),
          std::iostream( this )
      { }

private:
    // not for use
    IOSocketStream( const IOSocketStream & );
    IOSocketStream & operator=( const IOSocketStream & );
};

}
}

#endif
