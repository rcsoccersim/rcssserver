// -*-c++-*-

/***************************************************************************
                                clangrulemsg.h  
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

#ifndef CLANGRULEMSG_H
#define CLANGRULEMSG_H

#include "clangmsg.h"
#include "hasa.h"
#include <list>


namespace rcss
{
  namespace clang
  {
    class ActivateRules;

    class RuleMsg 
      : public Msg
    {
    public:
      typedef std::list< ActivateRules > Storage;

      RuleMsg();

      RuleMsg( const Storage& list );

      virtual
      ~RuleMsg();

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

      const Storage&
      getList() const;

	Storage&
      getList();

	virtual
	Types
	getType() const { return RULE; }

    private:
	Storage m_active;

    };
  }
}

#endif
