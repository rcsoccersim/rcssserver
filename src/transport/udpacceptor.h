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

#include <rcss/net/udpsocket.hpp>

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
 */
class UDPAcceptor
    : public Acceptor {
private:
    //! the socket bound to the shared port. Replies to inbound messages
    //! share it, so it is held by a shared pointer.
    std::shared_ptr< rcss::net::UDPSocket > M_socket;

    //! the accepted connections, used to recognize their messages
    std::vector< std::weak_ptr< UDPConnection > > M_connections;

public:
    UDPAcceptor();

    ~UDPAcceptor() override;

    /*!
      \brief open the shared socket in non-blocking mode and bind it.
      \param addr the address to bind
      \return true if successful
     */
    bool open( const rcss::net::Addr & addr );

    bool popInbound( Inbound & in ) override;

    std::shared_ptr< Connection > accept( const Inbound & in ) override;

private:
    const UDPConnection * findConnection( const rcss::net::Addr & addr ) const;
};

}

#endif
