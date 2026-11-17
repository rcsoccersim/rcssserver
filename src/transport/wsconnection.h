// -*-c++-*-


/*!
  \file wsconnection.h
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

#ifndef RCSS_WSCONNECTION_H
#define RCSS_WSCONNECTION_H

#include "connection.h"
#include "messagebuf.h"

#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/websocket.hpp>

#include <cstddef>
#include <deque>
#include <functional>
#include <memory>
#include <string>

namespace rcss {

/*!
  \class WSConnection
  \brief The connection over WebSocket.

  One WebSocket message carries one protocol message. The server sends text
  messages without the null character that terminates messages on the
  other transports. A null character, newline or carriage return at the end
  of a received message is ignored.

  The connection starts with the opening handshake. The request is checked
  by the function given to start(), which decides by the request path and
  the Origin header.
 */
class WSConnection
    : public Connection,
      public std::enable_shared_from_this< WSConnection > {
public:
    /*!
      \brief decides whether to accept an opening handshake.
      The arguments are the request path and the Origin header (empty if
      the request has none). The function returns
      http::status::switching_protocols to accept the request, or the error
      status to reply.
     */
    typedef std::function< boost::beast::http::status( const std::string & path,
                                                       const std::string & origin ) > Checker;

private:
    typedef boost::beast::websocket::stream< boost::beast::tcp_stream > Stream;

    Stream M_ws;
    std::string M_peer; //!< the client's address for log messages
    std::string M_path; //!< the request path of the opening handshake
    MessageBuf M_outbuf;

    boost::beast::flat_buffer M_read_buf;
    boost::beast::http::request< boost::beast::http::string_body > M_request;
    std::deque< std::string > M_inbox; //!< the received messages

    //! the messages waiting to be written. An asynchronous write may be in
    //! progress for the front one.
    std::deque< std::string > M_outbox;
    std::size_t M_outbox_bytes; //!< the total size of M_outbox
    bool M_writing; //!< true while an asynchronous write is in progress

    bool M_opened; //!< the opening handshake has completed
    bool M_read_closed; //!< the client has closed the connection
    bool M_closed;

public:
    explicit
    WSConnection( boost::asio::ip::tcp::socket socket );

    ~WSConnection() override;

    /*!
      \brief start the opening handshake. Call this once after construction.
      \param checker the function that decides whether to accept the request
     */
    void start( const Checker & checker );

    //! check if the opening handshake has completed
    bool isOpened() const
      {
          return M_opened;
      }

    //! the request path of the opening handshake
    const std::string & path() const
      {
          return M_path;
      }

    std::streambuf & outbuf() override;

    bool popMessage( std::string & msg ) override;

    bool isOpen() const override;

    void close() override;

    bool isBinarySafe() const override;

private:
    void handleRequest( const boost::system::error_code & error,
                        const Checker & checker );
    void reject( const boost::beast::http::status status );
    void handleAccept( const boost::system::error_code & error );

    void send( std::string & msg );
    void startRead();
    void handleRead( const boost::system::error_code & error );
    void startWrite();
    void handleWrite( const boost::system::error_code & error );

    void closeSocket();
    void shutdown( const std::string & reason );
};

}

#endif
