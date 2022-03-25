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
 *   Foundation; either version 3 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#ifndef RCSS_CLANGRULEMSG_H
#define RCSS_CLANGRULEMSG_H

#include <rcss/clang/clangmsg.h>

#include <list>

namespace rcss {
namespace clang {

class ActivateRules;

class RuleMsg
    : public Msg {
public:
    typedef std::list< ActivateRules > Storage;

    RuleMsg();
private:
    RuleMsg( const Storage & list );
public:
    virtual
    ~RuleMsg() override;

    virtual
    std::shared_ptr< Msg > deepCopy() const override;

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

    const Storage & getList() const
      {
          return M_active;
      }

	Storage & getList()
      {
          return M_active;
      }

	virtual
	Types getType() const
      {
          return RULE;
      }

private:
	Storage M_active;

};

}
}

#endif
