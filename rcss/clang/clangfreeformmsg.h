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
 *   Foundation; either version 3 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#ifndef RCSS_CLANGFREEFORMMSG_H
#define RCSS_CLANGFREEFORMMSG_H

#include <rcss/clang/clangmsg.h>

#include <string>

namespace rcss {
namespace clang {

class FreeformMsg
    : public Msg {
private:
    FreeformMsg();

public:

    FreeformMsg( const std::string & str );

    virtual
    ~FreeformMsg() override;

    virtual
    std::shared_ptr< Msg > deepCopy() const override;

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

	virtual
	Types getType() const
      {
          return FREEFORM;
      }

private:
	std::string M_str;

};

}
}

#endif
