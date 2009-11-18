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
 *   Foundation; either version 3 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#ifndef XPMHOLDER_H
#define XPMHOLDER_H

#include <string>
#include <vector>
#include <iostream>

class XPMHolder {
private:
    std::vector< std::string > M_data;
    int M_width;
    int M_height;
    int M_colors;

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

    bool valid() const
      {
          return ! M_data.empty();
      }

    int width() const
      {
          return M_width;
      }

    int height() const
      {
          return M_height;
      }

    int colors() const
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
