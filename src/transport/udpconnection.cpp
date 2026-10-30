// -*-c++-*-


/*!
  \file udpconnection.cpp
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "udpconnection.h"

#include "param.h"

#include <cerrno>
#include <cstring>
#include <iostream>

namespace rcss {

/*-------------------------------------------------------------------*/
UDPConnection::UDPConnection()
    : M_socket(),
      M_outbuf( M_socket ),
      M_dest(),
      M_connected( false )
{

}

/*-------------------------------------------------------------------*/
UDPConnection::~UDPConnection()
{
    close();
}

/*-------------------------------------------------------------------*/
bool
UDPConnection::open()
{
    if ( ! M_socket.open() )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << ": Error opening socket: "
                  << std::strerror( errno ) << std::endl;
        close();
        return false;
    }

    if ( M_socket.setNonBlocking() < 0 )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << ": Error setting socket non-blocking: "
                  << std::strerror( errno ) << std::endl;
        close();
        return false;
    }

    return true;
}

/*-------------------------------------------------------------------*/
bool
UDPConnection::connect( const rcss::net::Addr & dest )
{
    if ( ! M_socket.connect( dest ) )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << ": Error connecting socket" << std::endl;
        close();
        return false;
    }

    M_dest = dest;
    M_connected = true;
    return true;
}

/*-------------------------------------------------------------------*/
rcss::net::Addr
UDPConnection::getDest() const
{
    return ( isOpen()
             ? M_dest
             : rcss::net::Addr() );
}

/*-------------------------------------------------------------------*/
std::streambuf &
UDPConnection::outbuf()
{
    return M_outbuf;
}

/*-------------------------------------------------------------------*/
bool
UDPConnection::popMessage( std::string & msg )
{
    if ( ! isOpen() )
    {
        return false;
    }

    char buffer[ MaxMesg ];
    const int len = M_socket.recv( buffer, MaxMesg );

    if ( len < 0 )
    {
        if ( errno != EWOULDBLOCK )
        {
            // ECONNREFUSED means the client has gone away,
            // which is not worth reporting.
            if ( errno != ECONNREFUSED )
            {
                std::cerr << __FILE__ << ": " << __LINE__
                          << ": Error receiving from socket: "
                          << std::strerror( errno ) << std::endl;
            }
            close();
        }
        return false;
    }

    msg.assign( buffer, len );
    return true;
}

/*-------------------------------------------------------------------*/
bool
UDPConnection::isOpen() const
{
    return M_connected
        && M_socket.isOpen();
}

/*-------------------------------------------------------------------*/
void
UDPConnection::close()
{
    M_socket.close();
    M_connected = false;
}

/*-------------------------------------------------------------------*/
bool
UDPConnection::isBinarySafe() const
{
    // one datagram carries one message of any bytes
    return true;
}

}
