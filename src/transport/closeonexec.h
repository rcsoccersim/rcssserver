// -*-c++-*-


/*!
  \file closeonexec.h
  \brief The function that keeps a socket from the programs that the server starts
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

#ifndef RCSS_CLOSEONEXEC_H
#define RCSS_CLOSEONEXEC_H

#include <boost/system/error_code.hpp>

namespace rcss {

/*!
  \brief set the close-on-exec flag of a socket.
  \param handle the native handle of an open socket
  \return the error, or a value that evaluates to false on success

  The programs that the server starts (e.g. server::team_l_start) must not
  inherit the sockets. Otherwise, they keep the server's ports in use after
  the server exits. rcss::net::Socket sets this flag, but asio does not.
 */
boost::system::error_code
set_close_on_exec( const int handle );

}

#endif
