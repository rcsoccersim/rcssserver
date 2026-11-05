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

#include "closeonexec.h"
#include "datagrambuf.h"
#include "udpconnection.h"
#include "param.h"

#include <algorithm>
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
    std::shared_ptr< boost::asio::ip::udp::socket > M_socket;
    boost::asio::ip::udp::endpoint M_dest;
    DatagramBuf M_outbuf; //!< must be declared after M_socket
    bool M_closed;

public:
    UDPReply( const std::shared_ptr< boost::asio::ip::udp::socket > & socket,
              const boost::asio::ip::udp::endpoint & dest )
        : M_socket( socket ),
          M_dest( dest ),
          // never connect the shared socket to one client
          M_outbuf( *socket, dest ),
          M_closed( false )
      { }

    const std::shared_ptr< boost::asio::ip::udp::socket > & socket() const
      {
          return M_socket;
      }

    const boost::asio::ip::udp::endpoint & dest() const
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
              && M_socket->is_open();
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
UDPAcceptor::UDPAcceptor( boost::asio::io_context & io_context )
    : M_io_context( io_context ),
      M_socket(),
      M_connections()
{

}

/*-------------------------------------------------------------------*/
UDPAcceptor::~UDPAcceptor()
{

}

/*-------------------------------------------------------------------*/
boost::system::error_code
UDPAcceptor::open( const unsigned short port )
{
    namespace ip = boost::asio::ip;

    boost::system::error_code error;

    M_socket = std::make_shared< ip::udp::socket >( M_io_context );
    M_socket->open( ip::udp::v4(), error );
    if ( ! error )
    {
        error = set_close_on_exec( M_socket->native_handle() );
    }
    if ( ! error )
    {
        M_socket->bind( ip::udp::endpoint( ip::udp::v4(), port ), error );
    }
    if ( ! error )
    {
        M_socket->non_blocking( true, error );
    }

    if ( error )
    {
        M_socket.reset();
    }
    return error;
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
    boost::asio::ip::udp::endpoint from;
    std::size_t len = 0;
    boost::system::error_code error;

    do
    {
        len = M_socket->receive_from( boost::asio::buffer( buffer, MaxMesg ), from, 0, error );
    }
    while ( error == boost::asio::error::interrupted
            || ( ! error && len == 0 ) ); // ignore empty datagrams

    if ( error )
    {
        if ( error != boost::asio::error::would_block
             && error != boost::asio::error::try_again )
        {
            std::cerr << __FILE__ << ": " << __LINE__
                      << ": Error recv'ing from socket: "
                      << error.message() << std::endl;
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

    std::shared_ptr< UDPConnection > conn = std::make_shared< UDPConnection >( M_io_context );
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
UDPAcceptor::findConnection( const boost::asio::ip::udp::endpoint & endpoint ) const
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
             && conn->getDest() == endpoint ) // getDest() is the default endpoint if closed
        {
            return conn.get();
        }
    }

    return nullptr;
}

}
