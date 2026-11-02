// -*-c++-*-


/*!
  \file pendingconnections.cpp
  \brief The connections whose clients have not been accepted
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

#include "pendingconnections.h"

#include <algorithm>

namespace rcss {

const std::chrono::seconds PendingConnections::TIMEOUT( 10 );
const std::size_t PendingConnections::MAX_SIZE = 64;

/*-------------------------------------------------------------------*/
PendingConnections::PendingConnections()
    : M_entries()
{

}

/*-------------------------------------------------------------------*/
PendingConnections::~PendingConnections()
{
    closeAll();
}

/*-------------------------------------------------------------------*/
void
PendingConnections::add( const std::shared_ptr< Connection > & connection,
                         const std::chrono::steady_clock::time_point & since )
{
    Entry e;
    e.connection = connection;
    e.since = since;
    M_entries.push_back( e );
}

/*-------------------------------------------------------------------*/
bool
PendingConnections::popInbound( Inbound & in )
{
    const std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();

    // forget the connections that are closed or have waited too long
    M_entries.erase( std::remove_if( M_entries.begin(),
                                     M_entries.end(),
                                     [&now]( const Entry & e )
                                       {
                                           if ( now - e.since > TIMEOUT )
                                           {
                                               e.connection->close();
                                           }
                                           return ! e.connection->isOpen();
                                       } ),
                     M_entries.end() );

    for ( Entry & e : M_entries )
    {
        std::string msg;
        if ( e.connection->popMessage( msg ) )
        {
            in.message = std::move( msg );
            in.reply = e.connection;
            in.known = nullptr;
            return true;
        }
    }

    return false;
}

/*-------------------------------------------------------------------*/
std::shared_ptr< Connection >
PendingConnections::accept( const Inbound & in )
{
    std::vector< Entry >::iterator it
        = std::find_if( M_entries.begin(),
                        M_entries.end(),
                        [&in]( const Entry & e )
                          {
                              return e.connection == in.reply;
                          } );
    if ( it == M_entries.end() )
    {
        return std::shared_ptr< Connection >();
    }

    std::shared_ptr< Connection > connection = it->connection;
    M_entries.erase( it );
    return connection;
}

/*-------------------------------------------------------------------*/
void
PendingConnections::closeAll()
{
    for ( Entry & e : M_entries )
    {
        e.connection->close();
    }
    M_entries.clear();
}

}
