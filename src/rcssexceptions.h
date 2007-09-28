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
 *   Foundation; either version 2 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#ifndef _RCSS_EXCEPTIONS_
#define _RCSS_EXCEPTIONS_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_SSTREAM
#include <sstream>
#else
#include <strstream>
#endif

#include <exception>
#include <string>

namespace rcss
{
  namespace util
  {
    class NullErr
         : public std::exception
    {
    protected:
      std::string M_msg;
    public:
      NullErr ( const char* file, const int& line, const char* msg )
      {
#ifdef HAVE_SSTREAM
          std::ostringstream tmp;
          tmp << file << ": " << line << ": " << msg;
          M_msg = tmp.str();
#else
          std::ostrstream tmp;
          tmp << file << ": " << line << ": " << msg << std::ends;
          M_msg = tmp.str();
          tmp.freeze( false );
#endif
      }
    
    ~NullErr () throw ()
    {
    }
    
    const char*
    what () const throw ()
    { return M_msg.c_str (); }
        
    };
  }
}
#endif
    
