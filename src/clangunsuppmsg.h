// -*-c++-*-

/***************************************************************************
                                clangunsuppmsg.h
                       Class for CLang unsupported messages
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

#ifndef CLANGUNSUPPMSG_H
#define CLANGUNSUPPMSG_H

#include "clangmsg.h"

namespace rcss {
namespace clang {

class UnsuppMsg
    : public Msg  {
public:
    UnsuppMsg();

    virtual
    ~UnsuppMsg();

    virtual
    std::auto_ptr< Msg > deepCopy() const;

    virtual
    std::ostream & print( std::ostream & out ) const;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const;

	virtual
	Types getType() const
      {
          return UNSUP;
      }

};

}
}

#endif
