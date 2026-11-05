// -*-c++-*-


/*!
  \file datagrambuf.cpp
  \brief The stream buffer that sends its contents as UDP datagrams
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

#include "datagrambuf.h"

#include <cerrno>

namespace rcss {

/*-------------------------------------------------------------------*/
DatagramBuf::DatagramBuf( boost::asio::ip::udp::socket & socket )
    : M_socket( socket ),
      M_dest(),
      M_use_dest( false ),
      M_buffer()
{
    setp( M_buffer.data(), M_buffer.data() + M_buffer.size() );
}

/*-------------------------------------------------------------------*/
DatagramBuf::DatagramBuf( boost::asio::ip::udp::socket & socket,
                          const boost::asio::ip::udp::endpoint & dest )
    : M_socket( socket ),
      M_dest( dest ),
      M_use_dest( true ),
      M_buffer()
{
    setp( M_buffer.data(), M_buffer.data() + M_buffer.size() );
}

/*-------------------------------------------------------------------*/
DatagramBuf::~DatagramBuf()
{
    writeData();
}

/*-------------------------------------------------------------------*/
DatagramBuf::int_type
DatagramBuf::overflow( int_type c )
{
    // the buffer is full: send its contents as one datagram
    if ( ! writeData() )
    {
        return traits_type::eof();
    }

    setp( M_buffer.data(), M_buffer.data() + M_buffer.size() );

    if ( ! traits_type::eq_int_type( c, traits_type::eof() ) )
    {
        sputc( traits_type::to_char_type( c ) );
    }
    return traits_type::not_eof( c );
}

/*-------------------------------------------------------------------*/
int
DatagramBuf::sync()
{
    if ( ! writeData() )
    {
        return -1;
    }

    setp( M_buffer.data(), M_buffer.data() + M_buffer.size() );
    return 0;
}

/*-------------------------------------------------------------------*/
bool
DatagramBuf::writeData()
{
    const std::size_t size = static_cast< std::size_t >( pptr() - pbase() );
    if ( size == 0 )
    {
        return true;
    }

    boost::system::error_code error;
    for ( ; ; )
    {
        if ( M_use_dest )
        {
            M_socket.send_to( boost::asio::buffer( pbase(), size ), M_dest, 0, error );
        }
        else
        {
            M_socket.send( boost::asio::buffer( pbase(), size ), 0, error );
        }

        if ( error == boost::asio::error::would_block
             || error == boost::asio::error::try_again )
        {
            // The socket is non-blocking. Wait until it can send, as
            // rcss::net::Socket retried until the datagram was sent.
            M_socket.wait( boost::asio::socket_base::wait_write, error );
            if ( ! error )
            {
                continue;
            }
        }
        else if ( error == boost::asio::error::interrupted )
        {
            continue;
        }
        break;
    }

    if ( error )
    {
        // RemoteClient::send() reads errno, e.g. to ignore ECONNREFUSED
        errno = error.value();
        return false;
    }

    return true;
}

}
