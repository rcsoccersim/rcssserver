/* -*- Mode: c++ -*- */

/*
 *Copyright:

 Copyright (C) 1996-2000 Electrotechnical Laboratory.
 Itsuki Noda, Yasuo Kuniyoshi and Hitoshi Matsubara.
 Copyright (C) 2000, 2001 RoboCup Soccer Server Maintainance Group.
 Patrick Riley, Tom Howard, Daniel Polani, Itsuki Noda,
 Mikhail Prokopenko, Jan Wendler
 Copyright (C) 2002- RoboCup Soccer Simulator Maintainance Group.

 This file is a part of SoccerServer.

 This code is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 3 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 *EndCopyright:
 */


#ifndef XMLREADER_H
#define XMLREADER_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_LIBEXPAT

#include <string>
#include <iostream>

#include <expat.h>

class XmlReader {
public:
    XmlReader();
    virtual ~XmlReader();

    friend void
    _XML_READER_start ( void * data, const char * el, const char ** attr );
    friend void
    _XML_READER_end ( void * data, const char * el );

    virtual
    std::ostream & print( std::ostream & o ) const = 0;

protected:
    std::string path;

    XML_Parser parser;
    long line;

    bool readXml( const std::string & path_name );

    virtual
    void start( const char * el,
                const char ** attr ) = 0;
    virtual
    void end( const char * el ) = 0;

    bool createFile( const std::string & path );

    bool extract( const std::string & attr,
                  const std::string & val,
                  double & dest,
                  double start = 0.0,
                  double end = 100.0 );
    bool extract( const std::string & attr,
                  const std::string & val,
                  std::string & dest );
    bool extract ( const std::string & attr,
                   const std::string & val,
                   char & dest );

};

#endif // HAVE_LIBEXPAT

#endif // _XML_READER_H_
