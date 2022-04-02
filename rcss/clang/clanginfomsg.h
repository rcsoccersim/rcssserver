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
 *   Foundation; either version 3 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#ifndef RCSS_CLANGINFOMSG_H
#define RCSS_CLANGINFOMSG_H

#include <rcss/clang/clangmsg.h>
#include <rcss/clang/coach_lang_comp.h>

namespace rcss {
namespace clang {

class InfoMsg
    : public Msg {
public:
    typedef std::list< std::shared_ptr< Token > > Storage;

    InfoMsg();
private:
    InfoMsg( const Storage & tokens );
public:

    virtual
    ~InfoMsg() override;

    virtual
    std::shared_ptr< Msg > deepCopy() const override;

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

    const Storage & getTokens() const
      {
          return M_tokens;
      }

    Storage & getTokens()
      {
          return M_tokens;
      }

	virtual
	Types getType() const
      {
          return INFO;
      }

private:
	Storage M_tokens;

};

}
}

#endif
