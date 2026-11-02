// -*-c++-*-


/*!
  \file messagebuf.cpp
  \brief The stream buffer that collects one outgoing message
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

#include "messagebuf.h"

namespace rcss {

/*-------------------------------------------------------------------*/
MessageBuf::MessageBuf( const Sender & sender )
    : M_sender( sender ),
      M_message()
{

}

/*-------------------------------------------------------------------*/
MessageBuf::int_type
MessageBuf::overflow( int_type c )
{
    // There is no put area, so every character comes here or to xsputn().
    if ( ! traits_type::eq_int_type( c, traits_type::eof() ) )
    {
        M_message.push_back( traits_type::to_char_type( c ) );
    }
    return traits_type::not_eof( c );
}

/*-------------------------------------------------------------------*/
std::streamsize
MessageBuf::xsputn( const char_type * s,
                    std::streamsize n )
{
    M_message.append( s, n );
    return n;
}

/*-------------------------------------------------------------------*/
int
MessageBuf::sync()
{
    if ( ! M_message.empty() )
    {
        M_sender( M_message );
        M_message.clear();
    }

    // Errors are not reported to the stream. A failed connection is
    // detected through Connection::isOpen() instead.
    return 0;
}

}
