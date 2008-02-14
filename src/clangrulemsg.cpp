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
 *   Foundation; either version 2 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#include "clangrulemsg.h"
#include "rule.h"
#include "types.h"

namespace rcss
{
  namespace clang
  {
    RuleMsg::RuleMsg()
      : Msg()
    {}

    RuleMsg::RuleMsg( const Storage& list )
	: Msg(),
	  m_active( list )
    {}
    
    RuleMsg::~RuleMsg()
    {
    }
    
    std::auto_ptr< Msg >
    RuleMsg::deepCopy() const
    { 
	return std::auto_ptr< Msg >( new RuleMsg( *this ) ); 
    }

//     void
//     RuleMsg::accept( Visitor& v )
//     { v.startVisit( this ); }

//     void
//     RuleMsg::accept( ConstVisitor& v ) const
//     { v.startVisit( this ); }

    std::ostream&
    RuleMsg::print( std::ostream& out ) const
    { 
      out << "(rule ";
      for( Storage::const_iterator i = m_active.begin();
           i != m_active.end(); ++i )
        out << *i;
      out << ")";
      return out;
    }
    
    std::ostream&
    RuleMsg::printPretty( std::ostream& out,
                          const std::string& line_header ) const
    { 
      out << line_header << "Activation List:\n";
      bool first = true;
      for( Storage::const_iterator i = m_active.begin();
           i != m_active.end(); ++i )
        {
          if( first )
            first = false;
          else
            out << line_header << " Then:\n";
          i->printPretty( out, line_header + "  -" );
        }
      return out;
    }
    
    const RuleMsg::Storage&
    RuleMsg::getList() const
    { return m_active; }

      RuleMsg::Storage&
      RuleMsg::getList()
      { return m_active; }
  }
}
