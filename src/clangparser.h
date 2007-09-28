// -*-c++-*-

/***************************************************************************
                               clangparser.h  
                          Parser for the Coach Lang
                             -------------------
    begin                : 16-FEB-2002
    copyright            : (C) 2002 by The RoboCup Soccer Server 
                           Maintainance Group.
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

#ifndef _CLANGPARSER_H_
#define _CLANGPARSER_H_


#include <string>
#include <rcssbase/parser.h>
#include "coach_lang_tok.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_SSTREAM
#include <sstream>
#else
#include <strstream>;
#endif


extern int RCSS_CLANG_parse( void * );

namespace rcss
{
  namespace clang
  {
    class Builder;
    
    class Parser
      : public rcss::Parser
    {
    public:
      typedef rcss::clang::Lexer Lexer;
      typedef int (*ParserFunc)( void * );

      class Param
      {
      private:
        Lexer M_lexer;
        Parser& M_parser;

      protected:
        Builder& M_builder;
      public:
        Param( Parser& parser, Builder& builder )
          : M_parser( parser ),
            M_builder( builder )
        {}
        
        Parser&
        getParser()
        { return M_parser; }
      
        Lexer&
        getLexer()
        { return M_lexer; }
        
        Builder&
        getBuilder()
        { return M_builder; }
      };

    private:
      Param M_param;
      ParserFunc M_parser;
      
      virtual
      bool
      doParse( std::istream& strm )
      {
        M_param.getLexer().switch_streams( &strm, &std::cerr );
        return M_parser( (void*)&M_param ) == 0;
      }
     
    public:
	  Parser( Builder& builder );      
      // convenience method
      int
      parse( const char* msg )
      {
#ifdef HAVE_SSTREAM
        std::istringstream strm( msg );
#else
        std::istrstream strm( msg );
#endif
        return ( rcss::Parser::parse( strm ) ? 0 : 1 ); 
      }
    };

  }
}


#endif
