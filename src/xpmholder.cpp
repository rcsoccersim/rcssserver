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
#include <config.h>
#endif

#include "xpmholder.h"

#ifdef HAVE_SSTREAM
#include <sstream>
#else
#include <strstream>
#endif

#include <cstdio>
#include <cstring>
#include <cstdlib>

XPMHolder::XPMHolder( const char * str )
    : M_width( 0 ),
      M_height( 0 ),
      M_colors( 0 )
{
    const char * cur = str;

    while ( cur && *cur != '"' )
    {
        ++cur;
    }

    if ( ! cur )
    {
        // std::cerr << "XPMHolder: No data" << std::endl;
        return;
    }

    int n_read = 0;
    char buf[8192];

    //
    // header
    //
    int xpm_cpp = 0;
    n_read = 0;
    if ( std::sscanf( cur,
                      " \" %d %d %d %d \" %n ",
                      &M_width, &M_height, &M_colors, &xpm_cpp,
                      &n_read ) != 4
         || n_read == 0 )
    {
        // std::cerr << "XPMHolder: Illegal xpm header [" << std::string( cur, 0, 16 ) << "]"
        //           << std::endl;
        clear();
        return;
    }
    cur += n_read;

    M_data.reserve( 1 + M_colors + M_height );

    snprintf( buf, 127, "%d %d %d %d", M_width, M_height, M_colors, xpm_cpp );
    M_data.push_back( buf );

    for ( int i = 0; i < M_colors; ++i )
    {
        n_read = 0;
        if ( std::sscanf( cur, " \"%8191[^\"]\" %n ",
                          buf, &n_read ) != 1 )
        {
            // std::cerr << "XPMHolder: Illegal color data"
            //           << std::endl;
            clear();
            return;
        }
        cur += n_read;

        M_data.push_back( buf );
    }

    const size_t line_size = static_cast< size_t >( M_width * xpm_cpp );
    for ( int i = 0; i < M_height; ++i )
    {
        n_read = 0;
        if ( std::sscanf( cur, " \"%8191[^\"]\" %n ",
                          buf, &n_read ) != 1 )
        {
            // std::cerr << "XPMHolder: Illegal pixel data line"
            //           << std::endl;
            clear();
            return;
        }
        cur += n_read;

        if ( std::strlen( buf ) != line_size )
        {
            // std::cerr << "XPMHolder: Illegal width" << std::endl;
            clear();
            return;
        }

        M_data.push_back( buf );
    }
}


void
XPMHolder::clear()
{
    M_data.clear();
    M_width = 0;
    M_height = 0;
    M_colors = 0;
}


std::ostream &
XPMHolder::print( std::ostream & o ) const
{
    if ( ! M_data.empty() )
    {
        std::vector< std::string >::const_iterator it = M_data.begin();
        o << '"' << *it << '"';
        ++it;
        for ( ; it != M_data.end(); ++it )
        {
            o << " \"" << *it << '"';
        }
    }
    return o;
}
