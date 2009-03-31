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
 *   Foundation; either version 3 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#ifndef CLANGDEFMSG_H
#define CLANGDEFMSG_H

#include "clangmsg.h"
#include "coach_lang_comp.h"

namespace rcss {
namespace clang {

class DefineMsg
    : public Msg {
public:
    typedef std::list< Def* > Storage;

    DefineMsg();
private:
    DefineMsg( const Storage & defs );
public:

    virtual
    ~DefineMsg();

    virtual
    std::auto_ptr< Msg > deepCopy() const;

    virtual
    std::ostream & print( std::ostream & out ) const;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const;

    const Storage & getDefs() const
      {
          return M_defs;
      }

    Storage & getDefs()
      {
          return M_defs;
      }

	virtual
	Types getType() const
      {
          return DEFINE;
      }

private:
	Storage M_defs;

};

}
}

#endif
