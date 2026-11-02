// -*-c++-*-


/*!
  \file wsserver.h
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

#ifndef RCSS_WSSERVER_H
#define RCSS_WSSERVER_H

#include "acceptor.h"
#include "pendingconnections.h"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/http/status.hpp>

#include <chrono>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace rcss {

class WSConnection;

/*!
  \class WSServer
  \brief The WebSocket port shared by the services.

  A client chooses the service by the request path of the opening handshake
  (e.g. "/player"). The server keeps the connections whose clients have not
  been accepted for each path, and a WSAcceptor for each path takes inbound
  messages from them.

  The server must be created by std::make_shared, because the pending accept
  operation holds a shared pointer to it.
 */
class WSServer
    : public std::enable_shared_from_this< WSServer > {
private:
    //! a connection in the opening handshake
    struct Handshake {
        std::shared_ptr< WSConnection > connection;
        std::chrono::steady_clock::time_point since;
    };

    boost::asio::ip::tcp::acceptor M_acceptor;

    //! decides whether to accept an opening handshake
    std::function< boost::beast::http::status( const std::string &, const std::string & ) > M_checker;

    std::vector< Handshake > M_handshakes;
    std::map< std::string, PendingConnections > M_pending; //!< keyed by the path

public:
    /*!
      \brief create a server.
      \param io_context the io_context that runs the asynchronous I/O
      \param paths the request paths of the services
      \param allowed_origins the values of the Origin header to accept.
      If empty, all origins are accepted. A request without the Origin header
      (i.e. not from a web browser) is always accepted.
     */
    WSServer( boost::asio::io_context & io_context,
              const std::vector< std::string > & paths,
              const std::vector< std::string > & allowed_origins );

    ~WSServer();

    /*!
      \brief listen at the given port on all interfaces.
      \param port the port number
      \return the error, or a value that evaluates to false on success
     */
    boost::system::error_code open( const unsigned short port );

    /*!
      \brief take one inbound message for a path without blocking.
      \param path the request path of the service
      \param in set to the inbound message
      \return false if no message is available
     */
    bool popInbound( const std::string & path,
                     Inbound & in );

    /*!
      \brief accept the sender of an inbound message as a new client.
      \param path the request path of the service
      \param in the inbound message taken by popInbound()
      \return the connection, or nullptr if it is not pending
     */
    std::shared_ptr< Connection > accept( const std::string & path,
                                          const Inbound & in );

private:
    void startAccept();
    void handleAccept( const boost::system::error_code & error,
                       boost::asio::ip::tcp::socket socket );
    void checkHandshakes();
    std::size_t waitingConnections() const;
};

}

#endif
