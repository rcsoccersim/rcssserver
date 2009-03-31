// -*-c++-*-

/***************************************************************************
                                 clanglexer.h
                               Lexer for CLang
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

#ifndef CLANGLEXER_H
#define CLANGLEXER_H

#include "compop.h"
#include "arithop.h"
#include "clangutil.h"

#include <cstring>

#undef yyFlexLexer
#define yyFlexLexer RCSSCLangFLexLexer
//#include <rcssbase/FlexLexer.h>
#include <FlexLexer.h>

#define CLANG_MAX_STR 8192

class RCSSCLangLexer
    : public RCSSCLangFLexLexer {
public:
    struct Holder {
        union {
            int int_val;
            double doub_val;
            rcss::clang::BallMoveToken bmt;
            rcss::clang::PlayMode pm;
            bool bool_val;
            const rcss::util::CompOp * co;
            const rcss::util::ArithOp * ao;
            char str_val[ CLANG_MAX_STR+1 ];
        };

        int operator=( int value )
          {
              return int_val = value;
          }

        double operator=( double value )
          {
              return doub_val = value;
          }

        rcss::clang::BallMoveToken operator=( rcss::clang::BallMoveToken value )
          {
              return bmt = value;
          }

        rcss::clang::PlayMode operator=( rcss::clang::PlayMode value )
          {
              return pm = value;
          }

        bool operator=( bool value )
          {
              return bool_val = value;
          }

        const
        rcss::util::CompOp & operator=( const rcss::util::CompOp & value )
          {
              return *(co = &value);
          }

        const
        rcss::util::ArithOp & operator=( const rcss::util::ArithOp & value )
          {
              return *(ao = &value);
          }

        const char * operator=( const char * value )
          {
              std::strncpy( str_val, value, CLANG_MAX_STR );
              return str_val;
          }

        int getInt() const
          {
              return int_val;
          }

        double getDoub() const
          {
              return doub_val;
          }

        rcss::clang::BallMoveToken getBMT() const
          {
              return bmt;
          }

        rcss::clang::PlayMode getPM() const
          {
              return pm;
          }

        bool getBool() const
          {
              return bool_val;
          }

        const
        rcss::util::CompOp & getCompOp() const
          {
              return *co;
          }

        const
        rcss::util::ArithOp & getArithOp() const
          {
              return *ao;
          }

        const char * getStr() const
          {
              return str_val;
          }
    };

    virtual
    int yylex(); // defined in coach_lang_tok.cc

    inline
    int lex( Holder & holder )
      {
          M_lexed_val = &holder;
          return yylex();
      }

private:
    Holder * M_lexed_val;
};

namespace rcss {
namespace clang {
typedef RCSSCLangLexer Lexer;
}
}

#endif
