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

#include "closeonexec.h"
#include "param.h"

#include <iostream>

namespace rcss {

/*-------------------------------------------------------------------*/
UDPConnection::UDPConnection( boost::asio::io_context & io_context )
    : M_socket( io_context ),
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
    boost::system::error_code error;

    M_socket.open( boost::asio::ip::udp::v4(), error );
    if ( ! error )
    {
        error = set_close_on_exec( M_socket.native_handle() );
    }
    if ( error )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << ": Error opening socket: "
                  << error.message() << std::endl;
        close();
        return false;
    }

    M_socket.non_blocking( true, error );
    if ( error )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << ": Error setting socket non-blocking: "
                  << error.message() << std::endl;
        close();
        return false;
    }

    return true;
}

/*-------------------------------------------------------------------*/
bool
UDPConnection::connect( const boost::asio::ip::udp::endpoint & dest )
{
    boost::system::error_code error;
    M_socket.connect( dest, error );
    if ( error )
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
boost::asio::ip::udp::endpoint
UDPConnection::getDest() const
{
    return ( isOpen()
             ? M_dest
             : boost::asio::ip::udp::endpoint() );
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
    std::size_t len = 0;
    boost::system::error_code error;
    do
    {
        len = M_socket.receive( boost::asio::buffer( buffer, MaxMesg ), 0, error );
    }
    while ( error == boost::asio::error::interrupted );

    if ( error )
    {
        if ( error != boost::asio::error::would_block
             && error != boost::asio::error::try_again )
        {
            // ECONNREFUSED means the client has gone away,
            // which is not worth reporting.
            if ( error != boost::asio::error::connection_refused )
            {
                std::cerr << __FILE__ << ": " << __LINE__
                          << ": Error receiving from socket: "
                          << error.message() << std::endl;
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
        && M_socket.is_open();
}

/*-------------------------------------------------------------------*/
void
UDPConnection::close()
{
    boost::system::error_code ignored;
    M_socket.close( ignored );
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
