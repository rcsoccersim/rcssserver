// -*-c++-*-

/***************************************************************************
                               clangmsgbuilder.h
                      Class for building a clang message
                             -------------------
    begin                : 25-FEB-2002
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

#ifndef CLANGMSGBUILDER_H
#define CLANGMSGBUILDER_H

#include "clangbuilder.h"
#include <stack>
#include <memory>
#include <list>

#ifdef DEBUG
#include <typeinfo>
#include <iostream>
#endif

namespace rcss {
namespace clang {

class Msg;

class MetaToken;
class Action;
class Dir;
class Cond;
class Token;
class Def;
class Region;
class Point;
class UNum;
class UNumSet;
class RuleIDList;
class ActivateRules;
class Rule;

typedef std::string RuleID;


class MsgBuilder
    : public Builder
{
private:
    std::auto_ptr< Msg > M_msg;
    unsigned int M_min_ver;
    unsigned int M_max_ver;
    unsigned int M_str_var_size;
    unsigned int M_freeform_msg_size;

public:
    typedef std::list< Cond* > CondList;
    enum Types {
        NONE,
        META,
        ACTION,
        DIR,
        COND,
        CONDLIST,
        TOKEN,
        DEF,
        REG,
        POINT,
        UNUM,
        BMT,
        UNUMSET,
        STR,
        RIDLIST,
        ACTIVATE,
        RULE,
        RID
    };

protected:

    class ItemType {
    public:
        ItemType()
            : m_type( NONE )
          { }

        ~ItemType()
          { }

        void clear();


        union Items
        {
            MetaToken* m_meta_token;
            Action* m_action;
            Dir* m_dir;
            Cond* m_cond;
            CondList* m_cond_list;
            Token* m_token;
            Def* m_def;
            Region* m_region;
            Point* m_point;
            UNum* m_unum;
            BallMoveToken m_bmt;
            UNumSet* m_unum_set;
            std::string* m_str;
            RuleIDList* m_rule_id_list;
            ActivateRules* m_activate;
            Rule* m_rule;
            RuleID* m_rid;
        };

        Types m_type;
        Items m_item;
    };

    std::stack< ItemType > M_items;

    std::auto_ptr< MetaToken > getMetaToken();
    std::auto_ptr< Action > getAction();
    std::auto_ptr< Dir > getDir();
    std::auto_ptr< Cond > getCond();
    std::auto_ptr< CondList > getCondList();
    std::auto_ptr< Token > getToken();
    std::auto_ptr< Def > getDef();
    std::auto_ptr< Region > getRegion();
    std::auto_ptr< Point > getPoint();
    std::auto_ptr< UNum > getUNum();
    BallMoveToken getBMT();
    std::auto_ptr< UNumSet > getUNumSet();
    std::auto_ptr< std::string > getStr();
    std::auto_ptr< RuleIDList > getRIDList();
    std::auto_ptr< ActivateRules > getActivateRules();
    std::auto_ptr< Rule > getRule();
    std::auto_ptr< RuleID > getRID();
    bool isItem( Types type ) const;
    bool checkIsItem( Types type ) const;

    std::auto_ptr< MetaToken > checkAndGetMetaToken();
    std::auto_ptr< Action > checkAndGetAction();
    std::auto_ptr< Dir > checkAndGetDir();
    std::auto_ptr< Cond > checkAndGetCond();
    std::auto_ptr< CondList > checkAndGetCondList();
    std::auto_ptr< Token > checkAndGetToken();
    std::auto_ptr< Def > checkAndGetDef();
    std::auto_ptr< Region > checkAndGetRegion();
    std::auto_ptr< Point > checkAndGetPoint();
    std::auto_ptr< UNum > checkAndGetUNum();
    BallMoveToken checkAndGetBMT();
    std::auto_ptr< UNumSet > checkAndGetUNumSet();
    std::auto_ptr< std::string > checkAndGetStr();
    std::auto_ptr< RuleIDList > checkAndGetRIDList();
    std::auto_ptr< ActivateRules > checkAndGetActivateRules();
    std::auto_ptr< Rule > checkAndGetRule();
    RuleID checkAndGetRID();

    void add( MetaToken * meta );
    void add( Action * action );
    void add( Dir * dir );
    void add( Cond * cond );
    void add( CondList * cond_list );
    void add( Token * token );
    void add( Def * def );
    void add( Region * region );
    void add( Point * point );
    void add( UNum * unum );
    void add( BallMoveToken bmt );
    void add( UNumSet * unum_set );
    void add( std::string * str );
    void add( RuleIDList * rid_list );
    void add( ActivateRules * act_rules );
    void add( Rule * rule );
    void add( RuleID rid );

    void checkItemsEmpty() const;
    void setMsg( Msg * msg );
    void emptyStack();
    void clear();
    void onNoItems() const;
    void onWrongItem() const;
    void onNotEmpty() const;

public:
    MsgBuilder();

    virtual
    ~MsgBuilder();

    Msg * getMsg();

    const Msg * getMsg() const;

    std::auto_ptr< Msg > detatchMsg();

    virtual
    void setVer( const unsigned int & min,
                 const unsigned int & max );

    virtual
    void setStrVarSize( const unsigned int len ) { M_str_var_size = len; }

    virtual
    unsigned int strVarSize() const { return M_str_var_size; }

    virtual
    void setFreeformMsgSize( const unsigned int len ) { M_freeform_msg_size = len; }

    virtual
    unsigned int freeformMsgSize() const { return M_freeform_msg_size; }

    virtual
    void setTime( const int & time );

    virtual
    void setSide( const int & side );

    virtual
    void setTimeRecv( const int & time );

    virtual
    void buildMetaMsg();

    virtual
    void buildMetaTokenVer( const double & ver );

    virtual
    void buildDefineMsg();

    virtual
    void buildDefineCond( const std::string & name );

    virtual
    void buildDefineDir( const std::string & name );

    virtual
    void buildDefineReg( const std::string & name );

    virtual
    void buildDefineAct( const std::string & name );

    virtual
    void buildFreeformMsg( const std::string & str );

    virtual
    void buildUnsuppMsg();

    virtual
    void buildInfoMsg();

    virtual
    void buildAdviceMsg();

    virtual
    void buildTokenRule( const int & ttl );

    virtual
    void buildTokenClear();

    virtual
    void buildActPos();

    virtual
    void buildActHome();

    virtual
    void buildActBallToReg();

    virtual
    void buildActBallToPlayer();

    virtual
    void buildActMark();

    virtual
    void buildActMarkLinePlayer();

    virtual
    void buildActMarkLineReg();

    virtual
    void buildActOffsideLine();

    virtual
    void buildActHetType( const int & type );

    virtual
    void buildActNamed( const std::string & name );

    virtual
    void buildActPassReg();

    virtual
    void buildActPassUNum();

    virtual
    void buildActDribble();

    virtual
    void buildActClear();

    virtual
    void buildActShoot();

    virtual
    void buildActHold();

    virtual
    void buildActIntercept();

    virtual
    void buildActTackle();

    virtual
    void buildDirComm( const bool & do_dont,
                       const bool & our_side );

    virtual
    void buildDirNamed( const std::string& name );

    virtual
    void buildCondTrue();

    virtual
    void buildCondFalse();

    virtual
    void buildCondPlayerPos( const bool & our_side,
                             const int & min,
                             const int & max );

    virtual
    void buildCondBallPos();

    virtual
    void buildCondBallOwner( const bool & our_side );

    virtual
    void buildCondPlayMode( const PlayMode & play_mode );

    virtual
    void buildCondAnd();

    virtual
    void buildCondOr();

    virtual
    void buildCondNot();

    virtual
    void buildCondNamed( const std::string & name );

    virtual
    void buildCondTime( const int & time,
                        const util::CompOp & comp );

    virtual
    void buildCondOppGoal( const int & goals,
                           const util::CompOp & comp );

    virtual
    void buildCondOurGoal( const int & goals,
                           const util::CompOp & comp );

    virtual
    void buildCondGoalDiff( const int & goals,
                            const util::CompOp & comp );

    virtual
    void buildCondUNum( const rcss::clang::UNum & unum );

    virtual
    void buildAddToCondList();

    virtual
    void buildCreateCondList();

    virtual
    void buildRegNull();

    virtual
    void buildRegQuad();

    virtual
    void buildRegArc( const double & start_rad,
                      const double & end_rad,
                      const double & start_ang,
                      const double & span_ang );

    virtual
    void buildRegUnion();

    virtual
    void buildRegNamed( const std::string & name );

    virtual
    void buildRegPoint();

    virtual
    void buildRegTri();

    virtual
    void buildRegRec();

    virtual
    void buildPointSimple( const double & x,
                           const double & y );

    virtual
    void buildPointRel( const double & x,
                        const double & y );

    virtual
    void buildPointBall();

    virtual
    void buildPointPlayer( const bool & our_side,
                           const UNum & unum );

    virtual
    void buildPointArith( const rcss::util::ArithOp & arith_op );

    virtual
    void buildUNum( const UNum & unum );

    virtual
    void buildUNumSet();

    virtual
    void buildBallMoveToken( const BallMoveToken & bmt );

    virtual
    void buildRuleMsg();

    virtual
    void buildActivateAllRules( const bool & on );

    virtual
    void buildActivateRules( const bool & on );

    virtual
    void buildRuleID( const std::string & id );

    virtual
    void buildRuleIDList();

    virtual
    void buildRuleIDListALL();

    virtual
    void buildDelMsg();

    virtual
    void buildDefineModelRule( const std::string & id );

    virtual
    void buildDefineDirectiveRule( const std::string & id );

    virtual
    void buildSimpleRule();

    virtual
    void buildNestedRule();

    virtual
    void buildIDRule();
};

}
}

#endif
