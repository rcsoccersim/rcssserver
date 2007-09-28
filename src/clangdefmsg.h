// -*-c++-*-

/***************************************************************************
                                clangdefmsg.h  
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

#ifndef CLANGDEFMSG_H
#define CLANGDEFMSG_H

#include "clangmsg.h"
#include "hasa.h"
#include "coach_lang_comp.h"

namespace rcss
{
  namespace clang
  {
    class DefineMsg
      : public Msg
    {
    public:
      typedef std::list< Def* > Storage;

      DefineMsg();
      
      DefineMsg( const Storage& defs );

      virtual
      ~DefineMsg();

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
      getDefs() const;

	Storage& 
	getDefs();

	virtual
	Types
	getType() const { return DEFINE; }

    private:
	Storage m_defs;

    };
  }
}

#endif
