// -*-c++-*-

/***************************************************************************
                               clangmsgbuilder.cc
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "clangmsgbuilder.h"

#include "region.h"
#include "clangmsg.h"
#include "clangmetamsg.h"
#include "clangfreeformmsg.h"
#include "clangunsuppmsg.h"
#include "clangrulemsg.h"
#include "clangdelmsg.h"
#include "clanginfomsg.h"
#include "clangadvicemsg.h"
#include "clangdefmsg.h"
#include "coach_lang_comp.h"
#include "rule.h"


namespace rcss {
namespace clang {

namespace {
const size_t STR_SIZE = 8;
}


template< typename X >
inline
void
traceBuild()
{
#ifdef DEBUG
    std::cout << "Building: " << typeid( X ).name() << std::endl;
#endif
}

template< typename X >
inline
void
traceRemoving()
{
#ifdef DEBUG
    std::cout << "Removing: " << typeid( X ).name() << std::endl;
#endif
}

template< typename X >
inline
void
traceAdding()
{
#ifdef DEBUG
    std::cout << "Adding: " << typeid( X ).name() << std::endl;
#endif
}


void
MsgBuilder::ItemType::clear()
{
    switch ( m_type ) {
    case NONE:
        break;
    case META:
        delete m_item.m_meta_token;
        break;
    case ACTION:
        delete m_item.m_action;
        break;
    case DIR:
        delete m_item.m_dir;
        break;
    case COND:
        delete m_item.m_cond;
        break;
    case CONDLIST:
        delete m_item.m_cond_list;
        break;
    case TOKEN:
        delete m_item.m_token;
        break;
    case DEF:
        delete m_item.m_def;
        break;
    case REG:
        delete m_item.m_region;
        break;
    case POINT:
        delete m_item.m_point;
        break;
    case UNUM:
        delete m_item.m_unum;
        break;
    case BMT:
        break;
    case UNUMSET:
        delete m_item.m_unum_set;
        break;
    case STR:
        delete m_item.m_str;
        break;
    case RIDLIST:
        delete m_item.m_rule_id_list;
        break;
    case ACTIVATE:
        delete m_item.m_activate;
        break;
    case RULE:
        delete m_item.m_rule;
        break;
    case RID:
        break;
    }
}

std::auto_ptr< MetaToken >
MsgBuilder::getMetaToken()
{
    traceRemoving< MetaToken >();
    std::auto_ptr< MetaToken > rval( M_items.top().m_item.m_meta_token );
    //M_items.top().clear();
    M_items.pop();
    return rval;
}


std::auto_ptr< Action >
MsgBuilder::getAction()
{
    traceRemoving< Action >();
    std::auto_ptr< Action > rval( M_items.top().m_item.m_action );
    //M_items.top().clear();
    M_items.pop();
    return rval;
}

std::auto_ptr< Dir >
MsgBuilder::getDir()
{
    traceRemoving< Dir >();
    std::auto_ptr< Dir > rval( M_items.top().m_item.m_dir );
    //M_items.top().clear();
    M_items.pop();
    return rval;
}


std::auto_ptr< Cond >
MsgBuilder::getCond()
{
    traceRemoving< Cond >();
    std::auto_ptr< Cond > rval( M_items.top().m_item.m_cond );
    //M_items.top().clear();
    M_items.pop();
    return rval;
}


std::auto_ptr< MsgBuilder::CondList >
MsgBuilder::getCondList()
{
    traceRemoving< CondList >();
    std::auto_ptr< CondList > rval( M_items.top().m_item.m_cond_list );
    //M_items.top().clear();
    M_items.pop();
    return rval;
}


std::auto_ptr< Token >
MsgBuilder::getToken()
{
    traceRemoving< Token >();
    std::auto_ptr< Token > rval( M_items.top().m_item.m_token );
    //M_items.top().clear();
    M_items.pop();
    return rval;
}


std::auto_ptr< Def >
MsgBuilder::getDef()
{
    traceRemoving< Def >();
    std::auto_ptr< Def > rval( M_items.top().m_item.m_def );
    //M_items.top().clear();
    M_items.pop();
    return rval;
}


std::auto_ptr< Region >
MsgBuilder::getRegion()
{
    traceRemoving< Region >();
    std::auto_ptr< Region > rval( M_items.top().m_item.m_region );
    //M_items.top().clear();
    M_items.pop();
    return rval;
}


std::auto_ptr< Point >
MsgBuilder::getPoint()
{
    traceRemoving< Point >();
    std::auto_ptr< Point > rval( M_items.top().m_item.m_point );
    //M_items.top().clear();
    M_items.pop();
    return rval;
}


std::auto_ptr< UNum >
MsgBuilder::getUNum()
{
    traceRemoving< UNum >();
    std::auto_ptr< UNum > rval( M_items.top().m_item.m_unum );
    //M_items.top().clear();
    M_items.pop();
    return rval;
}


BallMoveToken
MsgBuilder::getBMT()
{
    traceRemoving< BallMoveToken >();
    BallMoveToken rval( M_items.top().m_item.m_bmt );
    //M_items.top().clear();
    M_items.pop();
    return rval;
}


std::auto_ptr< UNumSet >
MsgBuilder::getUNumSet()
{
    traceRemoving< UNumSet >();
    std::auto_ptr< UNumSet > rval( M_items.top().m_item.m_unum_set );
    //M_items.top().clear();
    M_items.pop();
    return rval;
}


std::auto_ptr< std::string >
MsgBuilder::getStr()
{
    traceRemoving< std::string >();
    std::auto_ptr< std::string > rval( M_items.top().m_item.m_str );
    //M_items.top().clear();
    M_items.pop();
    return rval;
}


std::auto_ptr< RuleIDList >
MsgBuilder::getRIDList()
{
    traceRemoving< RuleIDList >();
    std::auto_ptr< RuleIDList > rval( M_items.top().m_item.m_rule_id_list );
    //M_items.top().clear();
    M_items.pop();
    return rval;
}


std::auto_ptr< ActivateRules >
MsgBuilder::getActivateRules()
{
    traceRemoving< ActivateRules >();
    std::auto_ptr< ActivateRules > rval( M_items.top().m_item.m_activate );
    //M_items.top().clear();
    M_items.pop();
    return rval;
}


std::auto_ptr< Rule >
MsgBuilder::getRule()
{
    traceRemoving< Rule >();
    std::auto_ptr< Rule > rval( M_items.top().m_item.m_rule );
    //M_items.top().clear();
    M_items.pop();
    return rval;
}

std::auto_ptr< RuleID >
MsgBuilder::getRID()
{
    traceRemoving< RuleID >();
    std::auto_ptr< RuleID > rval( M_items.top().m_item.m_rid );
    //M_items.top().clear();
    M_items.pop();
    return rval;
}

bool
MsgBuilder::isItem( Types type ) const
{
    return ( ! M_items.empty()
             && M_items.top().m_type == type );
}

bool
MsgBuilder::checkIsItem( Types type ) const
{
    if ( !M_items.empty() )
    {
        if ( M_items.top().m_type == type )
        {
            return true;
        }
        else
        {
            onWrongItem();
        }
    }
    else
    {
        onNoItems();
    }
    return false;
}


std::auto_ptr< MetaToken >
MsgBuilder::checkAndGetMetaToken()
{
    if ( checkIsItem( META ) )
    {
        return getMetaToken();
    }
    // no need for error handling as this is done by checkIsItem()
    return std::auto_ptr< MetaToken >();
}


std::auto_ptr< Action >
MsgBuilder::checkAndGetAction()
{
    if ( checkIsItem( ACTION ) )
    {
        return getAction();
    }
    // no need for error handling as this is done by checkIsItem()
    return std::auto_ptr< Action >();
}

std::auto_ptr< Dir >
MsgBuilder::checkAndGetDir()
{
    if ( checkIsItem( DIR ) )
    {
        return getDir();
    }
    // no need for error handling as this is done by checkIsItem()
    return std::auto_ptr< Dir >();
}


std::auto_ptr< Cond >
MsgBuilder::checkAndGetCond()
{
    if ( checkIsItem( COND ) )
    {
        return getCond();
    }
    // no need for error handling as this is done by checkIsItem()
    return std::auto_ptr< Cond >();
}


std::auto_ptr< MsgBuilder::CondList >
MsgBuilder::checkAndGetCondList()
{
    if ( checkIsItem( CONDLIST ) )
    {
        return getCondList();
    }
    // no need for error handling as this is done by checkIsItem()
    return std::auto_ptr< CondList >( new CondList() );
}


std::auto_ptr< Token >
MsgBuilder::checkAndGetToken()
{
    if ( checkIsItem( TOKEN ) )
    {
        return getToken();
    }
    // no need for error handling as this is done by checkIsItem()
    return std::auto_ptr< Token >();
}


std::auto_ptr< Def >
MsgBuilder::checkAndGetDef()
{
    if ( checkIsItem( DEF ) )
    {
        return getDef();
    }
    // no need for error handling as this is done by checkIsItem()
    return std::auto_ptr< Def >();
}


std::auto_ptr< Region >
MsgBuilder::checkAndGetRegion()
{
    if ( checkIsItem( REG ) )
    {
        return getRegion();
    }
    // no need for error handling as this is done by checkIsItem()
    return std::auto_ptr< Region >();
}


std::auto_ptr< Point >
MsgBuilder::checkAndGetPoint()
{
    if ( checkIsItem( POINT ) )
    {
        return getPoint();
    }
    // no need for error handling as this is done by checkIsItem()
    return std::auto_ptr< Point >();
}


std::auto_ptr< UNum >
MsgBuilder::checkAndGetUNum()
{
    if ( checkIsItem( UNUM ) )
    {
        return getUNum();
    }
    // no need for error handling as this is done by checkIsItem()
    return std::auto_ptr< UNum >( new UNum() );
}


BallMoveToken
MsgBuilder::checkAndGetBMT()
{
    if ( checkIsItem( BMT ) )
    {
        return getBMT();
    }
    // no need for error handling as this is done by checkIsItem()
    return BallMoveToken();
}


std::auto_ptr< UNumSet >
MsgBuilder::checkAndGetUNumSet()
{
    if ( checkIsItem( UNUMSET ) )
    {
        return getUNumSet();
    }
    // no need for error handling as this is done by checkIsItem()
    return std::auto_ptr< UNumSet >( new UNumSet() );
}


std::auto_ptr< std::string >
MsgBuilder::checkAndGetStr()
{
    if ( checkIsItem( STR ) )
    {
        return getStr();
    }
    // no need for error handling as this is done by checkIsItem()
    return std::auto_ptr< std::string >( new std::string() );
}


std::auto_ptr< RuleIDList >
MsgBuilder::checkAndGetRIDList()
{
    if ( checkIsItem( RIDLIST ) )
    {
        return getRIDList();
    }
    // no need for error handling as this is done by checkIsItem()
    return std::auto_ptr< RuleIDList >();
}


std::auto_ptr< ActivateRules >
MsgBuilder::checkAndGetActivateRules()
{
    if ( checkIsItem( ACTIVATE ) )
    {
        return getActivateRules();
    }
    // no need for error handling as this is done by checkIsItem()
    return std::auto_ptr< ActivateRules >();
}


std::auto_ptr< Rule >
MsgBuilder::checkAndGetRule()
{
    if ( checkIsItem( RULE ) )
    {
        return getRule();
    }
    // no need for error handling as this is done by checkIsItem()
    return std::auto_ptr< Rule >();
}

RuleID
MsgBuilder::checkAndGetRID()
{
    if ( checkIsItem( RID ) )
    {
        return *getRID();
    }
    // no need for error handling as this is done by checkIsItem()
    return RuleID();
}

void MsgBuilder::add( MetaToken * meta )
{
    traceAdding< MetaToken >();
    ItemType tmp;
    tmp.m_item.m_meta_token = meta;
    tmp.m_type = META;
    M_items.push( tmp );
}

void MsgBuilder::add( Action * action )
{
    traceAdding< Action >();
    ItemType tmp;
    tmp.m_item.m_action = action;
    tmp.m_type = ACTION;
    M_items.push( tmp );
}

void MsgBuilder::add( Dir * dir )
{
    traceAdding< Dir >();
    ItemType tmp;
    tmp.m_item.m_dir = dir;
    tmp.m_type = DIR;
    M_items.push( tmp );
}

void MsgBuilder::add( Cond * cond )
{
    traceAdding< Cond >();
    ItemType tmp;
    tmp.m_item.m_cond = cond;
    tmp.m_type = COND;
    M_items.push( tmp );
}

void MsgBuilder::add( MsgBuilder::CondList * cond_list )
{
    traceAdding< CondList >();
    ItemType tmp;
    tmp.m_item.m_cond_list = cond_list;
    tmp.m_type = CONDLIST;
    M_items.push( tmp );
}

void MsgBuilder::add( Token * token )
{
    traceAdding< Token >();
    ItemType tmp;
    tmp.m_item.m_token = token;
    tmp.m_type = TOKEN;
    M_items.push( tmp );
}

void MsgBuilder::add( Def * def )
{
    traceAdding< Def >();
    ItemType tmp;
    tmp.m_item.m_def = def;
    tmp.m_type = DEF;
    M_items.push( tmp );
}

void MsgBuilder::add( Region * region )
{
    traceAdding< Region >();
    ItemType tmp;
    tmp.m_item.m_region = region;
    tmp.m_type = REG;
    M_items.push( tmp );
}

void MsgBuilder::add( Point * point )
{
    traceAdding< Point >();
    ItemType tmp;
    tmp.m_item.m_point = point;
    tmp.m_type = POINT;
    M_items.push( tmp );
}

void MsgBuilder::add( UNum * unum )
{
    traceAdding< UNum >();
    ItemType tmp;
    tmp.m_item.m_unum = unum;
    tmp.m_type = UNUM;
    M_items.push( tmp );
}

void MsgBuilder::add( BallMoveToken bmt )
{
    traceAdding< BallMoveToken >();
    ItemType tmp;
    tmp.m_item.m_bmt = bmt;
    tmp.m_type = BMT;
    M_items.push( tmp );
}

void MsgBuilder::add( UNumSet * unum_set )
{
    traceAdding< UNumSet >();
    ItemType tmp;
    tmp.m_item.m_unum_set = unum_set;
    tmp.m_type = UNUMSET;
    M_items.push( tmp );
}

void MsgBuilder::add( std::string * str )
{
    traceAdding< std::string >();
    ItemType tmp;
    tmp.m_item.m_str = str;
    tmp.m_type = STR;
    M_items.push( tmp );
}

void MsgBuilder::add( RuleIDList * rid_list )
{
    traceAdding< RuleIDList >();
    ItemType tmp;
    tmp.m_item.m_rule_id_list = rid_list;
    tmp.m_type = RIDLIST;
    M_items.push( tmp );
}

void
MsgBuilder::add( ActivateRules * act_rules )
{
    traceAdding< ActivateRules >();
    ItemType tmp;
    tmp.m_item.m_activate = act_rules;
    tmp.m_type = ACTIVATE;
    M_items.push( tmp );
}

void
MsgBuilder::add( Rule * rule )
{
    traceAdding< Rule >();
    ItemType tmp;
    tmp.m_item.m_rule = rule;
    tmp.m_type = RULE;
    M_items.push( tmp );
}

void
MsgBuilder::add( RuleID rid )
{
    traceAdding< BallMoveToken >();
    ItemType tmp;
    tmp.m_item.m_rid = new RuleID( rid );
    tmp.m_type = RID;
    M_items.push( tmp );
}


void
MsgBuilder::emptyStack()
{
    while ( ! M_items.empty() )
    {
        M_items.top().clear();
        M_items.pop();
    }
}

void
MsgBuilder::clear()
{
    M_msg.release();
    M_min_ver = (unsigned int)-1;
    M_max_ver = 0;
}

void
MsgBuilder::onNoItems() const
{
    throw BuilderErr( __FILE__, __LINE__,
                      "No item on stack." );
}

void
MsgBuilder::onWrongItem() const
{
    throw BuilderErr( __FILE__, __LINE__,
                      "Wrong item on stack." );
}

void
MsgBuilder::onNotEmpty() const
{
    throw BuilderErr( __FILE__, __LINE__,
                      "Stack is not empty when it should be." );
}

MsgBuilder::MsgBuilder()
    : M_min_ver( (unsigned int)-1 ),
      M_max_ver( 0 ),
      M_str_var_size( 8 ),
      M_freeform_msg_size( 128 )
{

}

MsgBuilder::~MsgBuilder()
{
    emptyStack();
}

Msg*
MsgBuilder::getMsg()
{
    return M_msg.get();
}

const Msg *
MsgBuilder::getMsg() const
{
    return M_msg.get();
}

std::auto_ptr< Msg >
MsgBuilder::detatchMsg()
{
    return M_msg;
}


void
MsgBuilder::checkItemsEmpty() const
{
    if ( !M_items.empty() )
    {
        onNotEmpty();
    }
}


void
MsgBuilder::setMsg( Msg* msg )
{
    std::auto_ptr< Msg > tmp( msg );
    M_msg = tmp;
    setVer( M_min_ver, M_max_ver );
    checkItemsEmpty();
}


void
MsgBuilder::setTime( const int & time )
{
    if ( M_msg.get() != NULL )
    {
        M_msg->setTimeSend( time );
    }
}


void
MsgBuilder::setSide( const int & side )
{
    if ( M_msg.get() != NULL )
    {
        M_msg->setSide( side );
    }
}

void
MsgBuilder::setTimeRecv( const int & time )
{
    if ( M_msg.get() != NULL )
    {
        M_msg->setTimeRecv( time );
    }
}



void
MsgBuilder::setVer( const unsigned int & min,
                    const unsigned int & max )
{
    if ( min == max )
    {
        // Then the version is restriced to min/max
        if ( min < M_max_ver )
        {
            M_min_ver = min;
        }
        else if ( min > M_min_ver )
        {
            M_max_ver = min;
        }
        else if ( M_max_ver < M_min_ver )
        {
            M_min_ver = M_max_ver = min;
        }
    }
    else
    {
        if ( min < M_min_ver )
        {
            M_min_ver = min;
        }

        if ( max > M_max_ver )
        {
            M_max_ver = max;
        }

        if ( min > max
             && M_max_ver < M_min_ver
             && min > M_max_ver
             && max < M_min_ver )
        {
            M_min_ver = min;
            M_max_ver = max;
        }
    }

    if ( M_msg.get() != NULL )
    {
        M_msg->setVer( min, max );
    }
}


void
MsgBuilder::buildMetaMsg()
{
    traceBuild< MetaMsg * >();
    MetaMsg * msg( new MetaMsg() );
    if ( checkIsItem( META ) )
    {
        while ( isItem( META ) )
        {
            msg->getTokens().push_front( getMetaToken().release() );
        }
    }
    setMsg( msg );
}


void
MsgBuilder::buildMetaTokenVer( const double& ver )
{
    traceBuild< MetaTokenVer * >();
    add( new MetaTokenVer( ver ) );
}



void
MsgBuilder::buildDefineMsg()
{
    traceBuild< DefineMsg * >();
    DefineMsg* msg = new DefineMsg();
    if ( checkIsItem( DEF ) )
    {
        while ( isItem( DEF ) )
        {
            msg->getDefs().push_front( getDef().release() );
        }
    }
    setMsg( msg );
}



void
MsgBuilder::buildDefineCond( const std::string & name )
{
    traceBuild< DefCond * >();
    if ( name.length() > STR_SIZE )
    {
        throw BuilderErr( __FILE__, __LINE__,
                          "Over the max string size." );
    }
    add( new DefCond( name, checkAndGetCond() ) );
}


void
MsgBuilder::buildDefineDir( const std::string & name )
{
    traceBuild< DefDir * >();
    if ( name.length() > STR_SIZE )
    {
        throw BuilderErr( __FILE__, __LINE__,
                          "Over the max string size." );
    }
    add( new DefDir( name, checkAndGetDir() ) );
}


void
MsgBuilder::buildDefineReg( const std::string & name )
{
    traceBuild< DefReg * >();
    if ( name.length() > STR_SIZE )
    {
        throw BuilderErr( __FILE__, __LINE__,
                          "Over the max string size." );
    }
    add( new DefReg( name, checkAndGetRegion() ) );
}


void
MsgBuilder::buildDefineAct( const std::string & name )
{
    traceBuild< DefAct * >();
    if ( name.length() > STR_SIZE )
    {
        throw BuilderErr( __FILE__, __LINE__,
                          "Over the max string size." );
    }
    add( new DefAct( name, checkAndGetAction() ) );
}


void
MsgBuilder::buildFreeformMsg( const std::string & str )
{
    traceBuild< FreeformMsg * >();
    setMsg( new FreeformMsg( str ) );
}


void
MsgBuilder::buildUnsuppMsg()
{
    traceBuild< UnsuppMsg * >();
    setMsg( new UnsuppMsg() );
}


void
MsgBuilder::buildInfoMsg()
{
    traceBuild< InfoMsg * >();
    InfoMsg * msg = new InfoMsg();
    if ( checkIsItem( TOKEN ) )
    {
        while ( isItem( TOKEN ) )
        {
            msg->getTokens().push_front( getToken().release() );
        }
    }
    setMsg( msg );
}


void
MsgBuilder::buildAdviceMsg()
{
    traceBuild< AdviceMsg * >();
    AdviceMsg* msg = new AdviceMsg();
    if ( checkIsItem( TOKEN ) )
    {
        while ( isItem( TOKEN ) )
        {
            msg->getTokens().push_front( getToken().release() );
        }
    }
    setMsg( msg );
}


void
MsgBuilder::buildTokenRule( const int & ttl )
{
    traceBuild< TokRule * >();
    TokRule * token = new TokRule( ttl );
    if ( checkIsItem( DIR ) )
    {
        while ( isItem( DIR ) )
        {
            token->getDirs().push_front( getDir().release() );
        }
    }
    token->set( checkAndGetCond() );
    add( token );
}


void
MsgBuilder::buildTokenClear()
{
    traceBuild< TokClear * >();
    add( new TokClear() );
}


void
MsgBuilder::buildActPos()
{
    traceBuild< ActPos * >();
    add( new ActPos( checkAndGetRegion() ) );
}


void
MsgBuilder::buildActHome()
{
    traceBuild< ActHome * >();
    add( new ActHome( checkAndGetRegion() ) );
}


void
MsgBuilder::buildActBallToReg()
{
    traceBuild< ActBallToReg * >();
    BallMove bm;
    if ( checkIsItem( BMT ) )
    {
        while( isItem( BMT ) )
            bm.addToken( getBMT() );
    }
    std::auto_ptr< Region > reg = checkAndGetRegion();
    add( new ActBallToReg( reg, bm ) );
}


void
MsgBuilder::buildActBallToPlayer()
{
    traceBuild< ActBallToPlayer * >();
    add( new ActBallToPlayer( *checkAndGetUNumSet() ) );
}


void
MsgBuilder::buildActMark()
{
    traceBuild< ActMark * >();
    add( new ActMark( *checkAndGetUNumSet() ) );
}


void
MsgBuilder::buildActMarkLinePlayer()
{
    traceBuild< ActMarkLinePlayer * >();
    add( new ActMarkLinePlayer( *checkAndGetUNumSet() ) );
}


void
MsgBuilder::buildActMarkLineReg()
{
    traceBuild< ActMarkLineReg * >();
    add( new ActMarkLineReg( checkAndGetRegion() ) );
}


void
MsgBuilder::buildActOffsideLine()
{
    traceBuild< ActOffsidesLine * >();
    add( new ActOffsidesLine( checkAndGetRegion() ) );
}


void
MsgBuilder::buildActHetType( const int & type )
{
    traceBuild< ActHetType * >();
    add( new ActHetType( type ) );
}


void
MsgBuilder::buildActNamed( const std::string & name )
{
    traceBuild< ActNamed * >();
    if ( name.length() > STR_SIZE )
    {
        throw BuilderErr( __FILE__, __LINE__,
                          "Over the max string size." );
    }
    add( new ActNamed( name ) );
}


void
MsgBuilder::buildActPassReg()
{
    traceBuild< ActPassReg * >();
    add( new ActPassReg( checkAndGetRegion() ) );
}


void
MsgBuilder::buildActPassUNum()
{
    traceBuild< ActPassUNum * >();
    add( new ActPassUNum( *checkAndGetUNumSet() ) );
}


void
MsgBuilder::buildActDribble()
{
    traceBuild< ActDribble * >();
    add( new ActDribble( checkAndGetRegion() ) );
}


void
MsgBuilder::buildActClear()
{
    traceBuild< ActClear * >();
    add( new ActClear( checkAndGetRegion() ) );
}


void
MsgBuilder::buildActShoot()
{
    traceBuild< ActShoot * >();
    add( new ActShoot() );
}


void
MsgBuilder::buildActHold()
{
    traceBuild< ActHold * >();
    add( new ActHold() );
}


void
MsgBuilder::buildActIntercept()
{
    traceBuild< ActIntercept * >();
    add( new ActIntercept() );
}


void
MsgBuilder::buildActTackle()
{
    traceBuild< ActTackle * >();
    add( new ActTackle( *checkAndGetUNumSet() ) );
}


void
MsgBuilder::buildDirComm( const bool& do_dont, const bool& our_side )
{
    traceBuild< DirComm * >();
    DirComm* dir = new DirComm( do_dont, our_side, UNumSet(),
                                std::list< Action* >() );
    if ( checkIsItem( ACTION ) )
    {
        while ( isItem( ACTION ) )
        {
            dir->add( getAction() );
        }
    }
    dir->set( *checkAndGetUNumSet() );
    add( dir );
}


void
MsgBuilder::buildDirNamed( const std::string& name )
{
    traceBuild< DirNamed * >();
    if ( name.length() > STR_SIZE )
    {
        throw BuilderErr( __FILE__, __LINE__,
                          "Over the max string size." );
    }
    add( new DirNamed( name ) );
}


void
MsgBuilder::buildCondTrue()
{
    traceBuild< CondBool * >();
    add( new CondBool( true ) );
}


void
MsgBuilder::buildCondFalse()
{
    traceBuild< CondBool * >();
    add( new CondBool( false ) );
}


void
MsgBuilder::buildCondPlayerPos( const bool & our_side,
                                const int & min,
                                const int & max )
{
    traceBuild< CondPlayerPos * >();
    std::auto_ptr< Region > reg = checkAndGetRegion();
    UNumSet unums = *checkAndGetUNumSet();
    add( new CondPlayerPos( our_side, unums, min, max, reg ) );
}


void
MsgBuilder::buildCondBallPos()
{
    traceBuild< CondBallPos * >();
    add( new CondBallPos( checkAndGetRegion() ) );
}


void
MsgBuilder::buildCondBallOwner( const bool & our_side )
{
    traceBuild< CondBallOwner * >();
    add( new CondBallOwner( our_side, *checkAndGetUNumSet() ) );
}


void
MsgBuilder::buildCondPlayMode( const PlayMode & play_mode )
{
    traceBuild< CondPlayMode * >();
    add( new CondPlayMode( play_mode ) );
}


void
MsgBuilder::buildCondAnd()
{
    traceBuild< CondAnd * >();
    CondList l = *checkAndGetCondList();
    std::auto_ptr< CondAnd > cond( new CondAnd( l ) );
    add( cond.release() );
}


void
MsgBuilder::buildCondOr()
{
    traceBuild< CondOr * >();
    CondList l = *checkAndGetCondList();
    std::auto_ptr< CondOr > cond( new CondOr( l ) );
    add( cond.release() );
}


void
MsgBuilder::buildCondNot()
{
    traceBuild< CondNot * >();
    add( new CondNot( checkAndGetCond() ) );
}


void
MsgBuilder::buildCondNamed( const std::string & name )
{
    traceBuild< CondNamed * >();
    if ( name.length() > STR_SIZE )
    {
        throw BuilderErr( __FILE__, __LINE__,
                          "Over the max string size." );
    }
    add( new CondNamed( name ) );
}


void
MsgBuilder::buildCondTime( const int & time,
                           const util::CompOp & comp )
{
    traceBuild< CondTime * >();
    add( new CondTime( time, comp ) );
}


void
MsgBuilder::buildCondOppGoal( const int & goals,
                              const util::CompOp & comp )
{
    traceBuild< CondOppGoal * >();
    add( new CondOppGoal( goals, comp ) );
}


void
MsgBuilder::buildCondOurGoal( const int & goals,
                              const util::CompOp & comp )
{
    traceBuild< CondOurGoal * >();
    add( new CondOurGoal( goals, comp ) );
}


void
MsgBuilder::buildCondGoalDiff( const int & goals,
                               const util::CompOp & comp )
{
    traceBuild< CondGoalDiff * >();
    add( new CondGoalDiff( goals, comp ) );
}


void
MsgBuilder::buildCondUNum( const rcss::clang::UNum & unum )
{
    traceBuild< CondUNum * >();
    add( new CondUNum( unum, *checkAndGetUNumSet() ) );
}


void
MsgBuilder::buildAddToCondList()
{
    traceBuild< std::list< Cond * > >();
    std::auto_ptr< Cond > c = checkAndGetCond();
    CondList * l = checkAndGetCondList().release();
    l->push_back( c.release() );
    add( l );
}

void
MsgBuilder::buildCreateCondList()
{
    traceBuild< std::list< Cond * > >();
    std::auto_ptr< Cond > c = checkAndGetCond();
    CondList * l = new CondList();
    l->push_back( c.release() );
    add( l );
}
void
MsgBuilder::buildRegNull()
{
    traceBuild< RegNull * >();
    add( new RegNull() );
}


void
MsgBuilder::buildRegQuad()
{
    traceBuild< RegQuad * >();
    std::auto_ptr< Point > p4 = checkAndGetPoint();
    std::auto_ptr< Point > p3 = checkAndGetPoint();
    std::auto_ptr< Point > p2 = checkAndGetPoint();
    std::auto_ptr< Point > p1 = checkAndGetPoint();
    add( new RegQuad( p1, p2, p3, p4 ) );
}


void
MsgBuilder::buildRegArc( const double & start_rad,
                         const double & end_rad,
                         const double & start_ang,
                         const double & span_ang )
{
    traceBuild< RegArc * >();
    add( new RegArc( checkAndGetPoint(),
                     start_rad, end_rad,
                     start_ang, span_ang ) );
}


void
MsgBuilder::buildRegUnion()
{
    traceBuild< RegUnion * >();
    std::auto_ptr< RegUnion > reg( new RegUnion() );
    if ( checkIsItem( REG ) )
    {
        while ( isItem( REG ) )
        {
            reg->getRegions().push_front( getRegion().release() );
        }
    }
    add( reg.release() );
}


void
MsgBuilder::buildRegNamed( const std::string & name )
{
    traceBuild< RegNamed * >();
    if ( name.length() > STR_SIZE )
    {
        throw BuilderErr( __FILE__, __LINE__,
                          "Over the max string size." );
    }
    add( new RegNamed( name ) );
}


void
MsgBuilder::buildRegPoint()
{
    traceBuild< RegPoint * >();
    add( new RegPoint( checkAndGetPoint() ) );
}


void
MsgBuilder::buildRegTri()
{
    traceBuild< RegTri * >();
    std::auto_ptr< Point > p3 = checkAndGetPoint();
    std::auto_ptr< Point > p2 = checkAndGetPoint();
    std::auto_ptr< Point > p1 = checkAndGetPoint();
    add( new RegTri( p1, p2, p3 ) );
}


void
MsgBuilder::buildRegRec()
{
    traceBuild< RegRec * >();
    std::auto_ptr< Point > p2 = checkAndGetPoint();
    std::auto_ptr< Point > p1 = checkAndGetPoint();
    add( new RegRec( p1, p2 ) );
}


void
MsgBuilder::buildPointSimple( const double & x,
                              const double & y )
{
    traceBuild< PointSimple * >();
    add( new PointSimple( x, y ) );
}


void
MsgBuilder::buildPointRel( const double & x,
                           const double & y )
{
    traceBuild< PointRel * >();
    add( new PointRel( x, y, checkAndGetPoint() ) );
}


void
MsgBuilder::buildPointBall()
{
    traceBuild< PointBall * >();
    add( new PointBall() );
}


void
MsgBuilder::buildPointPlayer( const bool & our_side,
                              const UNum & unum )
{
    traceBuild< PointPlayer * >();
    add( new PointPlayer( our_side, unum ) );
}


void
MsgBuilder::buildPointArith( const rcss::util::ArithOp & arith_op )
{
    traceBuild< PointArith * >();
    // we need to make sure ordering is maintained, otherwise you could end
    // up with 'b / a', when 'a / b' was what was actually sent.
    std::auto_ptr< Point > second = checkAndGetPoint();
    std::auto_ptr< Point > first = checkAndGetPoint();
    add( new PointArith( first, second, arith_op ) );
}


void
MsgBuilder::buildUNum( const UNum & unum )
{
    traceBuild< UNum * >();
    add( new UNum( unum ) );
}


void
MsgBuilder::buildUNumSet()
{
    traceBuild< UNumSet >();
    UNumSet unums;
    if( checkIsItem( UNUM ) )
    {
        while ( isItem( UNUM ) )
        {
            unums.add( *getUNum() );
        }
    }
    add( new UNumSet( unums ) );
}


void
MsgBuilder::buildBallMoveToken( const BallMoveToken& bmt )
{
    traceBuild< BallMoveToken >();
    add( bmt );
}


void
MsgBuilder::buildRuleMsg()
{
    traceBuild< RuleMsg * >();
    RuleMsg * msg = new RuleMsg();
    if ( checkIsItem( ACTIVATE ) )
    {
        while ( isItem( ACTIVATE ) )
        {
            msg->getList().push_front( *getActivateRules() );
        }
    }
    setMsg( msg );
}


void
MsgBuilder::buildActivateAllRules( const bool & on )
{
    traceBuild< ActivateRules >();
    add( new ActivateRules( on, RuleIDList() ) );
}


void
MsgBuilder::buildActivateRules( const bool & on )
{
    traceBuild< ActivateRules >();
    ActivateRules * act = new ActivateRules( on, RuleIDList() );
    if ( isItem( RID ) )
    {
        RuleIDList rids;
        rids.push_front( *getRID() );
        act->set( rids );
    }
    else
    {
        act->set( *checkAndGetRIDList() );
    }
    add( act );
}


void
MsgBuilder::buildRuleID( const std::string & id )
{
    traceBuild< RuleMsg * >();
    if ( id.length() > STR_SIZE )
    {
        throw BuilderErr( __FILE__, __LINE__,
                          "Over the max string size." );
    }
    add( id );
}


void
MsgBuilder::buildRuleIDList()
{
    traceBuild< RuleIDList >();
    RuleIDList rids;
    if ( checkIsItem( RID ) )
    {
        while ( isItem( RID ) )
        {
            rids.push_front( *getRID() );
        }
        // by adding the items to the front, the original order is
        // maintained.
    }
    add( new RuleIDList( rids ) );
}


void
MsgBuilder::buildRuleIDListALL()
{
    traceBuild< RuleIDList >();
    add( new RuleIDList() ); // an empty list == all rules.
}


void
MsgBuilder::buildDelMsg()
{
    traceBuild< DelMsg * >();
    DelMsg * msg = new DelMsg;
    if ( isItem( RID ) )
    {
        RuleIDList rids;
        rids.push_front( *getRID() );
        msg->set( rids );
    }
    else
    {
        msg->set( *checkAndGetRIDList() );
    }
    setMsg( msg );
}


void
MsgBuilder::buildDefineModelRule( const std::string & id )
{
    traceBuild< DefRule * >();
    if ( id.length() > STR_SIZE )
    {
        throw BuilderErr( __FILE__, __LINE__,
                          "Over the max string size." );
    }
    add( new DefRule( id, checkAndGetRule(), true ) );
}


void
MsgBuilder::buildDefineDirectiveRule( const std::string & id )
{
    traceBuild< DefRule * >();
    if ( id.length() > STR_SIZE )
    {
        throw BuilderErr( __FILE__, __LINE__,
                          "Over the max string size." );
    }
    add( new DefRule( id, checkAndGetRule(), false ) );
}


void
MsgBuilder::buildSimpleRule()
{
    traceBuild< SimpleRule * >();
    SimpleRule::Storage dirs;
    if ( checkIsItem( DIR ) )
    {
        do
        {
            dirs.push_front( getDir().release() );
        }
        while( isItem( DIR ) );
    }
    add( new SimpleRule( checkAndGetCond(), dirs ) );
}


void
MsgBuilder::buildNestedRule()
{
    traceBuild< NestedRule * >();
    NestedRule::Storage rules;
    if ( checkIsItem( RULE ) )
    {
        do
        {
            rules.push_front( getRule().release() );
        }
        while ( isItem( RULE ) );
    }
    add( new NestedRule( checkAndGetCond(), rules ) );
}


void
MsgBuilder::buildIDRule()
{
    traceBuild< IDListRule * >();
    add( new IDListRule( *checkAndGetRIDList() ) );
}

}
}
