// -*-c++-*-

/***************************************************************************
                                clangdelmsg.h  
                       Class for CLang Delete messages
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

#ifndef CLANGDELMSG_H
#define CLANGDELMSG_H

#include "clangmsg.h"
#include "hasa.h"
#include "rule.h"

namespace rcss
{
  namespace clang
  {
    class DelMsg 
      : public Msg
    {
    public:

      DelMsg();

      DelMsg( const RuleIDList& list );

      virtual
      ~DelMsg();

      virtual
      std::auto_ptr< Msg >
      deepCopy() const;

//       virtual
//       void
//       accept( Visitor& v );

//       virtual
//       void
//       accept( ConstVisitor& v ) const;

      virtual
      std::ostream&
      print( std::ostream& out ) const;

      virtual
      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const;

      const RuleIDList&
      getList() const;    

	void
	set( const RuleIDList& rids )
	    {
		m_rids = rids;
	    }

	virtual
	Types
	getType() const { return DEL; }

    private:
	RuleIDList m_rids;
    };
  }
}

#endif
