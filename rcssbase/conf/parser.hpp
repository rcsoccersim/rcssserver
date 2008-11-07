// -*-c++-*-

/***************************************************************************
                                parser.hpp
                          Parser for Config options
                             -------------------
    begin                : 14-MAY-2003
    copyright            : (C) 2003 by The RoboCup Soccer Server
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

#ifndef PARSER_HPP
#define PARSER_HPP

#include "rcssbase/parser.h"

#include <boost/filesystem/path.hpp>

#include <string>
#include <list>


namespace rcss {
namespace conf {

class Builder;

class Parser
    : public rcss::Parser {
public:
    enum Errors { NONE,
                  ASSIGN_EXPECTED,
                  DELIM_EXPECTED,
                  VALUE_EXPECTED,
                  STRING_EXPECTED };

    static const char * ErrorStrs[];

    Parser( Builder & builder );

    ~Parser();

    /// for parsing command line arguments
    bool parse( int argc,
                const char * const * argv );

    /// for parsing an unnamed stream
    bool parse( std::istream & strm );

    /// for parsing a named stream
    bool parse( std::istream & strm,
                const std::string & name );

    /// for parsing a file
    bool parse( const boost::filesystem::path & name );

    /// for parsing a file and creating it if it doesn't exist
    bool parseCreateConf( const boost::filesystem::path & conf_name,
                          const std::string & module_name );

private:
    Parser( const Parser & ); // not used
    Parser & operator=( const Parser & ); // not used

    /// calls the boost::spirit parser
    bool doParse( std::istream & strm );

    bool boostParse();

    Builder & m_builder;

    class StreamState {
    public:
			  std::istream * m_strm;
			  std::string m_name;
			  int m_lineno;

			  StreamState( std::istream & strm,
                     const std::string & name,
                     int lineno )
            : m_strm( &strm ),
              m_name( name ),
              m_lineno( lineno )
          { }

    };

    typedef std::list< StreamState > StreamStack;

    StreamStack m_stack;

    void countNewLines( const char * begin,
                        const char * end );

    bool include( const char * begin,
                  const char * end );

    bool setParamName( const char * begin,
                       const char * end );

    void appendParamName( const char * begin,
                          const char * end );

    bool buildBool( bool value );

    bool buildInt( int value );

    bool buildReal( double value );

    bool buildString( const char * begin,
                      const char * end );

    void requestGenericHelp();

    void requestDetailedHelp();

    void parseError( size_t pos,
                     const std::string & buffer );

    std::string m_curr_module;
    std::string m_curr_param;

    friend class ParseErrorHandler;
};

}
}

#endif
