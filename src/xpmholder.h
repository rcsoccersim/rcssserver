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

#include <string>
#include <iostream>

class XPMHolder {
private:
    char ** M_data;
    char * M_str;
    int M_x;
    int M_y;
    int M_colors;
    int M_size;
    bool M_valid;

    XPMHolder(); // not used
    XPMHolder( const XPMHolder & ); // not used
    const XPMHolder & operator=( const XPMHolder & ); // not used
public:

    explicit
    XPMHolder( const char * str );

    ~XPMHolder()
      {
          clear();
      }

    void clear();

    char ** const getXPM() const
      {
          return ( valid()
                   ? M_data
                   : NULL );
      }

    bool valid() const
      {
          return M_valid;
      }

    int getSize() const
      {
          return M_size;
      }

    int getWidth() const
      {
          return M_x;
      }

    int getHeight() const
      {
          return M_y;
      }

    int getColors() const
      {
          return M_colors;
      }

    std::ostream & print( std::ostream & o ) const;

};

inline
std::ostream &
operator<<( std::ostream & o,
            const XPMHolder & holder )
{
    return holder.print( o );
}

#endif
