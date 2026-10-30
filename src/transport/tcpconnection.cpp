// -*-c++-*-


/*!
  \file tcpconnection.cpp
  \brief The connection implementation over TCP
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

#include "tcpconnection.h"

#include "param.h"

#include <boost/asio/write.hpp>

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
    return error == boost::asio::error::eof
        || error == boost::asio::error::connection_reset
        || error == boost::asio::error::connection_aborted
        || error == boost::asio::error::broken_pipe;
}

}

/*-------------------------------------------------------------------*/
TCPConnection::OutBuf::OutBuf( TCPConnection & owner )
    : M_owner( owner ),
      M_message()
{

}

/*-------------------------------------------------------------------*/
TCPConnection::OutBuf::int_type
TCPConnection::OutBuf::overflow( int_type c )
{
    // There is no put area, so every character comes here or to xsputn().
    if ( ! traits_type::eq_int_type( c, traits_type::eof() ) )
    {
        M_message.push_back( traits_type::to_char_type( c ) );
    }
    return traits_type::not_eof( c );
}

/*-------------------------------------------------------------------*/
std::streamsize
TCPConnection::OutBuf::xsputn( const char_type * s,
                               std::streamsize n )
{
    M_message.append( s, n );
    return n;
}

/*-------------------------------------------------------------------*/
int
TCPConnection::OutBuf::sync()
{
    if ( ! M_message.empty() )
    {
        M_owner.send( M_message );
        M_message.clear();
    }

    // Errors are not reported to the stream. A failed connection is
    // detected through isOpen() instead.
    return 0;
}

/*-------------------------------------------------------------------*/
TCPConnection::TCPConnection( boost::asio::ip::tcp::socket socket )
    : M_socket( std::move( socket ) ),
      M_peer(),
      M_outbuf( *this ),
      M_read_buf(),
      M_partial(),
      M_inbox(),
      M_outbox(),
      M_outbox_bytes( 0 ),
      M_writing( false ),
      M_read_closed( false ),
      M_closed( false )
{
    boost::system::error_code error;
    const boost::asio::ip::tcp::endpoint peer = M_socket.remote_endpoint( error );
    if ( ! error )
    {
        M_peer = peer.address().to_string() + ':' + std::to_string( peer.port() );
    }
}

/*-------------------------------------------------------------------*/
TCPConnection::~TCPConnection()
{
    closeSocket();
}

/*-------------------------------------------------------------------*/
void
TCPConnection::start()
{
    boost::system::error_code error;

    // send() writes without blocking and queues what the socket did not accept
    M_socket.non_blocking( true, error );
    if ( error )
    {
        shutdown( "cannot set non-blocking mode: " + error.message() );
        return;
    }

    // Messages are small and must not be delayed by the Nagle algorithm.
    M_socket.set_option( boost::asio::ip::tcp::no_delay( true ), error );

    startRead();
}

/*-------------------------------------------------------------------*/
std::streambuf &
TCPConnection::outbuf()
{
    return M_outbuf;
}

/*-------------------------------------------------------------------*/
bool
TCPConnection::popMessage( std::string & msg )
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
TCPConnection::isOpen() const
{
    // After the client has closed the connection, the messages that arrived
    // before are still delivered.
    return ! M_closed
        && ( ! M_read_closed
             || ! M_inbox.empty() );
}

/*-------------------------------------------------------------------*/
void
TCPConnection::close()
{
    M_closed = true;
    closeSocket();
}

/*-------------------------------------------------------------------*/
bool
TCPConnection::isBinarySafe() const
{
    // a null character terminates a message
    return false;
}

/*-------------------------------------------------------------------*/
void
TCPConnection::send( std::string & msg )
{
    if ( M_closed
         || M_read_closed )
    {
        return;
    }

    if ( msg.back() != '\0' )
    {
        msg.push_back( '\0' );
    }

    if ( ! M_writing )
    {
        // Write as much as the socket accepts now. An asynchronous write
        // would delay the message until the io_context is polled next.
        boost::system::error_code error;
        const std::size_t n = M_socket.write_some( boost::asio::buffer( msg ), error );
        if ( error
             && error != boost::asio::error::would_block
             && error != boost::asio::error::try_again )
        {
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

        if ( n == msg.size() )
        {
            return;
        }
        msg.erase( 0, n );
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
TCPConnection::startRead()
{
    std::shared_ptr< TCPConnection > self = shared_from_this();
    M_socket.async_read_some( boost::asio::buffer( M_read_buf ),
                              [self]( const boost::system::error_code & error,
                                      std::size_t bytes )
                              {
                                  self->handleRead( error, bytes );
                              } );
}

/*-------------------------------------------------------------------*/
void
TCPConnection::handleRead( const boost::system::error_code & error,
                           std::size_t bytes )
{
    if ( error )
    {
        if ( error == boost::asio::error::operation_aborted )
        {
            return;
        }

        if ( ! is_disconnection( error ) )
        {
            std::cerr << "TCP connection from " << M_peer
                      << ": read failed: " << error.message() << std::endl;
        }

        // A client that closes its sending side is regarded as disconnected.
        M_read_closed = true;
        closeSocket();
        return;
    }

    for ( std::size_t i = 0; i < bytes; ++i )
    {
        const char c = M_read_buf[i];

        if ( c == '\0'
             || c == '\n' )
        {
            if ( c == '\n'
                 && ! M_partial.empty()
                 && M_partial.back() == '\r' )
            {
                M_partial.pop_back();
            }

            if ( ! M_partial.empty() )
            {
                // Keep the null character as UDP clients send it. The
                // parsers warn about a message without it.
                M_partial.push_back( '\0' );
                M_inbox.push_back( std::move( M_partial ) );
                M_partial.clear();
            }
        }
        else
        {
            // leave room for the null character
            if ( M_partial.size() + 1 >= MaxMesg )
            {
                shutdown( "too long message" );
                return;
            }
            M_partial.push_back( c );
        }
    }

    startRead();
}

/*-------------------------------------------------------------------*/
void
TCPConnection::startWrite()
{
    M_writing = true;

    // The front element of a deque stays at the same address while
    // other messages are pushed back.
    std::shared_ptr< TCPConnection > self = shared_from_this();
    boost::asio::async_write( M_socket,
                              boost::asio::buffer( M_outbox.front() ),
                              [self]( const boost::system::error_code & error,
                                      std::size_t )
                              {
                                  self->handleWrite( error );
                              } );
}

/*-------------------------------------------------------------------*/
void
TCPConnection::handleWrite( const boost::system::error_code & error )
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
TCPConnection::closeSocket()
{
    // pending operations are cancelled and their handlers get operation_aborted
    boost::system::error_code error;
    M_socket.close( error );
}

/*-------------------------------------------------------------------*/
void
TCPConnection::shutdown( const std::string & reason )
{
    std::cerr << "TCP connection from " << M_peer
              << " closed: " << reason << std::endl;
    M_closed = true;
    closeSocket();
}

}
