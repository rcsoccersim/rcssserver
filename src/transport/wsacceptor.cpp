// -*-c++-*-


/*!
  \file wsacceptor.cpp
  \brief The acceptor implementation over WebSocket
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

#include "wsacceptor.h"

#include "wsserver.h"

namespace rcss {

/*-------------------------------------------------------------------*/
WSAcceptor::WSAcceptor( const std::shared_ptr< WSServer > & server,
                        const std::string & path )
    : M_server( server ),
      M_path( path )
{

}

/*-------------------------------------------------------------------*/
WSAcceptor::~WSAcceptor()
{

}

/*-------------------------------------------------------------------*/
bool
WSAcceptor::popInbound( Inbound & in )
{
    return M_server->popInbound( M_path, in );
}

/*-------------------------------------------------------------------*/
std::shared_ptr< Connection >
WSAcceptor::accept( const Inbound & in )
{
    return M_server->accept( M_path, in );
}

}
