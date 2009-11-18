// -*-c++-*-

/***************************************************************************
                                  clangutil.cc
                       Utility classes for clang messages
                             -------------------
    begin                : 25-FEB-2002
    copyright            : (C) 2002 by The RoboCup Soccer Server
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

#include "clangutil.h"

#include <string>

namespace rcss {
namespace clang {

const char* BallMove::TOKEN_STRINGS[] =
    { "NONE", "p", "d", "c", "s", "ALL" };

const int BallMove::TOKEN_MAX = 4;

void
BallMove::addToken( const BallMoveToken & t )
{
    int i = static_cast< int >( t );
    if ( i < 1 || i > TOKEN_MAX )
    {
        return;
    }

    if ( i == static_cast< int >( BMT_All ) )
    {
        M_entries = ~0;
    }
    else
    {
        M_entries |= ( 1 << (i-1) );
    }
}

void
BallMove::removeToken( const BallMoveToken & t )
{
    int i = static_cast< int >( t );
    if ( i < 1
         || i > TOKEN_MAX )
    {
        return;
    }

    if ( i == static_cast< int >( BMT_All ) )
    {
        clear();
    }
    else
    {
        M_entries &= ~( 1 << (i-1) );
    }
}

bool
BallMove::isMember( const BallMoveToken & t ) const
{
    int i = static_cast< int >( t );
    if ( i < 1 || i > TOKEN_MAX )
    {
        return false;
    }

    if ( i == static_cast< int >( BMT_All ) )
    {
        return (~(((~0) << ((int)TOKEN_MAX)) | M_entries)) == 0;
    }
    else
    {
        return !!(M_entries & (1 << (i-1)));
    }
}

std::ostream &
BallMove::print( std::ostream & out ) const
{
    out << "{";
    for ( int i = 1; i <= BallMove::TOKEN_MAX; i++ )
    {
        if ( isMember( static_cast< BallMoveToken >( i ) ) )
        {
            out << BallMove::TOKEN_STRINGS[ i ];
        }
    }
    out << "}";
    return out;
}


bool
UNumSet::contains( const UNum & unum ) const
{
    if ( unum.isValid() )
    {
        // return std::find( M_entries.begin(), M_entries.end(), unum ) != M_entries.end();
        for ( container::const_iterator i = M_entries.begin();
              i != M_entries.end();
              ++i )
        {
            if ( unum == *i )
            {
                return true;
            }
        }
    }
    return false;
}

}
}
