// -*-c++-*-

/***************************************************************************
                 socketstreambuf.hpp  -  A socket stream buffer
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


#ifndef RCSS_NET_SOCKETSTREAMBUF_HPP
#define RCSS_NET_SOCKETSTREAMBUF_HPP

#include <rcssbase/net/addr.hpp>

//g++ 2.95.6 doesn't have the streambuf header, so iostream is used instead
//#include <streambuf>
#include <iostream>

namespace rcss {
namespace net {

class Socket;

class SocketStreamBuf
    : public std::streambuf {
public:
//     typedef int int_type;
    typedef char char_type;

    enum ConnType { CONN_ON_READ, NO_CONN };

private:

    Socket & M_socket;
    Addr M_end_point;
    std::streamsize M_bufsize;
    char_type * M_inbuf;
    char_type * M_outbuf;
    int M_remained;
    char_type M_remained_char;
    ConnType M_connect;

    // not used
    SocketStreamBuf( const SocketStreamBuf & );
    // not used
    SocketStreamBuf & operator=( const SocketStreamBuf & );

public:
    SocketStreamBuf( Socket & socket,
                     const Addr & dest,
                     ConnType conn = CONN_ON_READ,
                     std::streamsize bufsize = 8192 );

    SocketStreamBuf( Socket & socket,
                     ConnType conn = NO_CONN,
                     std::streamsize bufsize = 8192 );

    virtual
    ~SocketStreamBuf();

    void setEndPoint( const Addr & addr )
      {
          M_end_point = addr;
      }

    void setConnectType( ConnType conn )
      {
          M_connect = conn;
      }

private:

    bool writeData();

protected:

    virtual
    int_type overflow( int_type c = traits_type::eof() );

    virtual
    int sync();

    virtual
    int_type underflow();

};

}
}

#endif
