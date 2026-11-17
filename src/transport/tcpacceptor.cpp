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

#include "closeonexec.h"
#include "tcpconnection.h"
#include "tcplisten.h"

#include <iostream>

namespace rcss {

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
    M_pending.closeAll();
}

/*-------------------------------------------------------------------*/
boost::system::error_code
TCPAcceptor::open( const unsigned short port )
{
    const boost::system::error_code error = listen_tcp( M_acceptor, port );
    if ( ! error )
    {
        startAccept();
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
    else if ( M_pending.size() >= PendingConnections::MAX_SIZE )
    {
        std::cerr << "TCP connection refused: too many connections are waiting." << std::endl;
        boost::system::error_code ignored;
        socket.close( ignored );
    }
    else
    {
        // The flag cannot fail to be set on an accepted socket in practice.
        set_close_on_exec( socket.native_handle() );
        std::shared_ptr< TCPConnection > connection = std::make_shared< TCPConnection >( std::move( socket ) );
        connection->start();
        M_pending.add( connection, std::chrono::steady_clock::now() );
    }

    startAccept();
}

/*-------------------------------------------------------------------*/
bool
TCPAcceptor::popInbound( Inbound & in )
{
    return M_pending.popInbound( in );
}

/*-------------------------------------------------------------------*/
std::shared_ptr< Connection >
TCPAcceptor::accept( const Inbound & in )
{
    return M_pending.accept( in );
}

}
