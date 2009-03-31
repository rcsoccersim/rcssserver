// -*-c++-*-

/***************************************************************************
                               rcssexceptions.h
                               exceptions for rcss
                             -------------------
    begin                : 27-JUN-2002
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

#ifndef RCSS_EXCEPTIONS_H
#define RCSS_EXCEPTIONS_H

#include <exception>
#include <string>

namespace rcss {
namespace util {

class NullErr
    : public std::exception {
protected:
    std::string M_msg;
public:
    NullErr( const char * file,
             const int & line,
             const char * msg ) throw();

    ~NullErr() throw()
      { }

    const char * what() const throw()
      {
          return M_msg.c_str();
      }
};

}
}

#endif
