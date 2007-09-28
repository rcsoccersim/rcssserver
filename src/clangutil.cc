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
 *   Foundation; either version 2 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#include "clangutil.h"

#include <string>

namespace rcss
{
  namespace clang
  {    
    const char* BallMove::TOKEN_STRINGS[] =
    { "NONE", "p", "d", "c", "s", "ALL" };

    const int BallMove::TOKEN_MAX = 4;

    void 
    BallMove::addToken( const BallMoveToken& t )
    {
      int i = (int)t;
      if (i < 1 || i > TOKEN_MAX)
        return;
      if (i==(int)BMT_All) {
        M_entries = ~0;
      } else {
        M_entries |= (1 << (i-1));
      }
    }

    void
    BallMove::removeToken( const BallMoveToken& t )
    {
      int i = (int)t;
      if (i < 1 || i > TOKEN_MAX)
        return;
      if (i==(int)BMT_All) {
        clear();
      } else {
        M_entries &= ~(1 << (i-1));
      }
    }

    bool 
    BallMove::isMember( const BallMoveToken& t ) const
    {
      int i = (int)t;
      if (i < 1 || i > TOKEN_MAX)
        return false;
      if (i==BMT_All) {
        return (~(((~0) << ((int)TOKEN_MAX)) | M_entries)) == 0;
      } else {
        return !!(M_entries & (1 << (i-1)));
      }
    }

    std::ostream&
    BallMove::print( std::ostream& out ) const
    {
      out << "{";
      for( int i = 1; i <= BallMove::TOKEN_MAX; i++ )
        if( isMember( (BallMoveToken)i ) )
          out << BallMove::TOKEN_STRINGS[ i ];
      out << "}";
      return out;
    }


  }
}
