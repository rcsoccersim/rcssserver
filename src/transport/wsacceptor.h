// -*-c++-*-


/*!
  \file wsacceptor.h
  \brief The acceptor implementation over WebSocket
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

#ifndef RCSS_WSACCEPTOR_H
#define RCSS_WSACCEPTOR_H

#include "acceptor.h"

#include <memory>
#include <string>

namespace rcss {

class WSServer;

/*!
  \class WSAcceptor
  \brief The acceptor of one service on a WebSocket port.

  The acceptors of the services share one WSServer, and each of them takes
  the clients that requested its path.
 */
class WSAcceptor
    : public Acceptor {
private:
    std::shared_ptr< WSServer > M_server;
    std::string M_path;

public:
    /*!
      \brief create an acceptor.
      \param server the server that listens at the WebSocket port
      \param path the request path of the service, e.g. "/player"
     */
    WSAcceptor( const std::shared_ptr< WSServer > & server,
                const std::string & path );

    ~WSAcceptor() override;

    bool popInbound( Inbound & in ) override;

    std::shared_ptr< Connection > accept( const Inbound & in ) override;
};

}

#endif
