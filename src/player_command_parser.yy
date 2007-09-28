/* -*-bison-*- */
/*
 *Copyright:

    Copyright (C) 2001 RoboCup Soccer Server Maintainance Group.
        Patrick Riley, Tom Howard, Itsuki Noda, Mikhail Prokopenko, Jan Wendler

    This file is part of SoccerServer.

    SoccerServer is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 *EndCopyright:
 */
%{
#include "pcombuilder.h"
#include "pcomparser.h"


#define    yyparse    RCSS_PCOM_parse

void yyerror( const char* s );
int yyerror( char* s );

#define YYPARSE_PARAM param

namespace
{
  rcss::pcom::Parser::Param&
  getParam( void* param )
  {
    static rcss::pcom::Parser::Param* cached_param
                   = reinterpret_cast< rcss::pcom::Parser::Param* >( param );
    if( cached_param != param ){
      cached_param = reinterpret_cast< rcss::pcom::Parser::Param* >( param );
    }
    return *cached_param;
  }

  inline rcss::pcom::Builder& getBuilder( void* param )
  {
    return getParam( param ).getBuilder();
  }

#define YYSTYPE rcss::pcom::Parser::Lexer::Holder

  inline int yylex( YYSTYPE* holder, rcss::pcom::Parser::Param& param )
  {
    int rval = param.getLexer().lex( *holder );
//    cout << rval << endl;
    return rval;
  }

}

#define YYLEX_PARAM getParam( param )

#define BUILDER getBuilder( param )

%}

/* BISON DECLARATIONS */
%pure_parser

%token RCSS_PCOM_INT
%token RCSS_PCOM_REAL
%token RCSS_PCOM_STR

%token RCSS_PCOM_LP "("
%token RCSS_PCOM_RP ")"

%token RCSS_PCOM_DASH "dash"
%token RCSS_PCOM_TURN "turn"
%token RCSS_PCOM_TURN_NECK "turn_neck"
%token RCSS_PCOM_KICK "kick"
%token RCSS_PCOM_CATCH "catch"
%token RCSS_PCOM_SAY "say"
%token RCSS_PCOM_UNQ_SAY "unquoted say"
%token RCSS_PCOM_SENSE_BODY "sense_body"
%token RCSS_PCOM_SCORE "score"
%token RCSS_PCOM_MOVE "move"
%token RCSS_PCOM_CHANGE_VIEW "change_view"
%token RCSS_PCOM_COMPRESSION "compression"
%token RCSS_PCOM_BYE "bye"
%token RCSS_PCOM_DONE "done"
%token RCSS_PCOM_POINTTO "pointto"
%token RCSS_PCOM_ATTENTIONTO "attentionto"
%token RCSS_PCOM_TACKLE "tackle"
%token RCSS_PCOM_CLANG "clang"
%token RCSS_PCOM_EAR "ear"

%token < rcss:pcom::VIEW_WIDTH > RCSS_PCOM_VIEW_WIDTH_NARROW "narrow"
%token < rcss:pcom::VIEW_WIDTH > RCSS_PCOM_VIEW_WIDTH_NORMAL "normal"
%token < rcss:pcom::VIEW_WIDTH > RCSS_PCOM_VIEW_WIDTH_WIDE "wide"
%token < rcss:pcom::VIEW_QUALITY > RCSS_PCOM_VIEW_QUALITY_LOW "low"
%token < rcss:pcom::VIEW_QUALITY > RCSS_PCOM_VIEW_QUALITY_HIGH "high"
%token < bool > RCSS_PCOM_ON "on"
%token < bool > RCSS_PCOM_OFF "off"
%token < rcss:pcom::TEAM > RCSS_PCOM_OUR "our"
%token < rcss:pcom::TEAM > RCSS_PCOM_OPP "opp"
%token < rcss:pcom::TEAM > RCSS_PCOM_LEFT 'l'
%token < rcss:pcom::TEAM > RCSS_PCOM_RIGHT 'r'
%token < rcss:pcom::EAR_MODE > RCSS_PCOM_EAR_PARTIAL "partial"
%token < rcss:pcom::EAR_MODE > RCSS_PCOM_EAR_COMPLETE "complete"
%token RCSS_PCOM_CLANG_VERSION "ver"

/* the ERROR token is an error from lex and is never used below */
%token RCSS_PCOM_ERROR

%start command_list

%%

/* Grammar RULES */

command_list : command
               | command_list command;

command : dash_com
          | turn_com
          | turn_neck_com
          | kick_com
          | catch_com
          | say_com
          | sense_body_com
          | score_com
          | move_com
          | change_view_com
          | compression_com
          | bye_com
          | done_com
          | pointto_com
          | attentionto_com
          | tackle_com
          | clang_com
          | ear_com ;

dash_com : RCSS_PCOM_LP RCSS_PCOM_DASH floating_point_number RCSS_PCOM_RP
           {
	       BUILDER.dash( $3.m_double );
           } ;

turn_com : RCSS_PCOM_LP RCSS_PCOM_TURN floating_point_number RCSS_PCOM_RP
           {
             BUILDER.turn( $3.m_double );
           } ;

turn_neck_com : RCSS_PCOM_LP RCSS_PCOM_TURN_NECK floating_point_number RCSS_PCOM_RP
                {
                  BUILDER.turn_neck( $3.m_double );
                } ;

kick_com : RCSS_PCOM_LP RCSS_PCOM_KICK floating_point_number floating_point_number RCSS_PCOM_RP
           {
             BUILDER.kick( $3.m_double, $4.m_double );
           } ;

catch_com : RCSS_PCOM_LP RCSS_PCOM_CATCH floating_point_number RCSS_PCOM_RP
            {
              BUILDER.goalieCatch( $3.m_double );
            } ;

say_com : RCSS_PCOM_UNQ_SAY
          {
            BUILDER.say( $1.m_str );
          }
          | RCSS_PCOM_LP RCSS_PCOM_SAY RCSS_PCOM_STR RCSS_PCOM_RP
          {
            BUILDER.say( rcss::stripQuotes( $3.m_str ) );
          } ;

sense_body_com : RCSS_PCOM_LP RCSS_PCOM_SENSE_BODY RCSS_PCOM_RP
                 {
                   BUILDER.sense_body();
                 } ;

score_com : RCSS_PCOM_LP RCSS_PCOM_SCORE RCSS_PCOM_RP
            {
              BUILDER.score();
            } ;

move_com : RCSS_PCOM_LP RCSS_PCOM_MOVE floating_point_number floating_point_number RCSS_PCOM_RP
           {
             BUILDER.move( $3.m_double, $4.m_double );
           } ;

change_view_com : RCSS_PCOM_LP RCSS_PCOM_CHANGE_VIEW view_width view_quality RCSS_PCOM_RP
                 {
                   BUILDER.change_view( $3.m_view_w, $4.m_view_q );
                 } ;

view_width : RCSS_PCOM_VIEW_WIDTH_NARROW
             {
               $$.m_view_w = rcss::pcom::NARROW;
             }
             | RCSS_PCOM_VIEW_WIDTH_NORMAL
             {
               $$.m_view_w = rcss::pcom::NORMAL;
             }
             | RCSS_PCOM_VIEW_WIDTH_WIDE
             {
               $$.m_view_w = rcss::pcom::WIDE;
             } ;

view_quality : RCSS_PCOM_VIEW_QUALITY_LOW
               {
                 $$.m_view_q = rcss::pcom::LOW;
               }
               | RCSS_PCOM_VIEW_QUALITY_HIGH
               {
                 $$.m_view_q = rcss::pcom::HIGH;
               };

compression_com : RCSS_PCOM_LP RCSS_PCOM_COMPRESSION RCSS_PCOM_INT RCSS_PCOM_RP
                  {
                    BUILDER.compression( $3.m_int );
                  } ;

bye_com : RCSS_PCOM_LP RCSS_PCOM_BYE RCSS_PCOM_RP
          {
            BUILDER.bye();
          } ;

done_com : RCSS_PCOM_LP RCSS_PCOM_DONE RCSS_PCOM_RP
           {
             BUILDER.done();
           } ;

pointto_com : RCSS_PCOM_LP RCSS_PCOM_POINTTO floating_point_number floating_point_number RCSS_PCOM_RP
              {
                BUILDER.pointto( true, $3.m_double, $4.m_double );
              }
              | RCSS_PCOM_LP RCSS_PCOM_POINTTO RCSS_PCOM_OFF RCSS_PCOM_RP
              {
                BUILDER.pointto( false, 0.0, 0.0 );
              } ;

attentionto_com : RCSS_PCOM_LP RCSS_PCOM_ATTENTIONTO team_side RCSS_PCOM_INT RCSS_PCOM_RP
                  {
                    BUILDER.attentionto( true, $3.m_team, "", $4.m_int );
                  }
                  | RCSS_PCOM_LP RCSS_PCOM_ATTENTIONTO RCSS_PCOM_STR RCSS_PCOM_INT RCSS_PCOM_RP
                  {
                    BUILDER.attentionto( true, rcss::pcom::UNKNOWN_TEAM, $3.m_str, $4.m_int );
                  }
                  | RCSS_PCOM_LP RCSS_PCOM_ATTENTIONTO RCSS_PCOM_OFF RCSS_PCOM_RP
                  {
                    BUILDER.attentionto( false, rcss::pcom::UNKNOWN_TEAM, "", 0 );
                  } ;

tackle_com : RCSS_PCOM_LP RCSS_PCOM_TACKLE floating_point_number RCSS_PCOM_RP
             {
               BUILDER.tackle( $3.m_double );
             } ;

clang_com : RCSS_PCOM_LP RCSS_PCOM_CLANG RCSS_PCOM_LP RCSS_PCOM_CLANG_VERSION RCSS_PCOM_INT RCSS_PCOM_INT RCSS_PCOM_RP RCSS_PCOM_RP
           {
             BUILDER.clang( $5.m_int, $6.m_int );
           } ;

ear_com : RCSS_PCOM_LP RCSS_PCOM_EAR RCSS_PCOM_LP on_off team_side partial_complete RCSS_PCOM_RP RCSS_PCOM_RP
          {
            BUILDER.ear( $4.m_bool, $5.m_team, "", $6.m_ear );
          }
          | RCSS_PCOM_LP RCSS_PCOM_EAR RCSS_PCOM_LP on_off RCSS_PCOM_STR partial_complete RCSS_PCOM_RP RCSS_PCOM_RP
          {
            BUILDER.ear( $4.m_bool, rcss::pcom::UNKNOWN_TEAM, $5.m_str, $6.m_ear );
          }
          | RCSS_PCOM_LP RCSS_PCOM_EAR RCSS_PCOM_LP on_off team_side RCSS_PCOM_RP RCSS_PCOM_RP
          {
            BUILDER.ear( $4.m_bool, $5.m_team, "", rcss::pcom::UNKNOWN_EAR_MODE );
          }
          | RCSS_PCOM_LP RCSS_PCOM_EAR RCSS_PCOM_LP on_off RCSS_PCOM_STR RCSS_PCOM_RP RCSS_PCOM_RP
          {
            BUILDER.ear( $4.m_bool, rcss::pcom::UNKNOWN_TEAM, $5.m_str, rcss::pcom::UNKNOWN_EAR_MODE );
          }
          | RCSS_PCOM_LP RCSS_PCOM_EAR RCSS_PCOM_LP on_off partial_complete RCSS_PCOM_RP RCSS_PCOM_RP
          {
            BUILDER.ear( $4.m_bool, rcss::pcom::UNKNOWN_TEAM, "", $5.m_ear );
          }
          | RCSS_PCOM_LP RCSS_PCOM_EAR RCSS_PCOM_LP on_off RCSS_PCOM_RP RCSS_PCOM_RP
          {
            BUILDER.ear( $4.m_bool, rcss::pcom::UNKNOWN_TEAM, "", rcss::pcom::UNKNOWN_EAR_MODE );
          }
          ;

on_off : RCSS_PCOM_ON
         {
           $$.m_bool = true;
         }
         | RCSS_PCOM_OFF
         {
           $$.m_bool = false;
         } ;

team_side : RCSS_PCOM_OUR
            {
              $$.m_team = rcss::pcom::OUR;
            }
            | RCSS_PCOM_OPP
            {
              $$.m_team = rcss::pcom::OPP;
            }
            | RCSS_PCOM_LEFT
            {
              $$.m_team = rcss::pcom::LEFT_SIDE;
            }
            | RCSS_PCOM_RIGHT
            {
              $$.m_team = rcss::pcom::RIGHT_SIDE;
            } ;

partial_complete : RCSS_PCOM_EAR_PARTIAL
                   {
                     $$.m_ear = rcss::pcom::PARTIAL;
                   }
                   | RCSS_PCOM_EAR_COMPLETE
                   {
                     $$.m_ear = rcss::pcom::COMPLETE;
                   } ;

floating_point_number : RCSS_PCOM_INT
                        {
                          $$.m_double = static_cast<double>( $1.m_int );
                        }
                        | RCSS_PCOM_REAL
                        {
                          $$ = $1;
                        } ;

%%


void yyerror (const char* s)
{
  std::cerr << s << std::endl;
  //do nothing
}

int yyerror (char* s)
{
  yyerror ( (const char*)s );
  return 0;
}
