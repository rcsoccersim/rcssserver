// -*-c++-*-


/*!
  \file tcplisten.cpp
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "tcplisten.h"

#include <boost/asio/ip/v6_only.hpp>

namespace rcss {

namespace {

boost::system::error_code
listen_at( boost::asio::ip::tcp::acceptor & acceptor,
           const boost::asio::ip::tcp::endpoint & endpoint,
           const bool dual_stack )
{
    boost::system::error_code error;
    boost::system::error_code ignored;

    acceptor.close( ignored );

    acceptor.open( endpoint.protocol(), error );
    if ( ! error
         && dual_stack )
    {
        // also accept IPv4 clients through IPv4-mapped addresses
        acceptor.set_option( boost::asio::ip::v6_only( false ), error );
    }
    if ( ! error )
    {
        // allow restarting the server while old connections are in TIME_WAIT
        acceptor.set_option( boost::asio::ip::tcp::acceptor::reuse_address( true ), error );
    }
    if ( ! error )
    {
        acceptor.bind( endpoint, error );
    }
    if ( ! error )
    {
        acceptor.listen( boost::asio::socket_base::max_listen_connections, error );
    }

    if ( error )
    {
        acceptor.close( ignored );
    }
    return error;
}

}

/*-------------------------------------------------------------------*/
boost::system::error_code
listen_tcp( boost::asio::ip::tcp::acceptor & acceptor,
            const unsigned short port )
{
    namespace ip = boost::asio::ip;

    const boost::system::error_code error
        = listen_at( acceptor, ip::tcp::endpoint( ip::tcp::v6(), port ), true );
    if ( ! error )
    {
        return error;
    }

    return listen_at( acceptor, ip::tcp::endpoint( ip::tcp::v4(), port ), false );
}

}
