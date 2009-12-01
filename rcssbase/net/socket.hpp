// -*-c++-*-

/***************************************************************************
               socket.hpp  -  Base newtork socket class
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

#ifndef RCSS_NET_SOCKET_HPP
#define RCSS_NET_SOCKET_HPP

#include <boost/shared_ptr.hpp>

#include <rcssbase/net/addr.hpp>

namespace rcss {
namespace net {


class Socket {
public:
// #ifdef HAVE_SOCKET
//     typedef SOCKET SocketDesc;
// #else
    typedef int SocketDesc;
//#endif

    static const SocketDesc INVALIDSOCKET;

    enum CheckingType { CHECK, DONT_CHECK };

public:
    static
    void closeFD( SocketDesc * s );

    Socket();

    Socket( SocketDesc s );

    virtual
    ~Socket();

    bool open();

    bool bind( const Addr & addr );

    Addr getName() const;

    bool connect( const Addr & addr );

    Addr getPeer() const;

    void close();

    int setCloseOnExec( bool on = true );

    int setNonBlocking( bool on = true );

    int setAsync( bool on = true );

    int setBroadcast( bool on = true );

    SocketDesc getFD() const;

    bool isOpen() const;

    bool isConnected() const;

    Addr getDest() const; // deprecated.  Use getPeer instead.

    int send( const char * msg,
              size_t len,
              const Addr& dest,
              int flags = 0,
              CheckingType check = CHECK );

    int send( const char * msg,
              size_t len,
              int flags = 0,
              CheckingType check = CHECK );

    int recv( char * msg,
              size_t len,
              Addr& from,
              int flags = 0,
              CheckingType check = CHECK );

    int recv( char * msg,
              size_t len,
              int flags = 0,
              CheckingType check = CHECK );

    // The following two methods allow a timeout to be specified.
    // Overall, it's slower than the untimed varients so if you do
    // need to specify a timeout and you just want it the recv to
    // block or not to block, then you are better off setting the
    // socket to blocking or non-blocking and using the version
    // without timeouts.
    int recv( int timeout,
              char * msg,
              size_t len,
              Addr & from,
              int flags = 0 );

    int recv( int timeout,
              char * msg,
              size_t len,
              int flags = 0 );

protected:
    virtual
    bool doOpen( SocketDesc& fd ) = 0;

private:
    boost::shared_ptr< SocketDesc > M_handle;
};

}
}

#endif
