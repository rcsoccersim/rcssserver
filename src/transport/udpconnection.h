// -*-c++-*-


/*!
  \file udpconnection.h
  \brief The connection implementation over UDP
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

#ifndef RCSS_UDPCONNECTION_H
#define RCSS_UDPCONNECTION_H

#include "connection.h"
#include "datagrambuf.h"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/udp.hpp>

#include <memory>

namespace rcss {

/*!
  \class UDPConnection
  \brief The connection over a UDP socket dedicated to one client.

  The socket is connected to the client's address. The client receives the
  server's replies from this socket's port, and sends its following messages
  to that port (the "dedicated port" of the rcssserver protocol).
  One datagram carries one message.

  The socket is non-blocking, and popMessage() reads it directly. The only
  asynchronous operation is a wait for the next datagram, which wakes up the
  event loop, e.g. while the server waits for the clients in sync mode.
  The wait does not read the datagram.
 */
class UDPConnection
    : public Connection,
      public std::enable_shared_from_this< UDPConnection > {
private:
    boost::asio::ip::udp::socket M_socket;
    DatagramBuf M_outbuf; //!< must be declared after M_socket
    boost::asio::ip::udp::endpoint M_dest;
    bool M_connected;
    bool M_waiting; //!< true while waiting for a datagram to arrive

public:
    explicit
    UDPConnection( boost::asio::io_context & io_context );

    ~UDPConnection() override;

    /*!
      \brief open a new IPv4 socket in non-blocking mode.
      \return true if successful
     */
    bool open();

    /*!
      \brief connect the socket to the client.

      The connection must be owned by a shared pointer, so that the wait
      for datagrams can refer to it.
      \param dest the client's address
      \return true if successful. The socket is closed on failure.
     */
    bool connect( const boost::asio::ip::udp::endpoint & dest );

    /*!
      \brief get the client's address.
      \return the address, or the default endpoint if not connected
     */
    boost::asio::ip::udp::endpoint getDest() const;

    std::streambuf & outbuf() override;

    bool popMessage( std::string & msg ) override;

    bool isOpen() const override;

    void close() override;

    bool isBinarySafe() const override;

private:
    void waitRead();
};

}

#endif
