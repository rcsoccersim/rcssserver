// -*-c++-*-

/***************************************************************************
                               xpmholder.h
                    manages an xpm and it's construction
                             -------------------
    begin                : 21-MAY-2002
    copyright            : (C) 2001 by The RoboCup Soccer Server 
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

#ifndef XPMHOLDER_H
#define XPMHOLDER_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_SSTREAM
#include <sstream>
#else
#include <strstream>
#endif

#include <string>
#include <iostream>

class XPMHolder
{
private:
  char** M_data;
  char* M_str;
  int M_x;
  int M_y;
  int M_colors;
  int M_size;
  bool M_valid;
public:
  XPMHolder( const char* str )
    : M_data( NULL ),
      M_str( strdup( str ) ),
      M_x( 0 ),
      M_y( 0 ),
      M_colors( 0 ),
      M_size( 0 ),
      M_valid( false )
  {
    if( M_str == NULL )
      return;

    char* curr = M_str;
    int idx = 0;
    do
      {
        // advance curr to the begining of the next XPM line
        while( curr && curr[ 0 ] != '"' )
          ++curr;
        if( !curr )
          break;

        ++curr;
        
        // at this point curr point to the begining of a XPM line

        if( idx == 0 )
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
        while( curr && curr[ 0 ] != '"' )
          ++curr;
        
        if( !curr )
          break;
        // terminate the end of the XPM line
        curr[ 0 ] = 0;

        ++curr;
        ++idx;        

        if( idx == M_size )
          {
            M_valid = true;
            break;
          }
      }
    while( curr );

    if( !M_valid )
      clear();
  }

  ~XPMHolder()
  { clear(); }

  void
  clear()
  {
    delete[] M_data;
    free( M_str );
    M_data = NULL;
    M_str = NULL;
  }
  
  char **const
  getXPM() const
  {
    if( valid() )
      return M_data;
    else
      return NULL;
  }

  bool
  valid() const
  { return M_valid; }

  int
  getSize() const
  { return M_size; }

  int
  getWidth() const
  { return M_x; }

  int
  getHeight() const
  { return M_y; }

  int
  getColors() const
  { return M_colors; }

  std::ostream&
  print( std::ostream& o ) const
  {
    if( valid() )
      {
        o << '"' << M_data[ 0 ] << '"';
        for( int i = 1; i < M_size; ++i )
          {
            o << " \"" << M_data[ i ] << '"';
          }
      }
    else
      o << "Invalid";
    return o;
  }
};

inline
std::ostream&
operator<<( std::ostream& o, const XPMHolder& holder )
{ return holder.print( o ); }
#endif
