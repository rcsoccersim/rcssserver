// -*-c++-*-

/***************************************************************************
                                 pcomlexer.h
                          Lexer for PlayerCommands
                        ----------------------------
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

#ifndef RCSS_PCOMLEXER_H
#define RCSS_PCOMLEXER_H

#include "pcombuilder.h"


#undef yyFlexLexer
#define yyFlexLexer RCSSPComFlexLexer
//#include <rcssbase/FlexLexer.h>
#include <FlexLexer.h>

class RCSSPComLexer
    : public RCSSPComFlexLexer {
public:

    union Holder {
        enum { STR_MAX = 8192 };

        int m_int;
        double m_double;
        char m_str[ STR_MAX ];
        bool m_bool;
        rcss::pcom::VIEW_WIDTH m_view_w;
        rcss::pcom::VIEW_QUALITY m_view_q;
        rcss::pcom::TEAM m_team;
        rcss::pcom::EAR_MODE m_ear;
    };


    virtual
    int yylex(); // defined in player_command_tok.cc

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
namespace pcom {
typedef RCSSPComLexer Lexer;
}
}

#endif
