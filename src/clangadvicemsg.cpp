// -*-c++-*-

/***************************************************************************
                                clangadvicemsg.cc
                       Class for CLang Advice messages
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

#include "clangadvicemsg.h"
#include "types.h"

namespace rcss {
namespace clang {

AdviceMsg::AdviceMsg()
    : Msg()
{

}

AdviceMsg::AdviceMsg( const Storage & tokens )
    : Msg(),
      M_tokens( tokens )
{

}

AdviceMsg::~AdviceMsg()
{
    for ( Storage::iterator i = M_tokens.begin();
          i != M_tokens.end();
          ++i )
	{
	    delete *i;
	}
	M_tokens.clear();
}

std::auto_ptr< Msg >
AdviceMsg::deepCopy() const
{
	Storage new_tokens;
	for( Storage::const_iterator i = M_tokens.begin();
	     i != M_tokens.end(); ++i )
	{
	    new_tokens.push_back( (*i)->deepCopy().release() );
	}
	return std::auto_ptr< Msg >( new AdviceMsg( new_tokens ) );
}

//     void
//     AdviceMsg::accept( Visitor& v )
//     { v.startVisit( this ); }

//     void
//     AdviceMsg::accept( ConstVisitor& v ) const
//     { v.startVisit( this ); }

std::ostream &
AdviceMsg::print( std::ostream & out ) const
{
    out << "(advice";
    for ( Storage::const_iterator token_iter = getTokens().begin();
          token_iter != getTokens().end();
          ++token_iter )
    {
        if ( *token_iter == NULL )
        {
            out << " (null)";
        }
        else
        {
            out << " " << **token_iter;
        }
    }
    out << ")";
    return out;
}

std::ostream &
AdviceMsg::printPretty( std::ostream & out,
                        const std::string & line_header ) const
{
    out << line_header << "Advice" << std::endl;
    for ( Storage::const_iterator token_iter = getTokens().begin();
          token_iter != getTokens().end();
          ++token_iter )
    {
        if ( *token_iter == NULL )
        {
            out << line_header << " - (null)\n";
        }
        else
        {
            (*token_iter)->printPretty( out, line_header + " - " );
        }
    }
    return out;
}

}
}
