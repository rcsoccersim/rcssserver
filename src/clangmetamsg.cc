// -*-c++-*-

/***************************************************************************
                                clangmetamsg.cc 
                       Class for CLang Meta messages
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

#include "clangmetamsg.h"
#include "types.h"
#include <iostream>
#include <string>

namespace rcss
{
  namespace clang
  {
//     void
//     MetaToken::TypeExtractor::visit( MetaTokenVer* tok )
//     { setValue( tok ); }

//     void
//     MetaToken::ConstTypeExtractor::visit( const MetaTokenVer* tok )
//     { setValue( tok ); }
 
    MetaToken::MetaToken()
    {}
  
    MetaToken::~MetaToken()
    {}

    MetaTokenVer::MetaTokenVer( const double& ver ) 
	: MetaToken(),
	  m_ver( ver )
    {}

    MetaTokenVer::~MetaTokenVer()
    {}

    std::auto_ptr< MetaToken >
    MetaTokenVer::deepCopy() const
    { return std::auto_ptr< MetaToken >( new MetaTokenVer( *this ) ); } 

//     void
//     MetaTokenVer::accept( Visitor& v )
//     { v.startVisit( this ); }
    
//     void
//     MetaTokenVer::accept( ConstVisitor& v ) const
//     { v.startVisit( this ); }
    
    std::ostream& 
    MetaTokenVer::print( std::ostream& out ) const
    { return out << "(ver " << m_ver << ")"; }
    
    std::ostream&
    MetaTokenVer::printPretty( std::ostream& out, const std::string& line_header ) const
    { return out << line_header << "version: " << m_ver << std::endl; }
    
    double
    MetaTokenVer::getVer() const 
    { return m_ver; }




    MetaMsg::MetaMsg() 
      : Msg()
    {}
      
    MetaMsg::MetaMsg( const Storage& tokens ) 
	: Msg(),
	  m_tokens( tokens )
    {
    }

    MetaMsg::~MetaMsg()
    {
	for( Storage::const_iterator i = m_tokens.begin();
	     i != m_tokens.end(); ++i )
	{
	    delete *i;
	}
	m_tokens.clear();
    }
      
    std::auto_ptr< Msg >
    MetaMsg::deepCopy() const
    {
	Storage new_tokens;
	for( Storage::const_iterator i = m_tokens.begin();
	     i != m_tokens.end(); ++i )
	{
	    new_tokens.push_back( (*i)->deepCopy().release() );
	}
	return std::auto_ptr< Msg >( new MetaMsg( new_tokens ) ); 
    }

//     void
//     MetaMsg::accept( Visitor& v )
//     { v.startVisit( this ); }

//     void
//     MetaMsg::accept( ConstVisitor& v ) const
//     { v.startVisit( this ); }

    std::ostream& 
    MetaMsg::print( std::ostream& out ) const
    {
      out << "(meta";
      for( Storage::const_iterator token_iter = m_tokens.begin();
           token_iter != m_tokens.end();
           ++token_iter )
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
    MetaMsg::printPretty( std::ostream& out, const std::string& line_header ) const
    {
      out << line_header << "Meta" << std::endl;

      for( Storage::const_iterator token_iter = m_tokens.begin();
           token_iter != m_tokens.end(); ++token_iter) 
        {
          if( *token_iter == NULL )
            out << line_header << " - (null)\n";
          else
            (*token_iter)->printPretty( out, line_header + " - " );
        }
      return out;
    }

    MetaMsg::Storage&
    MetaMsg::getTokens()
    { return m_tokens; }

    const MetaMsg::Storage&
    MetaMsg::getTokens() const
    { return m_tokens; }
  }
}
