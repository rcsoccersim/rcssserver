// -*-c++-*-


/*!
  \file wsconnection.cpp
  \brief The connection implementation over WebSocket
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

#include "wsconnection.h"

#include "param.h"

#include <iostream>

namespace rcss {

namespace {

//! The limit of the messages waiting to be sent. A client that does not
//! read its messages for this long is disconnected.
const std::size_t MAX_OUTBOX_BYTES = 1024 * 1024;

/*!
  \brief check if an error means that the client has gone away.
 */
bool
is_disconnection( const boost::system::error_code & error )
{
    return error == boost::beast::websocket::error::closed
        || error == boost::beast::http::error::end_of_stream
        || error == boost::asio::error::eof
        || error == boost::asio::error::connection_reset
        || error == boost::asio::error::connection_aborted
        || error == boost::asio::error::broken_pipe;
}

/*!
  \brief convert a string view of Beast to std::string.
 */
template < typename StringView >
std::string
to_string( const StringView & view )
{
    return std::string( view.data(), view.size() );
}

}

/*-------------------------------------------------------------------*/
WSConnection::WSConnection( boost::asio::ip::tcp::socket socket )
    : M_ws( std::move( socket ) ),
      M_peer(),
      M_path(),
      M_outbuf( [this]( std::string & msg ) { send( msg ); } ),
      M_read_buf(),
      M_request(),
      M_inbox(),
      M_outbox(),
      M_outbox_bytes( 0 ),
      M_writing( false ),
      M_opened( false ),
      M_read_closed( false ),
      M_closed( false )
{
    boost::asio::ip::tcp::socket & tcp_socket = boost::beast::get_lowest_layer( M_ws ).socket();

    boost::system::error_code error;
    const boost::asio::ip::tcp::endpoint peer = tcp_socket.remote_endpoint( error );
    if ( ! error )
    {
        M_peer = peer.address().to_string() + ':' + std::to_string( peer.port() );
    }

    // Messages are small and must not be delayed by the Nagle algorithm.
    tcp_socket.set_option( boost::asio::ip::tcp::no_delay( true ), error );
}

/*-------------------------------------------------------------------*/
WSConnection::~WSConnection()
{
    closeSocket();
}

/*-------------------------------------------------------------------*/
void
WSConnection::start( const Checker & checker )
{
    // read the HTTP request of the opening handshake
    std::shared_ptr< WSConnection > self = shared_from_this();
    boost::beast::http::async_read( M_ws.next_layer(),
                                    M_read_buf,
                                    M_request,
                                    [self, checker]( const boost::system::error_code & error,
                                                     std::size_t )
                                    {
                                        self->handleRequest( error, checker );
                                    } );
}

/*-------------------------------------------------------------------*/
void
WSConnection::handleRequest( const boost::system::error_code & error,
                             const Checker & checker )
{
    namespace http = boost::beast::http;

    if ( error )
    {
        if ( error != boost::asio::error::operation_aborted
             && ! is_disconnection( error ) )
        {
            std::cerr << "WebSocket connection from " << M_peer
                      << " closed: bad request: " << error.message() << std::endl;
        }
        M_closed = true;
        closeSocket();
        return;
    }

    if ( ! boost::beast::websocket::is_upgrade( M_request ) )
    {
        reject( http::status::bad_request );
        return;
    }

    std::string path = to_string( M_request.target() );
    const std::string::size_type query = path.find( '?' );
    if ( query != std::string::npos )
    {
        // the query string is not used
        path.erase( query );
    }

    const http::request< http::string_body >::const_iterator origin
        = M_request.find( http::field::origin );

    const http::status status = checker( path,
                                         ( origin != M_request.end()
                                           ? to_string( origin->value() )
                                           : std::string() ) );
    if ( status != http::status::switching_protocols )
    {
        reject( status );
        return;
    }

    M_path = path;

    // The timeouts of the WebSocket stream replace those of the TCP stream.
    boost::beast::get_lowest_layer( M_ws ).expires_never();
    M_ws.set_option( boost::beast::websocket::stream_base::timeout::suggested( boost::beast::role_type::server ) );
    M_ws.read_message_max( MaxMesg );
    M_ws.text( true );

    std::shared_ptr< WSConnection > self = shared_from_this();
    M_ws.async_accept( M_request,
                       [self]( const boost::system::error_code & error )
                       {
                           self->handleAccept( error );
                       } );
}

/*-------------------------------------------------------------------*/
void
WSConnection::reject( const boost::beast::http::status status )
{
    namespace http = boost::beast::http;

    const std::string reason = to_string( http::obsolete_reason( status ) );
    std::cerr << "WebSocket connection from " << M_peer
              << " refused: " << static_cast< int >( status ) << ' ' << reason
              << " (path=" << to_string( M_request.target() );
    const http::request< http::string_body >::const_iterator origin
        = M_request.find( http::field::origin );
    if ( origin != M_request.end() )
    {
        std::cerr << " origin=" << to_string( origin->value() );
    }
    std::cerr << ')' << std::endl;

    // this connection is never used for a client
    M_closed = true;

    std::shared_ptr< http::response< http::string_body > > response
        = std::make_shared< http::response< http::string_body > >( status, M_request.version() );
    response->set( http::field::content_type, "text/plain" );
    response->body() = reason + '\n';
    response->keep_alive( false );
    response->prepare_payload();

    std::shared_ptr< WSConnection > self = shared_from_this();
    http::async_write( M_ws.next_layer(),
                       *response,
                       [self, response]( const boost::system::error_code &,
                                         std::size_t )
                       {
                           self->closeSocket();
                       } );
}

/*-------------------------------------------------------------------*/
void
WSConnection::handleAccept( const boost::system::error_code & error )
{
    if ( error )
    {
        if ( error != boost::asio::error::operation_aborted
             && ! is_disconnection( error ) )
        {
            std::cerr << "WebSocket connection from " << M_peer
                      << " closed: handshake failed: " << error.message() << std::endl;
        }
        M_closed = true;
        closeSocket();
        return;
    }

    M_opened = true;
    startRead();
}

/*-------------------------------------------------------------------*/
std::streambuf &
WSConnection::outbuf()
{
    return M_outbuf;
}

/*-------------------------------------------------------------------*/
bool
WSConnection::popMessage( std::string & msg )
{
    if ( M_closed
         || M_inbox.empty() )
    {
        return false;
    }

    msg = std::move( M_inbox.front() );
    M_inbox.pop_front();
    return true;
}

/*-------------------------------------------------------------------*/
bool
WSConnection::isOpen() const
{
    // After the client has closed the connection, the messages that arrived
    // before are still delivered.
    return ! M_closed
        && ( ! M_read_closed
             || ! M_inbox.empty() );
}

/*-------------------------------------------------------------------*/
void
WSConnection::close()
{
    M_closed = true;
    closeSocket();
}

/*-------------------------------------------------------------------*/
bool
WSConnection::isBinarySafe() const
{
    // messages are sent as text
    return false;
}

/*-------------------------------------------------------------------*/
void
WSConnection::send( std::string & msg )
{
    if ( ! M_opened
         || M_closed
         || M_read_closed )
    {
        return;
    }

    // a WebSocket message does not need the terminating null character
    if ( ! msg.empty()
         && msg.back() == '\0' )
    {
        msg.pop_back();
    }

    if ( msg.empty() )
    {
        return;
    }

    M_outbox_bytes += msg.size();
    M_outbox.push_back( std::move( msg ) );

    if ( M_outbox_bytes > MAX_OUTBOX_BYTES )
    {
        shutdown( "the client does not read its messages" );
        return;
    }

    if ( ! M_writing )
    {
        startWrite();
    }
}

/*-------------------------------------------------------------------*/
void
WSConnection::startRead()
{
    std::shared_ptr< WSConnection > self = shared_from_this();
    M_ws.async_read( M_read_buf,
                     [self]( const boost::system::error_code & error,
                             std::size_t )
                     {
                         self->handleRead( error );
                     } );
}

/*-------------------------------------------------------------------*/
void
WSConnection::handleRead( const boost::system::error_code & error )
{
    if ( error )
    {
        if ( error == boost::asio::error::operation_aborted )
        {
            return;
        }

        if ( error == boost::beast::websocket::error::message_too_big )
        {
            shutdown( "too long message" );
            return;
        }

        if ( ! is_disconnection( error ) )
        {
            std::cerr << "WebSocket connection from " << M_peer
                      << ": read failed: " << error.message() << std::endl;
        }
        M_read_closed = true;
        closeSocket();
        return;
    }

    std::string msg = boost::beast::buffers_to_string( M_read_buf.data() );
    M_read_buf.consume( M_read_buf.size() );

    while ( ! msg.empty()
            && ( msg.back() == '\0'
                 || msg.back() == '\n'
                 || msg.back() == '\r' ) )
    {
        msg.pop_back();
    }

    if ( ! msg.empty() )
    {
        // leave room for the null character
        if ( msg.size() + 1 > MaxMesg )
        {
            shutdown( "too long message" );
            return;
        }

        // Add the null character as UDP clients send it. The parsers warn
        // about a message without it.
        msg.push_back( '\0' );
        M_inbox.push_back( std::move( msg ) );
    }

    startRead();
}

/*-------------------------------------------------------------------*/
void
WSConnection::startWrite()
{
    M_writing = true;

    // The front element of a deque stays at the same address while
    // other messages are pushed back.
    std::shared_ptr< WSConnection > self = shared_from_this();
    M_ws.async_write( boost::asio::buffer( M_outbox.front() ),
                      [self]( const boost::system::error_code & error,
                              std::size_t )
                      {
                          self->handleWrite( error );
                      } );
}

/*-------------------------------------------------------------------*/
void
WSConnection::handleWrite( const boost::system::error_code & error )
{
    M_writing = false;

    if ( error )
    {
        if ( error == boost::asio::error::operation_aborted )
        {
            return;
        }

        if ( is_disconnection( error ) )
        {
            M_read_closed = true;
            closeSocket();
        }
        else
        {
            shutdown( "write failed: " + error.message() );
        }
        return;
    }

    M_outbox_bytes -= M_outbox.front().size();
    M_outbox.pop_front();

    if ( ! M_outbox.empty() )
    {
        startWrite();
    }
}

/*-------------------------------------------------------------------*/
void
WSConnection::closeSocket()
{
    // pending operations are cancelled and their handlers get operation_aborted
    boost::beast::get_lowest_layer( M_ws ).close();
}

/*-------------------------------------------------------------------*/
void
WSConnection::shutdown( const std::string & reason )
{
    std::cerr << "WebSocket connection from " << M_peer
              << " closed: " << reason << std::endl;
    M_closed = true;
    closeSocket();
}

}
