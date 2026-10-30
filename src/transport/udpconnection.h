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

#include <rcss/net/udpsocket.hpp>
#include <rcss/net/socketstreambuf.hpp>

namespace rcss {

/*!
  \class UDPConnection
  \brief The connection over a UDP socket dedicated to one client.

  The socket is connected to the client's address. The client receives the
  server's replies from this socket's port, and sends its following messages
  to that port (the "dedicated port" of the rcssserver protocol).
  One datagram carries one message.
 */
class UDPConnection
    : public Connection {
private:
    rcss::net::UDPSocket M_socket;
    rcss::net::SocketStreamBuf M_outbuf; //!< must be declared after M_socket
    rcss::net::Addr M_dest;
    bool M_connected;

public:
    UDPConnection();

    ~UDPConnection() override;

    /*!
      \brief open a new socket in non-blocking mode.
      \return true if successful
     */
    bool open();

    /*!
      \brief connect the socket to the client.
      \param dest the client's address
      \return true if successful. The socket is closed on failure.
     */
    bool connect( const rcss::net::Addr & dest );

    /*!
      \brief get the client's address.
      \return the address, or the default Addr if not connected
     */
    rcss::net::Addr getDest() const;

    std::streambuf & outbuf() override;

    bool popMessage( std::string & msg ) override;

    bool isOpen() const override;

    void close() override;

    bool isBinarySafe() const override;
};

}

#endif
