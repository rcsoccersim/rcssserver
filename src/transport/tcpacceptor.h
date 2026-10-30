// -*-c++-*-


/*!
  \file tcpacceptor.h
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

#ifndef RCSS_TCPACCEPTOR_H
#define RCSS_TCPACCEPTOR_H

#include "acceptor.h"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/tcp.hpp>

#include <chrono>
#include <memory>
#include <vector>

namespace rcss {

class TCPConnection;

/*!
  \class TCPAcceptor
  \brief The acceptor over a TCP port.

  Every TCP connection is dedicated to one client from the beginning. Until
  Stadium accepts the client, the messages from the connection are taken as
  inbound messages one by one. After that, they are left in the connection
  for the client.

  The acceptor must be destroyed before the io_context, because the pending
  accept operation refers to it.
 */
class TCPAcceptor
    : public Acceptor {
private:

    //! a connection whose client has not been accepted
    struct Pending {
        std::shared_ptr< TCPConnection > connection;
        std::chrono::steady_clock::time_point since;
    };

    boost::asio::ip::tcp::acceptor M_acceptor;
    std::vector< Pending > M_pending;

public:
    explicit
    TCPAcceptor( boost::asio::io_context & io_context );

    ~TCPAcceptor() override;

    /*!
      \brief listen at the given port on IPv6 and IPv4, or on IPv4 only if
      IPv6 is not available.
      \param port the port number
      \return the error, or a value that evaluates to false on success
     */
    boost::system::error_code open( const unsigned short port );

    bool popInbound( Inbound & in ) override;

    std::shared_ptr< Connection > accept( const Inbound & in ) override;

private:
    boost::system::error_code listen( const boost::asio::ip::tcp::endpoint & endpoint,
                                      const bool dual_stack );
    void startAccept();
    void handleAccept( const boost::system::error_code & error,
                       boost::asio::ip::tcp::socket socket );
};

}

#endif
