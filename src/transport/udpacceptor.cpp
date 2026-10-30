// -*-c++-*-


/*!
  \file udpacceptor.cpp
  \brief The acceptor implementation over UDP
*/

/*
 *Copyright:

 Copyright (C) The RoboCup Soccer Simulator Maintenance Group.

 This code is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 3 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 *EndCopyright:
 */

/////////////////////////////////////////////////////////////////////

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "udpacceptor.h"

#include "udpconnection.h"
#include "param.h"

#include <rcss/net/socketstreambuf.hpp>

#include <algorithm>
#include <cerrno>
#include <cstring>
#include <iostream>

namespace rcss {

namespace {

/*!
  \class UDPReply
  \brief The channel to answer a sender that has not been accepted.

  Messages are sent from the shared socket to the sender's address.
 */
class UDPReply
    : public Connection {
private:
    std::shared_ptr< rcss::net::UDPSocket > M_socket;
    rcss::net::Addr M_dest;
    rcss::net::SocketStreamBuf M_outbuf; //!< must be declared after M_socket
    bool M_closed;

public:
    UDPReply( const std::shared_ptr< rcss::net::UDPSocket > & socket,
              const rcss::net::Addr & dest )
        : M_socket( socket ),
          M_dest( dest ),
          // NO_CONN: never connect the shared socket to one client
          M_outbuf( *socket, dest, rcss::net::SocketStreamBuf::NO_CONN ),
          M_closed( false )
      { }

    const std::shared_ptr< rcss::net::UDPSocket > & socket() const
      {
          return M_socket;
      }

    const rcss::net::Addr & dest() const
      {
          return M_dest;
      }

    std::streambuf & outbuf() override
      {
          return M_outbuf;
      }

    bool popMessage( std::string & ) override
      {
          // messages from the sender arrive at the acceptor
          return false;
      }

    bool isOpen() const override
      {
          return ! M_closed
              && M_socket->isOpen();
      }

    void close() override
      {
          // the shared socket must stay open for other clients
          M_closed = true;
      }

    bool isBinarySafe() const override
      {
          return true;
      }
};

}

/*-------------------------------------------------------------------*/
UDPAcceptor::UDPAcceptor()
    : M_socket(),
      M_connections()
{

}

/*-------------------------------------------------------------------*/
UDPAcceptor::~UDPAcceptor()
{

}

/*-------------------------------------------------------------------*/
bool
UDPAcceptor::open( const rcss::net::Addr & addr )
{
    // the constructor of UDPSocket opens a socket
    M_socket = std::make_shared< rcss::net::UDPSocket >();

    return M_socket->bind( addr )
        && M_socket->setNonBlocking() != -1;
}

/*-------------------------------------------------------------------*/
bool
UDPAcceptor::popInbound( Inbound & in )
{
    if ( ! M_socket )
    {
        return false;
    }

    char buffer[ MaxMesg ];
    rcss::net::Addr from;
    int len = 0;

    do
    {
        len = M_socket->recv( buffer, MaxMesg, from );
    }
    while ( len == 0 ); // ignore empty datagrams

    if ( len < 0 )
    {
        if ( errno != EWOULDBLOCK )
        {
            std::cerr << __FILE__ << ": " << __LINE__
                      << ": Error recv'ing from socket: "
                      << std::strerror( errno ) << std::endl;
        }
        return false;
    }

    in.message.assign( buffer, len );
    in.reply = std::make_shared< UDPReply >( M_socket, from );
    in.known = findConnection( from );
    return true;
}

/*-------------------------------------------------------------------*/
std::shared_ptr< Connection >
UDPAcceptor::accept( const Inbound & in )
{
    const UDPReply * reply = dynamic_cast< const UDPReply * >( in.reply.get() );
    if ( ! reply
         || reply->socket() != M_socket )
    {
        // not taken from this acceptor
        return std::shared_ptr< Connection >();
    }

    std::shared_ptr< UDPConnection > conn = std::make_shared< UDPConnection >();
    if ( ! conn->open()
         || ! conn->connect( reply->dest() ) )
    {
        return std::shared_ptr< Connection >();
    }

    // forget the connections that their clients have released
    M_connections.erase( std::remove_if( M_connections.begin(),
                                         M_connections.end(),
                                         []( const std::weak_ptr< UDPConnection > & c )
                                           {
                                               return c.expired();
                                           } ),
                         M_connections.end() );
    M_connections.push_back( conn );

    return conn;
}

/*-------------------------------------------------------------------*/
const UDPConnection *
UDPAcceptor::findConnection( const rcss::net::Addr & addr ) const
{
    // Search from the newest one. If an old connection to the same address
    // is still open, the newer client is the one registered in Stadium.
    for ( std::vector< std::weak_ptr< UDPConnection > >::const_reverse_iterator it = M_connections.rbegin(),
              end = M_connections.rend();
          it != end;
          ++it )
    {
        const std::shared_ptr< UDPConnection > conn = it->lock();
        if ( conn
             && conn->getDest() == addr ) // getDest() is the default Addr if closed
        {
            return conn.get();
        }
    }

    return nullptr;
}

}
