// -*-c++-*-


/*!
  \file tcpacceptor.cpp
  \brief The acceptor implementation over TCP
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

#include "tcpacceptor.h"

#include "tcpconnection.h"

#include <boost/asio/ip/v6_only.hpp>

#include <algorithm>
#include <iostream>

namespace rcss {

namespace {

//! A connection is closed if its client is not accepted within this time.
const std::chrono::seconds PENDING_TIMEOUT( 10 );

//! The limit of the connections whose clients have not been accepted.
const std::size_t MAX_PENDING = 64;

}

/*-------------------------------------------------------------------*/
TCPAcceptor::TCPAcceptor( boost::asio::io_context & io_context )
    : M_acceptor( io_context ),
      M_pending()
{

}

/*-------------------------------------------------------------------*/
TCPAcceptor::~TCPAcceptor()
{
    boost::system::error_code error;
    M_acceptor.close( error );

    for ( Pending & p : M_pending )
    {
        p.connection->close();
    }
}

/*-------------------------------------------------------------------*/
boost::system::error_code
TCPAcceptor::open( const unsigned short port )
{
    namespace ip = boost::asio::ip;

    boost::system::error_code error = listen( ip::tcp::endpoint( ip::tcp::v6(), port ), true );
    if ( error )
    {
        error = listen( ip::tcp::endpoint( ip::tcp::v4(), port ), false );
        if ( error )
        {
            return error;
        }
    }

    startAccept();
    return error;
}

/*-------------------------------------------------------------------*/
boost::system::error_code
TCPAcceptor::listen( const boost::asio::ip::tcp::endpoint & endpoint,
                     const bool dual_stack )
{
    boost::system::error_code error;
    boost::system::error_code ignored;

    M_acceptor.close( ignored );

    M_acceptor.open( endpoint.protocol(), error );
    if ( ! error
         && dual_stack )
    {
        // also accept IPv4 clients through IPv4-mapped addresses
        M_acceptor.set_option( boost::asio::ip::v6_only( false ), error );
    }
    if ( ! error )
    {
        // allow restarting the server while old connections are in TIME_WAIT
        M_acceptor.set_option( boost::asio::ip::tcp::acceptor::reuse_address( true ), error );
    }
    if ( ! error )
    {
        M_acceptor.bind( endpoint, error );
    }
    if ( ! error )
    {
        M_acceptor.listen( boost::asio::socket_base::max_listen_connections, error );
    }

    if ( error )
    {
        M_acceptor.close( ignored );
    }
    return error;
}

/*-------------------------------------------------------------------*/
void
TCPAcceptor::startAccept()
{
    M_acceptor.async_accept( [this]( const boost::system::error_code & error,
                                     boost::asio::ip::tcp::socket socket )
                             {
                                 handleAccept( error, std::move( socket ) );
                             } );
}

/*-------------------------------------------------------------------*/
void
TCPAcceptor::handleAccept( const boost::system::error_code & error,
                           boost::asio::ip::tcp::socket socket )
{
    if ( error == boost::asio::error::operation_aborted )
    {
        return;
    }

    if ( error )
    {
        // e.g. too many open files. Keep accepting the other clients.
        std::cerr << "TCP accept failed: " << error.message() << std::endl;
    }
    else if ( M_pending.size() >= MAX_PENDING )
    {
        std::cerr << "TCP connection refused: too many connections are waiting." << std::endl;
        boost::system::error_code ignored;
        socket.close( ignored );
    }
    else
    {
        Pending p;
        p.connection = std::make_shared< TCPConnection >( std::move( socket ) );
        p.since = std::chrono::steady_clock::now();
        p.connection->start();
        M_pending.push_back( p );
    }

    startAccept();
}

/*-------------------------------------------------------------------*/
bool
TCPAcceptor::popInbound( Inbound & in )
{
    const std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

    // forget the connections that are closed or have waited too long
    M_pending.erase( std::remove_if( M_pending.begin(),
                                     M_pending.end(),
                                     [&now]( const Pending & p )
                                       {
                                           if ( now - p.since > PENDING_TIMEOUT )
                                           {
                                               p.connection->close();
                                           }
                                           return ! p.connection->isOpen();
                                       } ),
                     M_pending.end() );

    for ( Pending & p : M_pending )
    {
        std::string msg;
        if ( p.connection->popMessage( msg ) )
        {
            in.message = std::move( msg );
            in.reply = p.connection;
            in.known = nullptr;
            return true;
        }
    }

    return false;
}

/*-------------------------------------------------------------------*/
std::shared_ptr< Connection >
TCPAcceptor::accept( const Inbound & in )
{
    std::vector< Pending >::iterator it
        = std::find_if( M_pending.begin(),
                        M_pending.end(),
                        [&in]( const Pending & p )
                          {
                              return p.connection == in.reply;
                          } );
    if ( it == M_pending.end() )
    {
        // not taken from this acceptor, or already closed
        return std::shared_ptr< Connection >();
    }

    std::shared_ptr< Connection > connection = it->connection;
    M_pending.erase( it );
    return connection;
}

}
