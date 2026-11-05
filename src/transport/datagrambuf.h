// -*-c++-*-


/*!
  \file datagrambuf.h
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

#ifndef RCSS_DATAGRAMBUF_H
#define RCSS_DATAGRAMBUF_H

#include <boost/asio/ip/udp.hpp>

#include <array>
#include <streambuf>

namespace rcss {

/*!
  \class DatagramBuf
  \brief The stream buffer that sends its contents as UDP datagrams.

  sync() (i.e. std::flush on the stream) sends the buffered characters as
  one datagram. When the buffer is full, its contents are sent at that
  point, so a message longer than the buffer is split into several
  datagrams. This is the behavior of rcss::net::SocketStreamBuf, which this
  class replaces.

  Sending waits while the socket's send buffer is full. If sending fails,
  the characters stay in the buffer, the stream gets the error state, and
  errno tells the reason.
 */
class DatagramBuf
    : public std::streambuf {
private:
    boost::asio::ip::udp::socket & M_socket;
    boost::asio::ip::udp::endpoint M_dest;
    bool M_use_dest; //!< true if sent to M_dest, false if to the connected peer
    std::array< char, 8192 > M_buffer;

public:
    /*!
      \brief create a buffer that sends to the peer of a connected socket.
      \param socket the socket, which must outlive this buffer
     */
    explicit
    DatagramBuf( boost::asio::ip::udp::socket & socket );

    /*!
      \brief create a buffer that sends to the given address.
      \param socket the socket, which must outlive this buffer
      \param dest the address to send to
     */
    DatagramBuf( boost::asio::ip::udp::socket & socket,
                 const boost::asio::ip::udp::endpoint & dest );

    //! send the characters that are left, as rcss::net::SocketStreamBuf does
    ~DatagramBuf() override;

protected:
    int_type overflow( int_type c ) override;
    int sync() override;

private:
    bool writeData();
};

}

#endif
