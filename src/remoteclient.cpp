// -*-c++-*-

/***************************************************************************
                             remoteclient.cpp
              Classes for building visual messages for coaches
                             -------------------
    begin                : 2008-02-21
    copyright            : (C) 2008 by The RoboCup Soccer Server
                           Maintenance Group.
    email                : sserver-admin@lists.sourceforge.net
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LGPL as published by the Free Software  *
 *   Foundation; either version 3 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "remoteclient.h"

#include "param.h"
#include "rcssexceptions.h"

#include <rcssbase/net/socketstreambuf.hpp>
#include <rcssbase/gzip/gzstream.hpp>

#include <cerrno>
#include <cstring>


RemoteClient::RemoteClient()
    : M_socket()
    , M_socket_buf( NULL )
    , M_gz_buf( NULL )
    , M_transport( NULL )
    , M_comp_level( -1 )
    , M_enforce_dedicated_port( false )
{
    open();
}


RemoteClient::~RemoteClient()
{
    close();
}

std::ostream &
RemoteClient::getTransport()
{
    if ( M_transport )
    {
        return *M_transport;
    }
    else
    {
        throw rcss::util::NullErr( __FILE__, __LINE__, "Transport is NULL" );
    }
}

void
RemoteClient::close()
{
    M_socket.close();

    if ( M_transport )
    {
        delete M_transport;
        M_transport = NULL;
    }

    if ( M_gz_buf )
    {
        delete M_gz_buf;
        M_gz_buf = NULL;
    }

    if ( M_socket_buf )
    {
        delete M_socket_buf;
        M_socket_buf = NULL;
    }

    setEnforceDedicatedPort( false );
}

bool
RemoteClient::connect( const rcss::net::Addr & dest )
{
    if ( ! M_socket.connect( dest ) )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << ": Error connecting socket" << std::endl;
        M_socket.close();
        return false;
    }
    return true;
}

int
RemoteClient::open()
{
    if ( M_socket.open() )
    {
        if ( M_socket.setNonBlocking() < 0 )
        {
            std::cerr << __FILE__ << ": " << __LINE__
                      << ": Error setting socket non-blocking: "
                      << strerror( errno ) << std::endl;
            M_socket.close();
            return -1;
        }
    }
    else
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << ": Error opening socket: "
                  << strerror( errno ) << std::endl;
        M_socket.close();
        return -1;
    }

    M_socket_buf = new rcss::net::SocketStreamBuf( M_socket );
    M_transport = new std::ostream( M_socket_buf );
    //M_transport->setLevel( M_comp_level );
    return 0;
}

int
RemoteClient::send( const char * msg,
                    const size_t & len )
{
    if ( M_socket.isConnected() )
    {
        M_transport->write( msg, len );
        M_transport->flush();
        if ( ! M_transport->good() )
        {
            if ( errno != ECONNREFUSED )
            {
                std::cerr << __FILE__ << ": " << __LINE__
                          << ": Error sending to socket: "
                          << strerror( errno ) << std::endl
                          << "msg = [" << msg << "]\n";
            }
            close();
        }

        return len;
    }
    return -1;
}

int
RemoteClient::recv()
{
    if ( M_socket.isConnected() )
    {
        char buffer[ MaxMesg ];
        std::memset( &buffer, 0, sizeof( char ) * MaxMesg );

        size_t len = MaxMesg;
        int ret = M_socket.recv( buffer, len );

        if ( ret == -1 && errno != EWOULDBLOCK )
        {
            if ( errno != ECONNREFUSED )
            {
                std::cerr << __FILE__ << ": " << __LINE__
                          << ": Error receiving from socket: "
                          << strerror( errno ) << std::endl;
            }
            close();
        }
        else if ( ret > 0 )
        {
            processMsg( buffer, ret );
        }
        return ret;
    }
    return -1;
}

void
RemoteClient::undedicatedRecv( char * msg,
                               const size_t & len )
{
    if ( M_enforce_dedicated_port )
    {
        char err_msg[] = "(error only_init_allowed_on_init_port)";
        send( err_msg, std::strlen( err_msg ) + 1 );
    }
    else
    {
        processMsg( msg, len );
    }
}

void
RemoteClient::processMsg( char * msg,
                          const size_t & len )
{
#ifdef HAVE_LIBZ
    if ( M_comp_level >= 0 )
    {
        M_decomp.decompress( msg, len, Z_SYNC_FLUSH );
        char * out;
        int size;
        M_decomp.getOutput( out, size );
        if ( size > 0 )
        {
            parseMsg( out, size );
        }
    }
    else
#endif
    {
        parseMsg( msg, len );
    }
}

int
RemoteClient::setCompressionLevel( const int level )
{
#ifdef HAVE_LIBZ
    //M_transport->setLevel( level );

    if ( level >= 0 )
    {
        if ( ! M_gz_buf )
        {
            M_gz_buf = new rcss::gz::gzstreambuf( *M_socket_buf );
        }
        M_gz_buf->setLevel( level );
        M_transport->rdbuf( M_gz_buf );
    }
    else
    {
        M_transport->rdbuf( M_socket_buf );
    }
//     if ( level < 0 )
//     {
//         M_comp.reset();
//     }
//     else
//     {
//         M_comp.setCompression( level );
//     }

    return M_comp_level = level;
#endif
    return M_comp_level = -1;
}
