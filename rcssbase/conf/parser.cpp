// -*-c++-*-

/***************************************************************************
                                parser.cpp
                          Parser for config options
                             -------------------
    begin                : 15-MAY-2003
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "parser.hpp"

#include "builder.hpp"

#ifdef HAVE_SSTREAM
#include <sstream>
#else
#include <strstream>
#endif

#include <iterator>

#include <cerrno>
#include <climits>

#include <boost/version.hpp>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/exception.hpp>

// #define BOOST_SPIRIT_DEBUG
#if BOOST_VERSION >= 103800
#  include <boost/spirit/include/classic.hpp>
#else
#  include <boost/spirit.hpp>
#endif

#include <boost/function.hpp>
#include <boost/bind.hpp>

namespace {

bool
isQuated( const std::string & str )
{
    return ( str.length() >= 2
             && ( ( *str.begin() == '\"' && *str.rbegin() == '\"' )
                  || ( *str.begin() == '\'' && *str.rbegin() == '\'' )
                  )
             );
}

std::string
cleanString( std::string str )
{
    if ( str.empty() )
    {
        return str;
    }

    if ( *str.begin() == '\'' )
    {
        if ( *str.rbegin() == '\''  )
        {
            str = str.substr( 1, str.length() - 2 );
        }
        else
        {
            return str;
        }

        for ( std::string::size_type esc = str.find( "\\'" );
              esc != std::string::npos;
              esc = str.find( "\\'" ) )
        {
            str.replace( esc, 2, "'" );
        }
    }
    else if ( *str.begin() == '"' )
    {
        if ( *str.rbegin() == '"'  )
        {
            str = str.substr( 1, str.length() - 2 );
        }
        else
        {
            return str;
        }

        for ( std::string::size_type esc = str.find( "\\\"" );
              esc != std::string::npos;
              esc = str.find( "\\\"" ) )
        {
            str.replace( esc, 2, "\"" );
        }
    }
    return str;
}

std::string
cleanString( const char * begin,
             const char * end )
{
    return cleanString( std::string( begin, end ) );
}

}


namespace rcss {
namespace conf {

const char * Parser::ErrorStrs[] = { "None",
                                     "'=' expected",
                                     "'::' expected",
                                     "value expected",
                                     "string expected" };

namespace sc = BOOST_SPIRIT_CLASSIC_NS;


class ParseErrorHandler {
public:
    static
    sc::error_status<> parseError( const Parser * parser,
                                   const sc::rule<>::scanner_t & scanner,
                                   const sc::parser_error< Parser::Errors > & error )
      {
          std::string what;
          if ( error.where + 10 > scanner.last )
          {
              what = std::string( error.where, scanner.last );
          }
          else
          {
              what = std::string( error.where,
                                  error.where + 10 );
          }

          std::string::iterator newline = std::find( what.begin(),
                                                     what.end(),
                                                     '\n' );
          if ( newline != what.end() )
          {
              what = std::string( what.begin(), newline );
          }

          parser->m_builder.parseError( what,
                                        Parser::ErrorStrs[ error.descriptor ],
                                        parser->m_stack.front().m_name,
                                        parser->m_stack.front().m_lineno );
          return sc::error_status<>( sc::error_status<>::fail );
      }
};


Parser::Parser( Builder & builder )
    : m_builder( builder )
{
    m_builder.addedToParser( *this );
}

Parser::~Parser()
{
    m_builder.removedFromParser();
}


bool
Parser::parse( int argc,
               const char * const * argv )
{
#ifdef HAVE_SSTREAM
    std::stringstream strm;
#else
    std::strstream strm;
#endif
    if ( argc > 1 )
    {
        std::string arg( argv[1] );
        if ( arg.find_first_of( " \t" ) != std::string::npos
             && ! isQuated( arg ) )
        {
            strm << "\'" << arg << "\'";
        }
        else
        {
            strm << arg;
        }
    }

    for ( int i = 2; i < argc; ++i )
    {
        std::string arg( argv[i] );
        if ( arg.find_first_of( " \t" ) != std::string::npos
             && ! isQuated( arg ) )
        {
            strm << ' ' << '\'' << arg << '\'';
        }
        else
        {
            strm << ' ' << arg;
        }
    }
#ifndef HAVE_SSTREAM
    strm << std::ends;
#else
    strm << std::flush;
#endif
    bool res = rcss::Parser::parse( strm, "cmd line args" );
#ifndef HAVE_SSTREAM
    strm.freeze( false );
#endif
    return res;
}

bool
Parser::parse( std::istream & strm )
{
    return rcss::Parser::parse( strm );
}

bool
Parser::parse( std::istream & strm,
               const std::string & name )
{
    return rcss::Parser::parse( strm, name );
}

bool
Parser::parse( const boost::filesystem::path & file )
{
    return rcss::Parser::parse( file );
}

bool
Parser::parseCreateConf( const boost::filesystem::path & conf_name,
                         const std::string & module_name )
{
    std::string native_path = conf_name.BOOST_FS_FILE_STRING();

    boost::filesystem::ifstream conf( conf_name );
    if ( ! conf.is_open() )
    {
        m_builder.creatingConfFile( native_path );
        boost::filesystem::ofstream new_conf( conf_name );
        if ( new_conf.is_open() )
        {
            m_builder.createConfFile( new_conf, module_name );
            new_conf.close();
            m_builder.createdConfFile( native_path );
            return true;
        }
        else
        {
            std::cerr << "is not opened. Parser::parseCreateConf " << native_path << std::endl;
            m_builder.confCreationFailed( native_path, errno );
            return false;
        }
    }
    else
    {
        bool rval = parse( conf, native_path );
        conf.close();
        return rval;
    }
}

bool
Parser::doParse( std::istream & strm )
{
    m_builder.reset();
    m_stack.push_front( StreamState( strm,  getStreamName(), 1 ) );
    bool rval = boostParse();
    m_stack.pop_front();
    rval = rval && m_builder.success();
    return rval;
}

void
Parser::countNewLines( const char * begin,
                       const char * end )
{
    int count = 0;
    for ( const char * i = begin; i != end; ++i )
    {
        if ( '\n' == *i )
        {
            ++count;
        }
    }

    if ( ! m_stack.empty() )
    {
        m_stack.front().m_lineno += count;
    }
}

bool
Parser::include( const char * begin,
                 const char * end )
{
    std::string incname = cleanString( begin, end );
    boost::filesystem::path path;
    try
    {
        path = boost::filesystem::path( incname
#if defined(BOOST_FILESYSTEM_VERSION) && BOOST_FILESYSTEM_VERSION == 2
#  ifndef BOOST_FILESYSTEM_NO_DEPRECATED
                                        , &boost::filesystem::native
#  endif
#endif
                                        );
    }
    catch ( ... )
    {
        try
        {
            path = boost::filesystem::path( incname );
        }
        catch ( const std::exception & e )
        {
            m_builder.includeFailed( incname,
                                     e.what(),
                                     m_stack.front().m_name,
                                     m_stack.front().m_lineno );
            return false;
        }
    }

    boost::filesystem::path full_name;
    if ( path.has_root_directory() )
    {
        full_name = path;
    }
    else
    {
        const std::string & curr_path = m_stack.front().m_name;
        if ( curr_path.empty()
             || curr_path == "cmd line args"
             || curr_path == "anonymous stream" )
        {
            full_name = boost::filesystem::BOOST_FS_ABSOLUTE( path );
        }
        else
        {
            try
            {
                boost::filesystem::path branch( curr_path
#if defined(BOOST_FILESYSTEM_VERSION) && BOOST_FILESYSTEM_VERSION == 2
#  ifndef BOOST_FILESYSTEM_NO_DEPRECATED
                                                , &boost::filesystem::native
#  endif
#endif
                                                );
                branch = branch.BOOST_FS_PARENT_PATH();
                full_name = branch / path;
            }
            catch  ( const std::exception & e )
            {
                m_builder.includeFailed( incname,
                                         e.what(),
                                         m_stack.front().m_name,
                                         m_stack.front().m_lineno );
                return false;
            }
        }
    }

    // check if the stream is already on the stack
    for ( StreamStack::iterator i = m_stack.begin();
          i != m_stack.end();
          ++i )
    {
        if ( full_name.BOOST_FS_FILE_STRING() == i->m_name )
        {
            // file already included so ignore it
            return true;
        }
    }

    if ( boost::filesystem::exists( full_name ) )
    {
        if ( ! boost::filesystem::is_directory( full_name ) )
        {
            boost::filesystem::ifstream strm( full_name );
            if ( strm.is_open() && strm.good() )
            {
                bool rval = parse( strm,
                                   full_name.BOOST_FS_FILE_STRING() );

                strm.close();
                return rval;
            }
            else
            {
                m_builder.includeFailed( full_name.BOOST_FS_FILE_STRING(),
                                         "read failed",
                                         m_stack.front().m_name,
                                         m_stack.front().m_lineno );
                return false;
            }
        }
        else
        {
            m_builder.includeFailed( full_name.BOOST_FS_FILE_STRING(),
                                     "cannot include a directory",
                                     m_stack.front().m_name,
                                     m_stack.front().m_lineno );
            return false;
        }
    }
    else
    {
        m_builder.includeFailed( full_name.BOOST_FS_FILE_STRING(),
                                 "file not found",
                                 m_stack.front().m_name,
                                 m_stack.front().m_lineno );
        return false;
    }
}

bool
Parser::setParamName( const char * begin,
                      const char * end )
{
    m_curr_param = cleanString( begin, end );
    m_curr_module.clear();
    return true;
}

void
Parser::appendParamName( const char * begin,
                         const char * end )
{
    if ( m_curr_module.empty() )
    {
        m_curr_module = m_curr_param;
    }
    else
    {
        m_curr_module += "::" + m_curr_param;
    }
    m_curr_param = cleanString( begin, end );
}

bool
Parser::buildBool( bool value )
{
    m_builder.buildParam( m_curr_module, m_curr_param, value,
                          m_stack.front().m_name,
                          m_stack.front().m_lineno );
    return true;
}

bool
Parser::buildInt( int value )
{
    m_builder.buildParam( m_curr_module, m_curr_param, value,
                          m_stack.front().m_name, m_stack.front().m_lineno );
    return true;
}

bool
Parser::buildReal( double value )
{
    m_builder.buildParam( m_curr_module, m_curr_param, value,
                          m_stack.front().m_name, m_stack.front().m_lineno );
    return true;
}

bool
Parser::buildString( const char* begin, const char* end )
{
    m_builder.buildParam( m_curr_module, m_curr_param,
                          cleanString( begin, end ),
                          m_stack.front().m_name, m_stack.front().m_lineno );
    return true;
}

void
Parser::requestGenericHelp()
{
    m_builder.requestGenericHelp();
}

void
Parser::requestDetailedHelp()
{
    if ( m_curr_module.empty() )
    {
        m_curr_module = m_curr_param;
    }
    else
    {
        m_curr_module += "::" + m_curr_param;
    }
    m_builder.requestDetailedHelp( m_curr_module );
}

void
Parser::parseError( size_t pos,
                    const std::string & buffer )
{
    std::string what;
    if ( pos + 10 > buffer.size() )
    {
        what = std::string( buffer.begin() + pos, buffer.end() );
    }
    else
    {
        what = std::string( buffer.begin() + pos,
                            buffer.begin() + pos + 10 );
    }
    std::string::iterator newline = std::find( what.begin(),
                                               what.end(),
                                               '\n' );
    if ( newline != what.end() )
    {
        what = std::string( what.begin(), newline );
    }
    m_builder.parseError( what,
                          "unknown parse error",
                          m_stack.front().m_name,
                          m_stack.front().m_lineno );
}

bool
Parser::boostParse()
{
    using namespace rcss::conf;

    sc::assertion< Errors > expect_assign( ASSIGN_EXPECTED );
    sc::assertion< Errors > expect_delim( DELIM_EXPECTED );
    sc::assertion< Errors > expect_value( VALUE_EXPECTED );
    sc::assertion< Errors > expect_string( STRING_EXPECTED );
    sc::guard< Errors > conf_guard;

    std::istream & in = * m_stack.front().m_strm;
    typedef std::istreambuf_iterator< char > iter_t;
    std::string buffer;
    std::copy( iter_t( in ), iter_t(), std::back_inserter( buffer ) );

    sc::rule<> ws_p = +sc::chset_p( " \t\0" );
    sc::rule<> newline_p = sc::eol_p;
    sc::rule<> comment_p = (
                            sc::comment_p( "/*", "*/" )
                            | sc::comment_p( "//" )
                            | sc::comment_p( "#" ) );

    sc::rule<> junk_p
        = ( ws_p
            | comment_p[ boost::bind( &Parser::countNewLines, this, _1, _2 ) ]
            | newline_p[ boost::bind( &Parser::countNewLines, this, _1, _2 ) ]
            );

    sc::rule<> ignore_p = *junk_p;

    sc::rule<> minus_p = sc::ch_p( '-' );

    sc::rule<> assign_p = sc::ch_p( '=' );

    sc::rule<> simple_str_p
        = ( +(~sc::chset_p( ":\"'= \t\n-" ) )
            >> *(~sc::chset_p( ":\"'= \t\n" ) )
            );

    sc::rule<> pqsb_p = ~sc::ch_p( '"' );
    sc::rule<> qsb_p = ( sc::str_p( "\\\"" )
                         | pqsb_p
                         );

    sc::rule<> qstr_p = '"' >> *qsb_p >> '"';

    sc::rule<> pqsb2_p = ~sc::ch_p( '\'' );
    sc::rule<> qsb2_p = ( sc::str_p( "\\'" )
                          | pqsb2_p
                          );
    sc::rule<> qstr2_p = '\'' >> (*qsb2_p) >> '\'';

    sc::rule<> string_p = ( simple_str_p
                            | qstr_p
                            | qstr2_p );

    sc::rule<> flag_p = ( sc::as_lower_d[ "help" ]
                          | sc::as_lower_d[ "include" ]
                          );


    sc::rule<> includerule_p
        = ( sc::as_lower_d[ "include" ]
            >> ignore_p
            >> expect_assign( assign_p )
            >> ignore_p
            >> expect_string( string_p[ boost::bind( &Parser::include,
                                                     this,
                                                     _1, _2 ) ] )
            );

    sc::rule<> delim_p = sc::str_p( "::" );

    sc::rule<> param_name_p
        = ( (simple_str_p - flag_p)[ boost::bind( &Parser::setParamName,
                                                  this,
                                                  _1, _2 ) ]
            >> ( expect_delim( delim_p )
                 >> ( simple_str_p - sc::as_lower_d[ "help" ]
                      )[ boost::bind( &Parser::appendParamName,
                                      this,
                                      _1, _2 ) ]
                 )
            >> *( delim_p
                  >> ( simple_str_p - sc::as_lower_d[ "help" ]
                       )[ boost::bind( &Parser::appendParamName,
                                       this,
                                       _1, _2 ) ]
                  )
            );

    sc::rule<> module_name_p
        = ( ( simple_str_p - flag_p )[ boost::bind( &Parser::setParamName,
                                                    this,
                                                    _1, _2 ) ]
            >> *( delim_p
                  >> ( simple_str_p - sc::as_lower_d[ "help" ]
                       )[ boost::bind( &Parser::appendParamName,
                                       this,
                                       _1, _2 ) ]
                  )
            );

    sc::rule<> true_p = ( sc::as_lower_d[ "on" ]
                          | sc::as_lower_d[ "true" ]
                          );

    sc::rule<> false_p = ( sc::as_lower_d[ "off" ]
                           | sc::as_lower_d[ "false" ]
                           );

    sc::rule<> param_p
        = ( param_name_p
            >> ignore_p
            >> expect_assign( assign_p )
            >> ignore_p
            >> expect_value( true_p[ boost::bind( &Parser::buildBool,
                                                  this,
                                                  true ) ]
                             | false_p[ boost::bind( &Parser::buildBool,
                                                     this,
                                                     false ) ]
                             | sc::strict_real_p[ boost::bind( &Parser::buildReal,
                                                               this,
                                                               _1 ) ]
                             | sc::int_p[ boost::bind( &Parser::buildInt,
                                                       this,
                                                       _1 ) ]
                             | string_p[ boost::bind( &Parser::buildString,
                                                      this,
                                                      _1, _2 ) ]
                             )
            );

    sc::rule<> data_p
        = ( sc::as_lower_d[ "help" ][ boost::bind( &Parser::requestGenericHelp,
                                                   this ) ]
            | ( module_name_p
                >> delim_p
                >> sc::as_lower_d[ "help" ]
                )[ boost::bind( &Parser::requestDetailedHelp,
                                this ) ]
            | param_p
            | includerule_p
            );

    sc::rule<> item_p = ( junk_p
                          | ( !minus_p
                              >> !minus_p
                              >> data_p )
                          );
    sc::rule<> input_p
        = conf_guard( *item_p )[ boost::bind( &ParseErrorHandler::parseError,
                                              this,
                                              _1, _2 ) ];

    //      BOOST_SPIRIT_DEBUG_RULE(ws_p);
    //      BOOST_SPIRIT_DEBUG_RULE(newline_p);
    //      BOOST_SPIRIT_DEBUG_RULE(comment_p);
    //      BOOST_SPIRIT_DEBUG_RULE(string_p);
    //      BOOST_SPIRIT_DEBUG_RULE(input_p);
    //      BOOST_SPIRIT_DEBUG_RULE(item_p);
    //      BOOST_SPIRIT_DEBUG_RULE(param_p);
    //      BOOST_SPIRIT_DEBUG_RULE(data_p);
    //      BOOST_SPIRIT_DEBUG_RULE(junk_p);
    //      BOOST_SPIRIT_DEBUG_RULE(ignore_p);
    //      BOOST_SPIRIT_DEBUG_RULE(includerule_p);
    //      BOOST_SPIRIT_DEBUG_RULE(loadrule_p);
    //      BOOST_SPIRIT_DEBUG_RULE(pathrule_p);
    //      BOOST_SPIRIT_DEBUG_RULE(param_name_p);
    //      BOOST_SPIRIT_DEBUG_RULE(module_name_p);
    //      BOOST_SPIRIT_DEBUG_RULE(true_p);
    //      BOOST_SPIRIT_DEBUG_RULE(false_p);
    //      BOOST_SPIRIT_DEBUG_RULE(flag_p);
    //      BOOST_SPIRIT_DEBUG_RULE(sc::as_lower_d[ "help" ]);
    //      BOOST_SPIRIT_DEBUG_RULE(delim_p);
    //      BOOST_SPIRIT_DEBUG_RULE(minus_p);
    //      BOOST_SPIRIT_DEBUG_RULE(assign_p);
    //      BOOST_SPIRIT_DEBUG_RULE(simple_str_p);
    //      BOOST_SPIRIT_DEBUG_RULE(qstr_p);
    //      BOOST_SPIRIT_DEBUG_RULE(qstr2_p);
    //      BOOST_SPIRIT_DEBUG_RULE(string_p);
    //      BOOST_SPIRIT_DEBUG_RULE(sc::int_p);
    //      BOOST_SPIRIT_DEBUG_RULE(includerule_p);

    sc::parse_info<> info = sc::parse( buffer.c_str(), input_p );
    if( ! info.full )
    {
        parseError( info.length, buffer );
    }
    return info.full;
}

}
}
