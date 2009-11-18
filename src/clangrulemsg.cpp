// -*-c++-*-

/***************************************************************************
                                clangrulemsg.cc
                       Class for CLang Rule messages
                             -------------------
    begin                : 28-MAY-2002
    copyright            : (C) 2002 by The RoboCup Soccer Server
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

#include "clangrulemsg.h"

#include "rule.h"
#include "types.h"

namespace rcss {
namespace clang {

RuleMsg::RuleMsg()
    : Msg()
{

}

RuleMsg::RuleMsg( const Storage & list )
	: Msg(),
	  M_active( list )
{

}

RuleMsg::~RuleMsg()
{

}

std::auto_ptr< Msg >
RuleMsg::deepCopy() const
{
	return std::auto_ptr< Msg >( new RuleMsg( *this ) );
}

std::ostream &
RuleMsg::print( std::ostream & out ) const
{
    out << "(rule ";
    for ( Storage::const_iterator i = M_active.begin();
          i != M_active.end();
          ++i )
    {
        out << *i;
    }
    out << ")";
    return out;
}

std::ostream &
RuleMsg::printPretty( std::ostream & out,
                      const std::string & line_header ) const
{
    out << line_header << "Activation List:\n";
    bool first = true;
    for ( Storage::const_iterator i = M_active.begin();
          i != M_active.end();
          ++i )
    {
        if ( first )
        {
            first = false;
        }
        else
        {
            out << line_header << " Then:\n";
        }
        i->printPretty( out, line_header + "  -" );
    }
    return out;
}

}
}
