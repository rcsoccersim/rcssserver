// -*-c++-*-


/*!
  \file tcplisten.h
  \brief The function to open a listening TCP socket
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

#ifndef RCSS_TCPLISTEN_H
#define RCSS_TCPLISTEN_H

#include <boost/asio/ip/tcp.hpp>

namespace rcss {

/*!
  \brief listen at the given port on all interfaces.
  \param acceptor the acceptor to open
  \param port the port number
  \return the error, or a value that evaluates to false on success

  IPv6 with IPv4-mapped addresses (dual stack) is used if possible.
  Otherwise the acceptor listens on IPv4 only.
 */
boost::system::error_code
listen_tcp( boost::asio::ip::tcp::acceptor & acceptor,
            const unsigned short port );

}

#endif
