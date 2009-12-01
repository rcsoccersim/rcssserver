// -*-c++-*-

/***************************************************************************
                  osocketstream.hpp  -  An ostream for sockets
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

#ifndef RCSS_NET_OSOCKETSTREAM_HPP
#define RCSS_NET_OSOCKETSTREAM_HPP

#include <rcssbase/net/socketstreambuf.hpp>

namespace rcss {
namespace net {

class OSocketStream
    : public SocketStreamBuf,
      public std::ostream {
public:

    OSocketStream( Socket & socket,
                   const Addr & dest,
                   int buffer_size = 8192 )
        : SocketStreamBuf( socket, dest, NO_CONN, buffer_size ),
          std::ostream( this )
      { }

    OSocketStream( Socket & socket,
                   int buffer_size = 8192 )
        : SocketStreamBuf( socket, NO_CONN, buffer_size ),
          std::ostream( this )
      { }

private:
    // not for use
    OSocketStream( const OSocketStream & );
    OSocketStream & operator=( const OSocketStream & );
};

}
}

#endif
