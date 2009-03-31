// -*-c++-*-

/***************************************************************************
                                 arithop.h
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

#ifndef ARITHOP_H
#define ARITHOP_H

#include <iostream>

namespace rcss {
namespace util {

// ArithOp is a flyweight class that represents comparision
// operations such as +, -, *, etc.  Access to ArithOp instances
// can only be made via the static member functions such as
// plus(), minus(), etc.  The object returned is immutable and has
// no non-const member functions.

// to apply the operator to two objects, simple call the operate
// member function.  This will return the result of the operation
// on the objects

class ArithOp {
private:
    enum arith_t
        { PLUS, MINUS, MULT, DIV };

    arith_t M_arith;

    ArithOp( const arith_t & arith );

    ArithOp( const ArithOp & arith_op ); // not used


    ArithOp& operator=( const ArithOp& arith_op ); // not used

public:
    template< typename A, typename B >
    A operate( const A & a,
               const B & b ) const
      {
          switch ( M_arith ) {
          case PLUS:
              return a + b;
          case MINUS:
              return a - b;
          case MULT:
              return a * b;
          case DIV:
              return a / b;
          }
      }

    const char * getStr() const;

private:
    static const char * STRINGS[];

    static
    const ArithOp & instance( const arith_t& arith );

public:
    static
    const ArithOp & plus();

    static
    const ArithOp & minus();

    static
    const ArithOp & mult();

    static
    const ArithOp & div();

private:
    friend class ArithOpFriend;
    // this is just to get rid of the compiler warning us that ArithOp has
    // private constructors/destructors
};

inline
std::ostream &
operator<<( std::ostream & o,
            const ArithOp & arith )
{
    return o << arith.getStr();
}

}
}

#endif
