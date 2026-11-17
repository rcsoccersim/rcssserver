// -*-c++-*-


/*!
  \file pendingconnections.h
  \brief The connections whose clients have not been accepted
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

#ifndef RCSS_PENDINGCONNECTIONS_H
#define RCSS_PENDINGCONNECTIONS_H

#include "acceptor.h"

#include <chrono>
#include <cstddef>
#include <memory>
#include <vector>

namespace rcss {

/*!
  \class PendingConnections
  \brief The connections whose clients have not been accepted.

  This is used by the acceptors of connection-oriented transports, where a
  connection is dedicated to one client from the beginning. Until Stadium
  accepts the client, the messages from the connection are taken as inbound
  messages one by one. After that, they are left in the connection for the
  client.
 */
class PendingConnections {
public:
    //! A connection is closed if its client is not accepted within this time.
    static const std::chrono::seconds TIMEOUT;

    //! The maximum number of pending connections that an acceptor keeps.
    static const std::size_t MAX_SIZE;

private:
    struct Entry {
        std::shared_ptr< Connection > connection;
        std::chrono::steady_clock::time_point since;
    };

    std::vector< Entry > M_entries;

public:
    PendingConnections();

    ~PendingConnections();

    std::size_t size() const
      {
          return M_entries.size();
      }

    /*!
      \brief add a connection.
      \param connection the new connection
      \param since the time when the connection was made
     */
    void add( const std::shared_ptr< Connection > & connection,
              const std::chrono::steady_clock::time_point & since );

    /*!
      \brief take one message from the pending connections without blocking.
      Closed connections and the ones that have waited too long are removed.
      \param in set to the inbound message
      \return false if no message is available
     */
    bool popInbound( Inbound & in );

    /*!
      \brief remove the connection that an inbound message came from.
      \param in the inbound message taken by popInbound()
      \return the connection, or nullptr if it is not pending
     */
    std::shared_ptr< Connection > accept( const Inbound & in );

    /*!
      \brief close and remove all connections.
     */
    void closeAll();
};

}

#endif
