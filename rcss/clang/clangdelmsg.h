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
 *   Foundation; either version 3 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#ifndef RCSS_CLANGDELMSG_H
#define RCSS_CLANGDELMSG_H

#include <rcss/clang/clangmsg.h>
#include <rcss/clang/rule.h>

namespace rcss {
namespace clang {

class DelMsg
    : public Msg {
public:

    DelMsg();
private:
    DelMsg( const RuleIDList & list );
public:

    virtual
    ~DelMsg() override;

    virtual
    std::shared_ptr< Msg > deepCopy() const override;

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

    const RuleIDList & getList() const
      {
          return M_rids;
      }

	void set( const RuleIDList & rids )
      {
          M_rids = rids;
      }

	virtual
	Types getType() const
      {
          return DEL;
      }

private:
	RuleIDList M_rids;
};

}
}

#endif
