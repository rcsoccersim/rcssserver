// -*-c++-*-


/*!
  \file connection.h
  \brief The abstract message channel between the server and one client
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

#ifndef RCSS_CONNECTION_H
#define RCSS_CONNECTION_H

#include <streambuf>
#include <string>

namespace rcss {

/*!
  \class Connection
  \brief The message channel between the server and one client.

  A connection carries whole messages, not a byte stream. Each transport
  implements its own message framing behind this interface, so that
  RemoteClient and the senders do not depend on the transport.
 */
class Connection {
private:
    Connection( const Connection & ) = delete;
    Connection & operator=( const Connection & ) = delete;

protected:
    Connection() = default;

public:
    virtual
    ~Connection() = default;

    /*!
      \brief get the buffer that outgoing messages are written to.
      \return reference to the buffer

      One pubsync() (i.e. std::flush on the stream) completes one message.
     */
    virtual
    std::streambuf & outbuf() = 0;

    /*!
      \brief take one received message without blocking.
      \param msg set to the received message. Its length never exceeds MaxMesg.
      \return true if a message was taken. false if no message is available,
      or if the connection has been closed because of an error. Use isOpen()
      to tell these cases apart.
     */
    virtual
    bool popMessage( std::string & msg ) = 0;

    /*!
      \brief check if messages can be exchanged with the client.
      \return true if the connection is usable
     */
    virtual
    bool isOpen() const = 0;

    /*!
      \brief close the connection.
     */
    virtual
    void close() = 0;
};

}

#endif
