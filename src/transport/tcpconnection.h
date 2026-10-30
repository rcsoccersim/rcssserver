// -*-c++-*-


/*!
  \file tcpconnection.h
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

#ifndef RCSS_TCPCONNECTION_H
#define RCSS_TCPCONNECTION_H

#include "connection.h"

#include <boost/asio/ip/tcp.hpp>

#include <array>
#include <cstddef>
#include <deque>
#include <memory>
#include <string>

namespace rcss {

/*!
  \class TCPConnection
  \brief The connection over a TCP socket.

  Messages are terminated by a null character in both directions. A newline
  also terminates a received message, so that a client can be tested by
  hand with tools like nc.

  The I/O handlers run while the owner of the io_context polls it, and they
  only queue the received messages. An outgoing message is written at once
  if the socket accepts it, and is queued otherwise.
 */
class TCPConnection
    : public Connection,
      public std::enable_shared_from_this< TCPConnection > {
private:

    /*!
      \class OutBuf
      \brief The stream buffer that collects one outgoing message.
     */
    class OutBuf
        : public std::streambuf {
    private:
        TCPConnection & M_owner;
        std::string M_message;

    public:
        explicit
        OutBuf( TCPConnection & owner );

    protected:
        int_type overflow( int_type c ) override;
        std::streamsize xsputn( const char_type * s,
                                std::streamsize n ) override;
        int sync() override;
    };

    boost::asio::ip::tcp::socket M_socket;
    std::string M_peer; //!< the client's address for log messages
    OutBuf M_outbuf;

    std::array< char, 4096 > M_read_buf;
    std::string M_partial; //!< the message being received
    std::deque< std::string > M_inbox; //!< the received messages

    //! the messages waiting to be written. An asynchronous write may be in
    //! progress for the front one.
    std::deque< std::string > M_outbox;
    std::size_t M_outbox_bytes; //!< the total size of M_outbox
    bool M_writing; //!< true while an asynchronous write is in progress

    bool M_read_closed; //!< the client has closed the connection
    bool M_closed;

public:
    explicit
    TCPConnection( boost::asio::ip::tcp::socket socket );

    ~TCPConnection() override;

    /*!
      \brief start receiving messages. Call this once after construction.
     */
    void start();

    std::streambuf & outbuf() override;

    bool popMessage( std::string & msg ) override;

    bool isOpen() const override;

    void close() override;

    bool isBinarySafe() const override;

private:
    void send( std::string & msg );
    void startRead();
    void handleRead( const boost::system::error_code & error,
                     std::size_t bytes );
    void startWrite();
    void handleWrite( const boost::system::error_code & error );

    void closeSocket();
    void shutdown( const std::string & reason );
};

}

#endif
