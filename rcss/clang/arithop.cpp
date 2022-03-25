// -*-c++-*-

/***************************************************************************
                                 arithop.cpp
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "arithop.h"

namespace rcss {
namespace util {

const char * ArithOp::STRINGS[] = { "+", "-", "*", "/" };

ArithOp::ArithOp( const arith_t & arith )
    : M_arith( arith )
{

}

const char *
ArithOp::getStr() const
{
    return STRINGS[ M_arith ];
}

const ArithOp &
ArithOp::instance( const arith_t & arith )
{
    // The entire collection of ArithOps is created on the first
    // call to ArithOp::instance.  Because the array is a static
    // varaible in this function, we can legally return references
    // to its elements.  One of the advatages of doing this is
    // that the ArithOps will be destroyed automatically.
    static ArithOp instances[] = { ArithOp( PLUS ),
                                   ArithOp( MINUS ),
                                   ArithOp( MULT ),
                                   ArithOp( DIV ) };
    return instances[ arith ];
}

const ArithOp &
ArithOp::plus()
{
    return instance( PLUS );
}

const ArithOp &
ArithOp::minus()
{
    return instance( MINUS );
}

const ArithOp &
ArithOp::mult()
{
    return instance( MULT );
}

const ArithOp &
ArithOp::div()
{
    return instance( DIV );
}

}
}
