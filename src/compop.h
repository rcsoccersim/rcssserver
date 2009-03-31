// -*-c++-*-

/***************************************************************************
                                 compop.h
                  Flyweight classes representing comparison operators
                             -------------------
    begin                : 22-MAR-2002
    copyright            : (C) 2002 by The RoboCup Soccer Server
                           Maintainance Group.
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

#ifndef COMPOP_H
#define COMPOP_H

#include <iostream>

namespace rcss {
namespace util {

// CompOp is a flyweight class that represents comparision
// operations such as less than, equal, etc.  Access to CompOp
// instances can only be made via the static member functions such
// as less(), equal(), etc.  The object returned is immutable and
// has no non-const member functions.

// You can compare two objects using a CompOp object and the
// compare() member function.

// You can get the swap of the comparison by calling the swap()
// member function.  As an example this is used is you want to
// convert a < b to b > a.  Swap does not modify the current
// CompOp object, it just returns a reference to it's swaped equivilant.

// CompOp also has an inverse() member function. As and example,
// inverse() can be used when you want to convert something like a
// < b to a >= b.  Like swap(), inverse() does not modify the
// CompOp object, it simply returns a const reference to the
// appropriate inverse.

class CompOp {
private:
    enum comp_t
        { L, LE, E, NE, GE, G };

    comp_t M_comp;

    CompOp( const comp_t & comp );

    CompOp( const CompOp & comp_op ); // not used


    CompOp& operator=( const CompOp & comp_op ); // not used
public:
    template< typename A, typename B >
    bool compare( const A & a,
                  const B & b ) const
      {
          switch ( M_comp ) {
          case L:
              return a < b;
          case LE:
              return a <= b;
          case E:
              return a == b;
          case NE:
              return a != b;
          case GE:
              return a >= b;
          case G:
              return a > b;
          }
      }

    const char * getStr() const;

    const CompOp & swap() const;

    const CompOp & inverse() const;

private:
    static const char * STRINGS[];

    static
    const CompOp & instance( const comp_t & comp );

public:
    static
    const CompOp & less();

    static
    const CompOp & lessEqual();

    static
    const CompOp & equal();

    static
    const CompOp & notEqual();

    static
    const CompOp & greaterEqual();

    static
    const CompOp & greater();

private:
    friend class CompOpFriend;
    // this is just to get rid of the compiler warning us that compop has
    // private constructors/destructors
};

std::ostream &
operator<<( std::ostream & o,
            const CompOp & comp );
}

}
#endif
