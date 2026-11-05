// -*-c++-*-


/*!
  \file closeonexec.cpp
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "closeonexec.h"

#include <cerrno>
#include <fcntl.h>

namespace rcss {

/*-------------------------------------------------------------------*/
boost::system::error_code
set_close_on_exec( const int handle )
{
    boost::system::error_code error;

    // keep the other descriptor flags, if any
    const int flags = ::fcntl( handle, F_GETFD );
    if ( flags == -1
         || ::fcntl( handle, F_SETFD, flags | FD_CLOEXEC ) == -1 )
    {
        error.assign( errno, boost::system::system_category() );
    }
    return error;
}

}
