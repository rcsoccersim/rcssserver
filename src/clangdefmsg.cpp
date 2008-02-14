// -*-c++-*-

/***************************************************************************
                                clangdefmsg.cc  
                       Class for CLang Define messages
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

#include "clangdefmsg.h"
#include "types.h"

namespace rcss
{
  namespace clang
  {
    DefineMsg::DefineMsg() 
      : Msg()
    {}
    
    DefineMsg::DefineMsg( const Storage& defs ) 
      : Msg(),
        m_defs( defs )
    {}
    
    DefineMsg::~DefineMsg()
    {
	for( Storage::iterator i = m_defs.begin();
	     i != m_defs.end(); ++i )
	{
	    delete *i;
	}
	m_defs.clear();

    }
    
    std::auto_ptr< Msg >
    DefineMsg::deepCopy() const
    {
	Storage new_defs;
	for( Storage::const_iterator i = m_defs.begin();
	     i != m_defs.end(); ++i )
	    new_defs.push_back( (*i)->deepCopy().release() );
	return std::auto_ptr< Msg >( new DefineMsg( new_defs ) ); 
    }
    
//     void
//     DefineMsg::accept( Visitor& v )
//     { v.startVisit( this ); }
    
//     void
//     DefineMsg::accept( ConstVisitor& v ) const
//     { v.startVisit( this ); }
    
    std::ostream&
    DefineMsg::print( std::ostream& out ) const
    {
      out << "(define";
      for( Storage::const_iterator token_iter = getDefs().begin(); 
           token_iter != getDefs().end(); ++token_iter )
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
    DefineMsg::printPretty( std::ostream& out,
                            const std::string& line_header ) const
    {
      out << line_header << "Define" << std::endl;
      for( Storage::const_iterator token_iter = getDefs().begin(); 
           token_iter != getDefs().end(); ++token_iter )
        {
          if( *token_iter == NULL )
            out << line_header << " - (null)\n";
          else
            (*token_iter)->printPretty( out, line_header + " - " );
        }
      return out;
    }

    const DefineMsg::Storage& 
    DefineMsg::getDefs() const
    { return m_defs; }

    DefineMsg::Storage& 
    DefineMsg::getDefs()
    { return m_defs; }
  }
}
