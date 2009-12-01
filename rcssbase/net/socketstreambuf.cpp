// -*-c++-*-

/***************************************************************************
                 socketstreambuf.cpp  -  A socket stream buffer
                             -------------------
    begin                : 2009-12-01
    copyright            : (C) 2009 by The RoboCup Soccer Server
                           Maintenance Group.
    email                : sserver-admin@lists.sourceforge.net
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LGPL as published by the Free Software  *
 *   Foundation; either version 2 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "socketstreambuf.hpp"

#include "socket.hpp"

namespace rcss {
namespace net {

SocketStreamBuf::SocketStreamBuf( Socket & socket,
                                  const Addr & dest,
                                  ConnType conn,
                                  std::streamsize bufsize )
    : M_socket( socket ),
      M_end_point( dest ),
      M_bufsize( bufsize ),
      M_inbuf( NULL ),
      M_outbuf( NULL ),
      M_remained( 0 ),
      M_connect( conn )
{
    M_outbuf = new char_type[M_bufsize];
    setp( M_outbuf, M_outbuf + M_bufsize );
}



SocketStreamBuf::SocketStreamBuf( Socket & socket,
                                  ConnType conn,
                                  std::streamsize bufsize )
    : M_socket( socket ),
      M_bufsize( bufsize ),
      M_inbuf( NULL ),
      M_outbuf( NULL ),
      M_remained( 0 ),
      M_connect( conn )
{
    M_outbuf = new char_type[M_bufsize];
    setp( M_outbuf, M_outbuf + M_bufsize );
}


SocketStreamBuf::~SocketStreamBuf()
{
    overflow();
    if ( M_inbuf ) delete [] M_inbuf;
    if ( M_outbuf ) delete [] M_outbuf;
}



bool
SocketStreamBuf::writeData()
{
    unsigned long size = (unsigned long)( (pptr() - M_outbuf ) * sizeof( char_type ) );
    if ( size == 0 )
    {
        return true;
    }

    if ( M_socket.isConnected() )
    {
        return M_socket.send( M_outbuf, size ) > 0;
    }
    else
    {
        return M_socket.send( M_outbuf, size, M_end_point ) > 0;
    }
}

//    virtual
//    std::streamsize xsputn( const char_type * s,
//                            std::streamsize n )
//      {
//          const std::streamsize len = std::min( n, epptr() - pptr() );
//          traits_type::copy( pptr(), s, len );
//          pbump( len );
//          return len;
//      }


SocketStreamBuf::int_type
SocketStreamBuf::overflow( int_type c )
{
    // this method is supposed to flush the put area of the buffer
    // to the I/O device

    // if the buffer was not already allocated nor set by user,
    // do it just now
    if ( ! writeData() )
    {
        return traits_type::eof();
    }

    setp( M_outbuf, M_outbuf + M_bufsize );
    if ( c != traits_type::eof() )
    {
        sputc( c );
    }
    return 0;
}


int
SocketStreamBuf::sync()
{
    // just flush the put area
    if ( ! writeData() )
    {
        return traits_type::eof();
    }

    setp( M_outbuf, M_outbuf + M_bufsize );
    return 0;
}


SocketStreamBuf::int_type
SocketStreamBuf::underflow()
{
    // this method is supposed to read some bytes from the I/O device

    // if the buffer was not already allocated nor set by user,
    // do it just now
    if ( gptr() == NULL )
    {
        M_inbuf = new char_type[M_bufsize];
    }

    if ( M_remained != 0 )
    {
        M_inbuf[0] = M_remained_char;
    }

    int readn = M_bufsize * sizeof( char_type ) - M_remained;
    if ( M_socket.isConnected() )
    {
        readn = M_socket.recv( M_inbuf + M_remained,
                               readn );
    }
    else
    {
        Addr addr;
        readn = M_socket.recv( M_inbuf + M_remained,
                               readn,
                               addr );
        if ( M_connect == CONN_ON_READ && readn > 0 )
        {
            M_socket.connect( addr );
        }
    }

    if ( readn < 0
         || ( readn == 1 && (M_inbuf + M_remained)[ 0 ] == -1 ) )
    {
        (M_inbuf + M_remained)[ 0 ] = -1;
        return traits_type::eof();
    }

    int totalbytes = readn + M_remained;
    setg( M_inbuf, M_inbuf,
          M_inbuf + totalbytes / sizeof( char_type ) );

    M_remained = totalbytes % sizeof( char_type );
    if ( M_remained != 0 )
    {
        M_remained_char = M_inbuf[totalbytes / sizeof(char_type)];
    }

    return sgetc();
}

}
}
