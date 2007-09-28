// -*-c++-*-

/***************************************************************************
                                clanginfomsg.h  
                       Class for CLang Info messages
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

#ifndef CLANGINFOMSG_H
#define CLANGINFOMSG_H

#include "clangmsg.h"
#include "hasa.h"
#include "coach_lang_comp.h"

namespace rcss
{
  namespace clang
  {
    class InfoMsg 
      : public Msg
    {
    public:
      typedef std::list< Token* > Storage;

      InfoMsg();

      InfoMsg( const Storage& tokens );

      virtual
      ~InfoMsg();

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
      getTokens() const;

      Storage& 
      getTokens();

	virtual
	Types
	getType() const { return INFO; }

    private:
	Storage m_tokens;

    };
  }
}

#endif
