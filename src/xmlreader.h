/* -*- Mode: c++ -*- */

/*
 *Copyright:

    Copyright (C) 1996-2000 Electrotechnical Laboratory. 
    	Itsuki Noda, Yasuo Kuniyoshi and Hitoshi Matsubara.
    Copyright (C) 2000, 2001 RoboCup Soccer Server Maintainance Group.
    	Patrick Riley, Tom Howard, Daniel Polani, Itsuki Noda,
	Mikhail Prokopenko, Jan Wendler 

    This file is a part of SoccerServer.

    This code is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 *EndCopyright:
 */


#ifndef _XMLREADER_H_
#define _XMLREADER_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBEXPAT


#include <cstdlib>
#include <fstream>
#include <string>
#include <cmath>
#include <expat.h>
#include "utility.h"
#include <iostream>

class XmlReader
{
public:
  XmlReader();
  virtual ~XmlReader();

  friend void 
  _XML_READER_start ( void *data, const char *el, const char **attr );
  friend void 
  _XML_READER_end ( void *data, const char *el );

  friend ostream& operator<< ( ostream& o, const XmlReader& xml )
  {
    return xml.print ( o );
  }

  virtual ostream& print ( ostream& o ) const = 0;

protected:
  string path;
 
  XML_Parser parser;
  long line;

  bool readXml ( const string& path_name );

  virtual void start ( const char *el, const char **attr ) = 0;
  virtual void end ( const char *el ) = 0;

  bool createFile ( const string& path );

  bool 
  extract ( const string& attr, 
	    const string& val, 
	    double& dest, 
	    double start = 0.0, double end = 100.0 )
  {
    istrstream str ( val.c_str (), val.length () );
    str >> dest;
    if ( !str.fail () && str.get () == '%' )
      {
        dest *= 0.01;
        dest = ( start * ( 1 - dest ) 
                 + end * ( 1 + dest ) ) * 0.5;
      }
    if ( str.fail () || str.peek () > 0 )
      {
        // error
        cerr << path << ": " << line
             << ": Cannot convert " << attr 
             << " attribute from " << val << endl;
        return false;
      }
    return true;
  }

  bool extract ( const string& attr, const string& val, string& dest )
  {
    if ( val.length () == 0 )
      {
        // error
        cerr << path << ": " << line
             << ": Cannot convert " << attr 
             << " attribute from \"\"" << endl;
        return false;
      }
    dest = val;
    return true;
  }

  bool extract ( const string& attr, const string& val, char& dest )
  {
    if ( val.length () == 0 )
      {
        // error
        cerr << path << ": " << line
             << ": Cannot convert " << attr 
             << " attribute from \"\"" << endl;
        return false;
      }

    dest = val [ 0 ];
    return true;
  }
};

#endif // HAVE_LIBEXPAT

#endif // _XML_READER_H_
