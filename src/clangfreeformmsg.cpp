// -*-c++-*-

/***************************************************************************
                                clangfreeformmsg.h  
                       Class for CLang Freeform messages
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

#include "clangfreeformmsg.h"
#include "types.h"

namespace rcss
{
  namespace clang
  { 
    FreeformMsg::FreeformMsg()
      : Msg()
    {}
    
    FreeformMsg::FreeformMsg( const std::string& str )
	: Msg(),
	  m_str( str )
    {}
    
    FreeformMsg::~FreeformMsg()
    {}

    std::auto_ptr< Msg >
    FreeformMsg::deepCopy() const
    { return std::auto_ptr< Msg >( new FreeformMsg( *this ) ); }

//     void
//     FreeformMsg::accept( Visitor& v )
//     { v.startVisit( this ); }

//     void
//     FreeformMsg::accept( ConstVisitor& v ) const
//     { v.startVisit( this ); }
    
    std::ostream&
    FreeformMsg::print( std::ostream& out ) const
    {
      out << "(freeform \"" << m_str << "\")"; 
      return out;
    }
    
    std::ostream&
    FreeformMsg::printPretty( std::ostream& out, const std::string& line_header ) const
    {
      return out << line_header << "Freeform" << std::endl
                 << line_header << "  " << "\"" << m_str << "\"" << std::endl;
    }

      std::string
      FreeformMsg::getStr()
      { return m_str; }

    const std::string&
    FreeformMsg::getStr() const
    { return m_str; }
  }
}

