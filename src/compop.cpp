// -*-c++-*-

/***************************************************************************
                                 compop.cc
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

#include "compop.h"

namespace rcss {
namespace util {

const char* CompOp::STRINGS[] = { "<", "<=", "==", "!=", ">=", ">" };


CompOp::CompOp( const comp_t & comp )
    : M_comp( comp )
{

}

const char*
CompOp::getStr() const
{
    return STRINGS[ M_comp ];
}

const CompOp &
CompOp::swap() const
{
    switch ( M_comp ) {
    case L:
        return greater();
    case LE:
        return greaterEqual();
    case E:
        return equal();
    case NE:
        return notEqual();
    case GE:
        return lessEqual();
    case G:
        return less();
    default:
        // this code should never be executed
        return equal();
    }
}

const CompOp &
CompOp::inverse() const
{
    switch ( M_comp ) {
    case L:
        return greaterEqual();
    case LE:
        return greater();
    case E:
        return notEqual();
    case NE:
        return equal();
    case GE:
        return less();
    case G:
        return lessEqual();
    default:
        // this code should never be executed
        return equal();
    }
}

const CompOp &
CompOp::instance( const comp_t & comp )
{
    // The entire collection of CompOps is created on the first call to
    // CompOp::instance.  Because the array is a static varaible in this
    // function, we can legally return references to its elements.  One
    // of the advatages of doing this is that the CompOps will be destroyed
    // automatically.
    static CompOp instances[] = { L, LE, E, NE, GE, G };
    return instances[ comp ];
}

const CompOp &
CompOp::less()
{
    return instance( L );
}

const CompOp &
CompOp::lessEqual()
{
    return instance( LE );
}

const CompOp &
CompOp::equal()
{
    return instance( E );
}

const CompOp &
CompOp::notEqual()
{
    return instance( NE );
}

const CompOp &
CompOp::greaterEqual()
{
    return instance( GE );
}

const CompOp &
CompOp::greater()
{
    return instance( G );
}


std::ostream &
operator<<( std::ostream & o,
            const CompOp & comp )
{
    return o << comp.getStr();
}

}
}
