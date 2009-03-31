// -*-c++-*-

/***************************************************************************
                               xpmholder.h
                    manages an xpm and it's construction
                             -------------------
    begin                : 2008-02-15
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
#include "config.h"
#endif

#include "xpmholder.h"

#ifdef HAVE_SSTREAM
#include <sstream>
#else
#include <strstream>
#endif

#include <cstring>
#include <cstdlib>

XPMHolder::XPMHolder( const char * str )
    : M_data( NULL ),
      M_str( strdup( str ) ),
      M_x( 0 ),
      M_y( 0 ),
      M_colors( 0 ),
      M_size( 0 ),
      M_valid( false )
{
    if ( M_str == NULL )
    {
        return;
    }

    char * curr = M_str;
    int idx = 0;
    do
    {
        // advance curr to the begining of the next XPM line
        while ( curr && curr[ 0 ] != '"' )
        {
            ++curr;
        }

        if ( ! curr )
        {
            break;
        }

        ++curr;

        // at this point curr point to the begining of a XPM line

        if ( idx == 0 )
        {
            // alocate M_data
#ifdef HAVE_SSTREAM
            std::istringstream strm( curr );
#else
            std::istrstream strm( curr );
#endif
            strm >> M_x;
            strm >> M_y;
            M_size += M_y;
            strm >> M_colors;
            M_size += M_colors;
            ++M_size;
            M_data = new char*[ M_size ];
        }
        M_data[ idx ] = curr;

        // advance curr to the end of the current XPM line
        while ( curr && curr[ 0 ] != '"' )
        {
            ++curr;
        }

        if ( ! curr )
        {
            break;
        }

        // terminate the end of the XPM line
        curr[ 0 ] = 0;

        ++curr;
        ++idx;

        if ( idx == M_size )
        {
            M_valid = true;
            break;
        }
    }
    while ( curr );

    if ( ! M_valid )
    {
        clear();
    }
}


void
XPMHolder::clear()
{
    delete[] M_data;
    free( M_str );
    M_data = NULL;
    M_str = NULL;
}


std::ostream &
XPMHolder::print( std::ostream & o ) const
{
    if ( valid() )
    {
        o << '"' << M_data[ 0 ] << '"';
        for( int i = 1; i < M_size; ++i )
        {
            o << " \"" << M_data[ i ] << '"';
        }
    }
    else
    {
        o << "Invalid";
    }
    return o;
}
