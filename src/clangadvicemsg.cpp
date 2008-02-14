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
 *   Foundation; either version 2 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#include "clangadvicemsg.h"
#include "types.h"


namespace rcss
{
  namespace clang
  {
    AdviceMsg::AdviceMsg() 
      : Msg()
    {}

    AdviceMsg::AdviceMsg( const Storage& tokens) 
      : Msg(),
        m_tokens( tokens )
    {}

    AdviceMsg::~AdviceMsg()
    {
    	for( Storage::iterator i = m_tokens.begin();
	     i != m_tokens.end(); ++i )
	{
	    delete *i;
	}
	m_tokens.clear();
    }
    
    std::auto_ptr< Msg >
    AdviceMsg::deepCopy() const
    { 
	Storage new_tokens;
	for( Storage::const_iterator i = m_tokens.begin();
	     i != m_tokens.end(); ++i )
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
    
    std::ostream&
    AdviceMsg::print( std::ostream& out ) const
    {
      out << "(advice";
      for( Storage::const_iterator token_iter = getTokens().begin(); 
           token_iter != getTokens().end(); ++token_iter )
        {
          if( *token_iter == NULL )
            out << " (null)";
          else
            out << " " << **token_iter;
        }
      out << ")";
      return out;
    }

    std::ostream&
    AdviceMsg::printPretty( std::ostream& out,
                            const std::string& line_header ) const
    {
      out << line_header << "Advice" << std::endl;
      for( Storage::const_iterator token_iter = getTokens().begin(); 
           token_iter != getTokens().end(); ++token_iter )
        {
          if( *token_iter == NULL )
            out << line_header << " - (null)\n";
          else
            (*token_iter)->printPretty(out, line_header + " - ");
        }
      return out;
    }

    const AdviceMsg::Storage& 
    AdviceMsg::getTokens() const 
    { return m_tokens; }

    AdviceMsg::Storage& 
    AdviceMsg::getTokens()
    { return m_tokens; }
  }
}
