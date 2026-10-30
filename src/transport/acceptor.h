// -*-c++-*-


/*!
  \file acceptor.h
  \brief The abstract entrance through which new clients connect to the server
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

#ifndef RCSS_ACCEPTOR_H
#define RCSS_ACCEPTOR_H

#include "connection.h"

#include <memory>
#include <string>

namespace rcss {

/*!
  \struct Inbound
  \brief A message that arrived at an acceptor from a sender that has not
  been accepted as a client.
 */
struct Inbound {
    //! the received message. Its length never exceeds MaxMesg.
    std::string message;

    //! the channel to answer the sender. Always set.
    std::shared_ptr< Connection > reply;

    //! the connection of an accepted client that sent this message to the
    //! shared port instead of its dedicated port, or nullptr. Only the UDP
    //! acceptor sets this.
    const Connection * known = nullptr;
};

/*!
  \class Acceptor
  \brief The entrance through which new clients connect to the server.

  There is one acceptor for each pair of a service (players, offline coaches
  or online coaches) and a transport.
 */
class Acceptor {
private:
    Acceptor( const Acceptor & ) = delete;
    Acceptor & operator=( const Acceptor & ) = delete;

protected:
    Acceptor() = default;

public:
    virtual
    ~Acceptor() = default;

    /*!
      \brief take one inbound message without blocking.
      \param in set to the inbound message
      \return false if no message is available
     */
    virtual
    bool popInbound( Inbound & in ) = 0;

    /*!
      \brief accept the sender of an inbound message as a new client.
      \param in the inbound message taken from this acceptor
      \return the connection dedicated to the client, or nullptr on failure
     */
    virtual
    std::shared_ptr< Connection > accept( const Inbound & in ) = 0;
};

}

#endif
