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
//#include "rcssexceptions.h"

#include <rcss/gzip/gzstream.hpp>

#include <algorithm>
#include <cerrno>
#include <cstring>
#include <iostream>


RemoteClient::RemoteClient()
    : M_connection()
    , M_gz_buf( nullptr )
    , M_transport( nullptr )
    , M_comp_level( -1 )
    , M_enforce_dedicated_port( false )
{

}


RemoteClient::~RemoteClient()
{
    close();
}

std::ostream &
RemoteClient::getTransport()
{
    return M_transport;
}

void
RemoteClient::close()
{
    if ( M_connection )
    {
        M_connection->close();
    }

    M_transport.rdbuf( nullptr );

    // The gzip buffer may flush into the connection's buffer when it is
    // deleted, so delete it before releasing the connection.
    if ( M_gz_buf )
    {
        delete M_gz_buf;
        M_gz_buf = nullptr;
    }

    M_connection.reset();

    setEnforceDedicatedPort( false );
}

void
RemoteClient::attach( std::shared_ptr< rcss::Connection > connection )
{
    close();

    M_connection = connection;
    if ( M_connection )
    {
        // rdbuf() also clears the error state of the stream.
        M_transport.rdbuf( &M_connection->outbuf() );
    }
}

int
RemoteClient::send( const char * msg,
                    const size_t & len )
{
    if ( connected() )
    {
        M_transport.write( msg, len );
        M_transport.flush();
        if ( ! M_transport.good() )
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
    if ( ! connected() )
    {
        return -1;
    }

    std::string msg;
    if ( ! M_connection->popMessage( msg ) )
    {
        if ( ! M_connection->isOpen() )
        {
            // the connection has been closed because of an error
            close();
        }
        return -1;
    }

    const size_t len = std::min( msg.size(), static_cast< size_t >( MaxMesg ) );
    if ( len > 0 )
    {
        // The parsers write a null character at msg[len], so the buffer needs
        // one byte more than MaxMesg. The rest is zero-filled as before.
        char buffer[ MaxMesg + 1 ];
        std::memset( buffer, 0, sizeof( buffer ) );
        std::memcpy( buffer, msg.data(), len );
        processMsg( buffer, len );
    }

    return static_cast< int >( len );
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

    if ( M_connection )
    {
        if ( level >= 0 )
        {
            if ( ! M_gz_buf )
            {
                M_gz_buf = new rcss::gz::gzstreambuf( M_connection->outbuf() );
            }
            M_gz_buf->setLevel( level );
            M_transport.rdbuf( M_gz_buf );
        }
        else
        {
            M_transport.rdbuf( &M_connection->outbuf() );
        }
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
