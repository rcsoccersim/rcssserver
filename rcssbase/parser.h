// -*-c++-*-

/***************************************************************************
                                  parser.h
                               Parser template
                             -------------------
    begin                : 24-MAY-2002
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

#ifndef PARSER_H
#define PARSER_H

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>

#include <string>
#include <iostream>
#include <fstream>

#if defined(BOOST_FILESYSTEM_VERSION) && BOOST_FILESYSTEM_VERSION > 2

#define BOOST_FS_ABSOLUTE absolute
#define BOOST_FS_FILE_STRING string
#define BOOST_FS_DIRECTORY_STRING string
#define BOOST_FS_PARENT_PATH parent_path

#else

#define BOOST_FS_ABSOLUTE complete

#  ifdef BOOST_FILESYSTEM_NO_DEPRECATED
#    define BOOST_FS_FILE_STRING file_string
#    define BOOST_FS_DIRECTORY_STRING directory_string
#    define BOOST_FS_PARENT_PATH parent_path
#  else
#    define BOOST_FS_FILE_STRING native_file_string
#    define BOOST_FS_DIRECTORY_STRING native_directory_string
#    define BOOST_FS_PARENT_PATH branch_path
#  endif

#endif

namespace rcss {

class Parser {
private:
    std::string M_file_name;

public:
    Parser()
      { }

    virtual
    ~Parser()
      { }

    bool parse( std::istream & strm )
      {
          M_file_name = "anonymous stream";
          return doParse( strm );
      }

    bool parse( std::istream & strm,
                std::ostream & errstrm )
      {
          return doParse( strm, errstrm );
      }

    bool parse( std::istream & strm,
                const std::string & name )
      {
          M_file_name = name;
          return doParse( strm );
      }

    bool parse( std::istream & strm,
                const std::string & name,
                std::ostream & errstrm )
      {
          M_file_name = name;
          return doParse( strm, errstrm );
      }

    bool parse( const boost::filesystem::path & file )
      {
          return parse( file, std::cerr );
      }

    bool parse( const boost::filesystem::path & file,
                std::ostream & errstrm )
      {
          boost::filesystem::ifstream strm( file );

          if( ! strm.is_open() )
          {
              return false;
          }

          bool rval = parse( strm, file.BOOST_FS_FILE_STRING(), errstrm );

          strm.close();

          return rval;
      }

private:
    virtual
    bool doParse( std::istream& strm ) = 0;

    virtual
    bool doParse( std::istream & strm,
                  std::ostream & )
      {
          return doParse( strm );
      }

protected:
    const
    std::string & getFileName() const
      {
          return M_file_name;
      }

    const
    std::string & getStreamName() const
      {
          return M_file_name;
      }
};

inline
std::string
stripQuotes( std::string str,
             char quote = '"' )
{
    // returns a new string with the quotes removed or as is if there
    // are no bounding quotes
    if ( str.empty() )
    {
        return str;
    }
    else if ( str[ 0 ] == quote )
    {
        str = str.substr( 1, str.length() - 1 );
    }

    if ( str[ str.length() - 1 ] == quote )
    {
        return str.substr( 0, str.length() - 1 );
    }

    return str;
}

}

#endif
