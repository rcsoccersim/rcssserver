// -*-c++-*-


/*!
  \file udpacceptor.h
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

#ifndef RCSS_UDPACCEPTOR_H
#define RCSS_UDPACCEPTOR_H

#include "acceptor.h"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/udp.hpp>

#include <memory>
#include <vector>

namespace rcss {

class UDPConnection;

/*!
  \class UDPAcceptor
  \brief The acceptor over a UDP port shared by all clients of a service.

  A new client sends its first message to the shared port. When the client
  is accepted, it gets a connection with its own socket (the dedicated port).
  Accepted clients may still send messages to the shared port. Those
  messages are taken as inbound messages with Inbound::known set.

  The socket is non-blocking, and popInbound() reads it directly. No
  asynchronous operation is used.
 */
class UDPAcceptor
    : public Acceptor {
private:
    boost::asio::io_context & M_io_context;

    //! the socket bound to the shared port. Replies to inbound messages
    //! share it, so it is held by a shared pointer.
    std::shared_ptr< boost::asio::ip::udp::socket > M_socket;

    //! the accepted connections, used to recognize their messages
    std::vector< std::weak_ptr< UDPConnection > > M_connections;

public:
    explicit
    UDPAcceptor( boost::asio::io_context & io_context );

    ~UDPAcceptor() override;

    /*!
      \brief open the shared IPv4 socket in non-blocking mode and bind it
      to the given port on all interfaces.
      \param port the port number
      \return the error, or a value that evaluates to false on success
     */
    boost::system::error_code open( const unsigned short port );

    bool popInbound( Inbound & in ) override;

    std::shared_ptr< Connection > accept( const Inbound & in ) override;

private:
    const UDPConnection * findConnection( const boost::asio::ip::udp::endpoint & endpoint ) const;
};

}

#endif
