// -*-c++-*-


/*!
  \file wsserver.cpp
  \brief The WebSocket port shared by the services
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

#include "wsserver.h"

#include "closeonexec.h"
#include "tcplisten.h"
#include "wsconnection.h"

#include <algorithm>
#include <iostream>
#include <set>

namespace rcss {

/*-------------------------------------------------------------------*/
WSServer::WSServer( boost::asio::io_context & io_context,
                    const std::vector< std::string > & paths,
                    const std::vector< std::string > & allowed_origins )
    : M_acceptor( io_context ),
      M_checker(),
      M_handshakes(),
      M_pending()
{
    for ( const std::string & path : paths )
    {
        M_pending[path];
    }

    // The checker runs in the handlers of the connections, so it keeps its
    // own copies of the settings.
    const std::set< std::string > path_set( paths.begin(), paths.end() );
    const std::vector< std::string > origins = allowed_origins;
    M_checker = [path_set, origins]( const std::string & path,
                                     const std::string & origin )
        {
            if ( path_set.find( path ) == path_set.end() )
            {
                return boost::beast::http::status::not_found;
            }

            if ( ! origin.empty()
                 && ! origins.empty()
                 && std::find( origins.begin(), origins.end(), origin ) == origins.end() )
            {
                return boost::beast::http::status::forbidden;
            }

            return boost::beast::http::status::switching_protocols;
        };
}

/*-------------------------------------------------------------------*/
WSServer::~WSServer()
{
    boost::system::error_code error;
    M_acceptor.close( error );

    for ( Handshake & h : M_handshakes )
    {
        h.connection->close();
    }
}

/*-------------------------------------------------------------------*/
boost::system::error_code
WSServer::open( const unsigned short port )
{
    const boost::system::error_code error = listen_tcp( M_acceptor, port );
    if ( ! error )
    {
        startAccept();
    }
    return error;
}

/*-------------------------------------------------------------------*/
bool
WSServer::popInbound( const std::string & path,
                      Inbound & in )
{
    checkHandshakes();

    std::map< std::string, PendingConnections >::iterator it = M_pending.find( path );
    if ( it == M_pending.end() )
    {
        return false;
    }

    return it->second.popInbound( in );
}

/*-------------------------------------------------------------------*/
std::shared_ptr< Connection >
WSServer::accept( const std::string & path,
                  const Inbound & in )
{
    std::map< std::string, PendingConnections >::iterator it = M_pending.find( path );
    if ( it == M_pending.end() )
    {
        return std::shared_ptr< Connection >();
    }

    return it->second.accept( in );
}

/*-------------------------------------------------------------------*/
void
WSServer::startAccept()
{
    std::shared_ptr< WSServer > self = shared_from_this();
    M_acceptor.async_accept( [self]( const boost::system::error_code & error,
                                     boost::asio::ip::tcp::socket socket )
                             {
                                 self->handleAccept( error, std::move( socket ) );
                             } );
}

/*-------------------------------------------------------------------*/
void
WSServer::handleAccept( const boost::system::error_code & error,
                        boost::asio::ip::tcp::socket socket )
{
    if ( error == boost::asio::error::operation_aborted )
    {
        return;
    }

    if ( error )
    {
        // e.g. too many open files. Keep accepting the other clients.
        std::cerr << "WebSocket accept failed: " << error.message() << std::endl;
    }
    else if ( waitingConnections() >= PendingConnections::MAX_SIZE )
    {
        std::cerr << "WebSocket connection refused: too many connections are waiting." << std::endl;
        boost::system::error_code ignored;
        socket.close( ignored );
    }
    else
    {
        // The flag cannot fail to be set on an accepted socket in practice.
        set_close_on_exec( socket.native_handle() );
        Handshake h;
        h.connection = std::make_shared< WSConnection >( std::move( socket ) );
        h.since = std::chrono::steady_clock::now();
        h.connection->start( M_checker );
        M_handshakes.push_back( h );
    }

    startAccept();
}

/*-------------------------------------------------------------------*/
void
WSServer::checkHandshakes()
{
    const std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

    std::vector< Handshake >::iterator it = M_handshakes.begin();
    while ( it != M_handshakes.end() )
    {
        if ( ! it->connection->isOpen() )
        {
            // refused or failed
            it = M_handshakes.erase( it );
        }
        else if ( it->connection->isOpened() )
        {
            // The checker accepts only the known paths. The time limit for
            // the client to be accepted counts from the connection.
            M_pending[it->connection->path()].add( it->connection, it->since );
            it = M_handshakes.erase( it );
        }
        else if ( now - it->since > PendingConnections::TIMEOUT )
        {
            it->connection->close();
            it = M_handshakes.erase( it );
        }
        else
        {
            ++it;
        }
    }
}

/*-------------------------------------------------------------------*/
std::size_t
WSServer::waitingConnections() const
{
    std::size_t count = M_handshakes.size();
    for ( const std::map< std::string, PendingConnections >::value_type & v : M_pending )
    {
        count += v.second.size();
    }
    return count;
}

}
