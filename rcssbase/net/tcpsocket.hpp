// -*-c++-*-

/***************************************************************************
                          tcpsocket.hpp  -  A simple tcp socket class
                             -------------------
    begin                : 2003-11-11
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


#ifndef RCSS_NET_TCPSOCKET_HPP
#define RCSS_NET_TCPSOCKET_HPP


#include <rcssbase/net/socket.hpp>

namespace rcss {
namespace net {

class TCPSocket
    : public Socket {
public:

    TCPSocket();

    TCPSocket( SocketDesc & s );

    TCPSocket( const Addr & addr );

    TCPSocket( const Addr & addr,
               const Addr & dest );

    bool accept( TCPSocket & sock );

    bool listen( int backlog );

protected:
    virtual
    bool doOpen( SocketDesc& fd );
};

}
}

#endif
