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
    m_item = boost::any();
}

std::shared_ptr< MetaToken >
MsgBuilder::getMetaToken()
{
    traceRemoving< MetaToken >();
    std::shared_ptr< MetaToken > rval = boost::any_cast< std::shared_ptr< MetaToken > >( M_items.top().m_item );
    M_items.pop();
    return rval;
}


std::shared_ptr< Action >
MsgBuilder::getAction()
{
    traceRemoving< Action >();
    std::shared_ptr< Action > rval = boost::any_cast< std::shared_ptr< Action > >( M_items.top().m_item );
    M_items.pop();
    return rval;
}

std::shared_ptr< Dir >
MsgBuilder::getDir()
{
    traceRemoving< Dir >();
    std::shared_ptr< Dir > rval = boost::any_cast< std::shared_ptr< Dir > >( M_items.top().m_item );
    M_items.pop();
    return rval;
}


std::shared_ptr< Cond >
MsgBuilder::getCond()
{
    traceRemoving< Cond >();
    std::shared_ptr< Cond > rval = boost::any_cast< std::shared_ptr< Cond > >( M_items.top().m_item );
    M_items.pop();
    return rval;
}


std::shared_ptr< MsgBuilder::CondList >
MsgBuilder::getCondList()
{
    traceRemoving< CondList >();
    std::shared_ptr< CondList > rval = boost::any_cast< std::shared_ptr< CondList > >( M_items.top().m_item );
    M_items.pop();
    return rval;
}


std::shared_ptr< Token >
MsgBuilder::getToken()
{
    traceRemoving< Token >();
    std::shared_ptr< Token > rval = boost::any_cast< std::shared_ptr< Token > >( M_items.top().m_item );
    M_items.pop();
    return rval;
}


std::shared_ptr< Def >
MsgBuilder::getDef()
{
    traceRemoving< Def >();
    std::shared_ptr< Def > rval = boost::any_cast< std::shared_ptr< Def > >( M_items.top().m_item );
    M_items.pop();
    return rval;
}


std::shared_ptr< Region >
MsgBuilder::getRegion()
{
    traceRemoving< Region >();
    std::shared_ptr< Region > rval = boost::any_cast< std::shared_ptr< Region > >( M_items.top().m_item );
    M_items.pop();
    return rval;
}


std::shared_ptr< Point >
MsgBuilder::getPoint()
{
    traceRemoving< Point >();
    std::shared_ptr< Point > rval = boost::any_cast< std::shared_ptr< Point > >( M_items.top().m_item );
    M_items.pop();
    return rval;
}


std::shared_ptr< UNum >
MsgBuilder::getUNum()
{
    traceRemoving< UNum >();
    std::shared_ptr< UNum > rval = boost::any_cast< std::shared_ptr< UNum > >( M_items.top().m_item );
    M_items.pop();
    return rval;
}


BallMoveToken
MsgBuilder::getBMT()
{
    traceRemoving< BallMoveToken >();
    BallMoveToken rval = boost::any_cast< BallMoveToken >( M_items.top().m_item );
    M_items.pop();
    return rval;
}


std::shared_ptr< UNumSet >
MsgBuilder::getUNumSet()
{
    traceRemoving< UNumSet >();
    std::shared_ptr< UNumSet > rval = boost::any_cast< std::shared_ptr< UNumSet > >( M_items.top().m_item );
    M_items.pop();
    return rval;
}


std::shared_ptr< std::string >
MsgBuilder::getStr()
{
    traceRemoving< std::string >();
    std::shared_ptr< std::string > rval = boost::any_cast< std::shared_ptr< std::string > >( M_items.top().m_item );
    M_items.pop();
    return rval;
}


std::shared_ptr< RuleIDList >
MsgBuilder::getRIDList()
{
    traceRemoving< RuleIDList >();
    std::shared_ptr< RuleIDList > rval = boost::any_cast< std::shared_ptr< RuleIDList > >( M_items.top().m_item );
    M_items.pop();
    return rval;
}


std::shared_ptr< ActivateRules >
MsgBuilder::getActivateRules()
{
    traceRemoving< ActivateRules >();
    std::shared_ptr< ActivateRules > rval = boost::any_cast< std::shared_ptr< ActivateRules > >( M_items.top().m_item );
    M_items.pop();
    return rval;
}


std::shared_ptr< Rule >
MsgBuilder::getRule()
{
    traceRemoving< Rule >();
    std::shared_ptr< Rule > rval = boost::any_cast< std::shared_ptr< Rule > >( M_items.top().m_item );
    M_items.pop();
    return rval;
}

std::shared_ptr< RuleID >
MsgBuilder::getRID()
{
    traceRemoving< RuleID >();
    std::shared_ptr< RuleID > rval = boost::any_cast< std::shared_ptr< RuleID > >( M_items.top().m_item );
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


std::shared_ptr< MetaToken >
MsgBuilder::checkAndGetMetaToken()
{
    if ( checkIsItem( META ) )
    {
        return getMetaToken();
    }
    // no need for error handling as this is done by checkIsItem()
    return std::shared_ptr< MetaToken >();
}


std::shared_ptr< Action >
MsgBuilder::checkAndGetAction()
{
    if ( checkIsItem( ACTION ) )
    {
        return getAction();
    }
    // no need for error handling as this is done by checkIsItem()
    return std::shared_ptr< Action >();
}

std::shared_ptr< Dir >
MsgBuilder::checkAndGetDir()
{
    if ( checkIsItem( DIR ) )
    {
        return getDir();
    }
    // no need for error handling as this is done by checkIsItem()
    return std::shared_ptr< Dir >();
}


std::shared_ptr< Cond >
MsgBuilder::checkAndGetCond()
{
    if ( checkIsItem( COND ) )
    {
        return getCond();
    }
    // no need for error handling as this is done by checkIsItem()
    return std::shared_ptr< Cond >();
}


std::shared_ptr< MsgBuilder::CondList >
MsgBuilder::checkAndGetCondList()
{
    if ( checkIsItem( CONDLIST ) )
    {
        return getCondList();
    }
    // no need for error handling as this is done by checkIsItem()
    return std::shared_ptr< CondList >( new CondList() );
}


std::shared_ptr< Token >
MsgBuilder::checkAndGetToken()
{
    if ( checkIsItem( TOKEN ) )
    {
        return getToken();
    }
    // no need for error handling as this is done by checkIsItem()
    return std::shared_ptr< Token >();
}


std::shared_ptr< Def >
MsgBuilder::checkAndGetDef()
{
    if ( checkIsItem( DEF ) )
    {
        return getDef();
    }
    // no need for error handling as this is done by checkIsItem()
    return std::shared_ptr< Def >();
}


std::shared_ptr< Region >
MsgBuilder::checkAndGetRegion()
{
    if ( checkIsItem( REG ) )
    {
        return getRegion();
    }
    // no need for error handling as this is done by checkIsItem()
    return std::shared_ptr< Region >();
}


std::shared_ptr< Point >
MsgBuilder::checkAndGetPoint()
{
    if ( checkIsItem( POINT ) )
    {
        return getPoint();
    }
    // no need for error handling as this is done by checkIsItem()
    return std::shared_ptr< Point >();
}


std::shared_ptr< UNum >
MsgBuilder::checkAndGetUNum()
{
    if ( checkIsItem( UNUM ) )
    {
        return getUNum();
    }
    // no need for error handling as this is done by checkIsItem()
    return std::shared_ptr< UNum >( new UNum() );
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


std::shared_ptr< UNumSet >
MsgBuilder::checkAndGetUNumSet()
{
    if ( checkIsItem( UNUMSET ) )
    {
        return getUNumSet();
    }
    // no need for error handling as this is done by checkIsItem()
    return std::shared_ptr< UNumSet >( new UNumSet() );
}


std::shared_ptr< std::string >
MsgBuilder::checkAndGetStr()
{
    if ( checkIsItem( STR ) )
    {
        return getStr();
    }
    // no need for error handling as this is done by checkIsItem()
    return std::shared_ptr< std::string >( new std::string() );
}


std::shared_ptr< RuleIDList >
MsgBuilder::checkAndGetRIDList()
{
    if ( checkIsItem( RIDLIST ) )
    {
        return getRIDList();
    }
    // no need for error handling as this is done by checkIsItem()
    return std::shared_ptr< RuleIDList >();
}


std::shared_ptr< ActivateRules >
MsgBuilder::checkAndGetActivateRules()
{
    if ( checkIsItem( ACTIVATE ) )
    {
        return getActivateRules();
    }
    // no need for error handling as this is done by checkIsItem()
    return std::shared_ptr< ActivateRules >();
}


std::shared_ptr< Rule >
MsgBuilder::checkAndGetRule()
{
    if ( checkIsItem( RULE ) )
    {
        return getRule();
    }
    // no need for error handling as this is done by checkIsItem()
    return std::shared_ptr< Rule >();
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

void
MsgBuilder::add( std::shared_ptr< MetaToken > meta )
{
    traceAdding< MetaToken >();
    ItemType tmp;
    tmp.m_item = meta;
    tmp.m_type = META;
    M_items.push( tmp );
}

void
MsgBuilder::add( std::shared_ptr< Action > action )
{
    traceAdding< Action >();
    ItemType tmp;
    tmp.m_item = action;
    tmp.m_type = ACTION;
    M_items.push( tmp );
}

void
MsgBuilder::add( std::shared_ptr< Dir > dir )
{
    traceAdding< Dir >();
    ItemType tmp;
    tmp.m_item = dir;
    tmp.m_type = DIR;
    M_items.push( tmp );
}

void
MsgBuilder::add( std::shared_ptr< Cond > cond )
{
    traceAdding< Cond >();
    ItemType tmp;
    tmp.m_item = cond;
    tmp.m_type = COND;
    M_items.push( tmp );
}

void
MsgBuilder::add( std::shared_ptr< MsgBuilder::CondList > cond_list )
{
    traceAdding< CondList >();
    ItemType tmp;
    tmp.m_item = cond_list;
    tmp.m_type = CONDLIST;
    M_items.push( tmp );
}

void
MsgBuilder::add( std::shared_ptr< Token > token )
{
    traceAdding< Token >();
    ItemType tmp;
    tmp.m_item = token;
    tmp.m_type = TOKEN;
    M_items.push( tmp );
}

void
MsgBuilder::add( std::shared_ptr< Def > def )
{
    traceAdding< Def >();
    ItemType tmp;
    tmp.m_item = def;
    tmp.m_type = DEF;
    M_items.push( tmp );
}

void
MsgBuilder::add( std::shared_ptr< Region > region )
{
    traceAdding< Region >();
    ItemType tmp;
    tmp.m_item = region;
    tmp.m_type = REG;
    M_items.push( tmp );
}

void
MsgBuilder::add( std::shared_ptr< Point > point )
{
    traceAdding< Point >();
    ItemType tmp;
    tmp.m_item = point;
    tmp.m_type = POINT;
    M_items.push( tmp );
}

void
MsgBuilder::add( std::shared_ptr< UNum > unum )
{
    traceAdding< UNum >();
    ItemType tmp;
    tmp.m_item = unum;
    tmp.m_type = UNUM;
    M_items.push( tmp );
}

void
MsgBuilder::add( BallMoveToken bmt )
{
    traceAdding< BallMoveToken >();
    ItemType tmp;
    tmp.m_item = bmt;
    tmp.m_type = BMT;
    M_items.push( tmp );
}

void
MsgBuilder::add( std::shared_ptr< UNumSet > unum_set )
{
    traceAdding< UNumSet >();
    ItemType tmp;
    tmp.m_item = unum_set;
    tmp.m_type = UNUMSET;
    M_items.push( tmp );
}

void
MsgBuilder::add( std::shared_ptr< std::string > str )
{
    traceAdding< std::string >();
    ItemType tmp;
    tmp.m_item = str;
    tmp.m_type = STR;
    M_items.push( tmp );
}

void
MsgBuilder::add( std::shared_ptr< RuleIDList > rid_list )
{
    traceAdding< RuleIDList >();
    ItemType tmp;
    tmp.m_item = rid_list;
    tmp.m_type = RIDLIST;
    M_items.push( tmp );
}

void
MsgBuilder::add( std::shared_ptr< ActivateRules > act_rules )
{
    traceAdding< ActivateRules >();
    ItemType tmp;
    tmp.m_item = act_rules;
    tmp.m_type = ACTIVATE;
    M_items.push( tmp );
}

void
MsgBuilder::add( std::shared_ptr< Rule > rule )
{
    traceAdding< Rule >();
    ItemType tmp;
    tmp.m_item = rule;
    tmp.m_type = RULE;
    M_items.push( tmp );
}

void
MsgBuilder::add( RuleID rid )
{
    traceAdding< BallMoveToken >();
    ItemType tmp;
    tmp.m_item = std::shared_ptr< RuleID >( new RuleID( rid ) );
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
    M_msg.reset();
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

std::shared_ptr< Msg >
MsgBuilder::getMsg()
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
MsgBuilder::setMsg( std::shared_ptr< Msg > msg )
{
    M_msg = msg;
    setVer( M_min_ver, M_max_ver );
    checkItemsEmpty();
}


void
MsgBuilder::setTime( const int & time )
{
    if ( M_msg )
    {
        M_msg->setTimeSend( time );
    }
}


void
MsgBuilder::setSide( const int & side )
{
    if ( M_msg )
    {
        M_msg->setSide( side );
    }
}

void
MsgBuilder::setTimeRecv( const int & time )
{
    if ( M_msg )
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

    if ( M_msg )
    {
        M_msg->setVer( min, max );
    }
}


void
MsgBuilder::buildMetaMsg()
{
    traceBuild< MetaMsg * >();
    std::shared_ptr< MetaMsg > msg( new MetaMsg() );
    if ( checkIsItem( META ) )
    {
        while ( isItem( META ) )
        {
            msg->getTokens().push_front( getMetaToken() );
        }
    }

    setMsg( msg );
}


void
MsgBuilder::buildMetaTokenVer( const double& ver )
{
    traceBuild< MetaTokenVer * >();
    std::shared_ptr< MetaTokenVer > ptr( new MetaTokenVer( ver ) );
    add( ptr );
}



void
MsgBuilder::buildDefineMsg()
{
    traceBuild< DefineMsg * >();
    std::shared_ptr< DefineMsg > msg( new DefineMsg() );
    if ( checkIsItem( DEF ) )
    {
        while ( isItem( DEF ) )
        {
            msg->getDefs().push_front( getDef() );
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

    std::shared_ptr< DefCond > ptr( new DefCond( name, checkAndGetCond() ) );;
    add( ptr );
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

    std::shared_ptr< DefDir > ptr( new DefDir( name, checkAndGetDir() ) );
    add( ptr );
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

    std::shared_ptr< DefReg > ptr( new DefReg( name, checkAndGetRegion() ) );
    add( ptr );
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

    std::shared_ptr< DefAct > ptr( new DefAct( name, checkAndGetAction() ) );
    add( ptr );
}


void
MsgBuilder::buildFreeformMsg( const std::string & str )
{
    traceBuild< FreeformMsg * >();
    std::shared_ptr< Msg > msg( new FreeformMsg( str ) );
    setMsg( msg );
}


void
MsgBuilder::buildUnsuppMsg()
{
    traceBuild< UnsuppMsg * >();
    std::shared_ptr< Msg > msg( new UnsuppMsg() );
    setMsg( msg );
}


void
MsgBuilder::buildInfoMsg()
{
    traceBuild< InfoMsg * >();
    std::shared_ptr< InfoMsg > msg( new InfoMsg() );
    if ( checkIsItem( TOKEN ) )
    {
        while ( isItem( TOKEN ) )
        {
            msg->getTokens().push_front( getToken() );
        }
    }
    setMsg( msg );
}


void
MsgBuilder::buildAdviceMsg()
{
    traceBuild< AdviceMsg * >();
    std::shared_ptr< AdviceMsg > msg( new AdviceMsg() );
    if ( checkIsItem( TOKEN ) )
    {
        while ( isItem( TOKEN ) )
        {
            msg->getTokens().push_front( getToken() );
        }
    }
    setMsg( msg );
}


void
MsgBuilder::buildTokenRule( const int & ttl )
{
    traceBuild< TokRule * >();
    std::shared_ptr< TokRule > token( new TokRule( ttl ) );
    if ( checkIsItem( DIR ) )
    {
        while ( isItem( DIR ) )
        {
            token->getDirs().push_front( getDir() );
        }
    }

    token->setCond( checkAndGetCond() );
    add( token );
}


void
MsgBuilder::buildTokenClear()
{
    traceBuild< TokClear * >();

    std::shared_ptr< Token > ptr( new TokClear() );
    add( ptr );
}


void
MsgBuilder::buildActPos()
{
    traceBuild< ActPos * >();

    std::shared_ptr< Action > ptr( new ActPos( checkAndGetRegion() ) );
    add( ptr );
}


void
MsgBuilder::buildActHome()
{
    traceBuild< ActHome * >();
    std::shared_ptr< Action > ptr( new ActHome( checkAndGetRegion() ) );
    add( ptr );
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

    std::shared_ptr< Region > reg = checkAndGetRegion();
    std::shared_ptr< Action > ptr( new ActBallToReg( reg, bm ) );
    add( ptr );
}


void
MsgBuilder::buildActBallToPlayer()
{
    traceBuild< ActBallToPlayer * >();

    std::shared_ptr< Action > ptr( new ActBallToPlayer( *checkAndGetUNumSet() ) );
    add( ptr );
}


void
MsgBuilder::buildActMark()
{
    traceBuild< ActMark * >();
    std::shared_ptr< Action > ptr( new ActMark( *checkAndGetUNumSet() ) );
    add( ptr );
}


void
MsgBuilder::buildActMarkLinePlayer()
{
    traceBuild< ActMarkLinePlayer * >();
    std::shared_ptr< Action > ptr( new ActMarkLinePlayer( *checkAndGetUNumSet() ) );
    add( ptr );
}


void
MsgBuilder::buildActMarkLineReg()
{
    traceBuild< ActMarkLineReg * >();
    std::shared_ptr< Action > ptr( new ActMarkLineReg( checkAndGetRegion() ) );
    add( ptr );
}


void
MsgBuilder::buildActOffsideLine()
{
    traceBuild< ActOffsidesLine * >();
    std::shared_ptr< Action > ptr( new ActOffsidesLine( checkAndGetRegion() ) );
    add( ptr );
}


void
MsgBuilder::buildActHetType( const int & type )
{
    traceBuild< ActHetType * >();
    std::shared_ptr< Action > ptr( new ActHetType( type ) );
    add( ptr );
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

    std::shared_ptr< Action > ptr( new ActNamed( name ) );
    add( ptr );
}


void
MsgBuilder::buildActPassReg()
{
    traceBuild< ActPassReg * >();

    std::shared_ptr< Action > ptr( new ActPassReg( checkAndGetRegion() ) );
    add( ptr );
}


void
MsgBuilder::buildActPassUNum()
{
    traceBuild< ActPassUNum * >();

    std::shared_ptr< Action > ptr( new ActPassUNum( *checkAndGetUNumSet() ) );
    add( ptr );
}


void
MsgBuilder::buildActDribble()
{
    traceBuild< ActDribble * >();

    std::shared_ptr< Action > ptr( new ActDribble( checkAndGetRegion() ) );
    add( ptr );
}


void
MsgBuilder::buildActClear()
{
    traceBuild< ActClear * >();
    std::shared_ptr< Action > ptr( new ActClear( checkAndGetRegion() ) );
    add( ptr );
}


void
MsgBuilder::buildActShoot()
{
    traceBuild< ActShoot * >();
    std::shared_ptr< Action > ptr( new ActShoot() );
    add( ptr );
}


void
MsgBuilder::buildActHold()
{
    traceBuild< ActHold * >();
    std::shared_ptr< Action > ptr( new ActHold() );
    add( ptr );
}


void
MsgBuilder::buildActIntercept()
{
    traceBuild< ActIntercept * >();
    std::shared_ptr< Action > ptr( new ActIntercept() );
    add( ptr );
}


void
MsgBuilder::buildActTackle()
{
    traceBuild< ActTackle * >();
    std::shared_ptr< Action > ptr( new ActTackle( *checkAndGetUNumSet() ) );
    add( ptr);
}


void
MsgBuilder::buildDirComm( const bool& do_dont, const bool& our_side )
{
    traceBuild< DirComm * >();
    std::shared_ptr< DirComm > dir( new DirComm( do_dont, our_side, UNumSet(),
                                                 DirComm::Storage() ) );
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

    std::shared_ptr< Dir > ptr( new DirNamed( name ) );
    add( ptr );
}


void
MsgBuilder::buildCondTrue()
{
    traceBuild< CondBool * >();
    std::shared_ptr< Cond > ptr( new CondBool( true ) );
    add( ptr );
}


void
MsgBuilder::buildCondFalse()
{
    traceBuild< CondBool * >();
    std::shared_ptr< Cond > ptr( new CondBool( false ) );
    add( ptr );
}


void
MsgBuilder::buildCondPlayerPos( const bool & our_side,
                                const int & min,
                                const int & max )
{
    traceBuild< CondPlayerPos * >();
    std::shared_ptr< Region > reg = checkAndGetRegion();
    UNumSet unums = *checkAndGetUNumSet();

    std::shared_ptr< Cond > ptr( new CondPlayerPos( our_side, unums, min, max, reg ) );
    add( ptr );
}


void
MsgBuilder::buildCondBallPos()
{
    traceBuild< CondBallPos * >();

    std::shared_ptr< Cond > ptr( new CondBallPos( checkAndGetRegion() ) );
    add( ptr );
}


void
MsgBuilder::buildCondBallOwner( const bool & our_side )
{
    traceBuild< CondBallOwner * >();

    std::shared_ptr< UNumSet > unumset = checkAndGetUNumSet();
    std::shared_ptr< Cond > ptr( new CondBallOwner( our_side, *unumset ) );
    add( ptr );
}


void
MsgBuilder::buildCondPlayMode( const PlayMode & play_mode )
{
    traceBuild< CondPlayMode * >();

    std::shared_ptr< Cond > ptr( new CondPlayMode( play_mode ) );
    add( ptr );
}


void
MsgBuilder::buildCondAnd()
{
    traceBuild< CondAnd * >();
    std::shared_ptr< CondList > l = checkAndGetCondList();
    std::shared_ptr< Cond > cond( new CondAnd( *l ) );
    add( cond );
}


void
MsgBuilder::buildCondOr()
{
    traceBuild< CondOr * >();
    std::shared_ptr< CondList > l = checkAndGetCondList();
    std::shared_ptr< Cond > cond( new CondOr( *l ) );
    add( cond );
}


void
MsgBuilder::buildCondNot()
{
    traceBuild< CondNot * >();
    std::shared_ptr< Cond > cond( new CondNot( checkAndGetCond() ) );
    add( cond );
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

    std::shared_ptr< Cond > cond( new CondNamed( name ) );
    add( cond );
}


void
MsgBuilder::buildCondTime( const int & time,
                           const util::CompOp & comp )
{
    traceBuild< CondTime * >();
    std::shared_ptr< Cond > cond( new CondTime( time, comp ) );
    add( cond );
}


void
MsgBuilder::buildCondOppGoal( const int & goals,
                              const util::CompOp & comp )
{
    traceBuild< CondOppGoal * >();
    std::shared_ptr< Cond > cond( new CondOppGoal( goals, comp ) );
    add( cond );
}


void
MsgBuilder::buildCondOurGoal( const int & goals,
                              const util::CompOp & comp )
{
    traceBuild< CondOurGoal * >();
    std::shared_ptr< Cond > cond( new CondOurGoal( goals, comp ) );
    add( cond );
}


void
MsgBuilder::buildCondGoalDiff( const int & goals,
                               const util::CompOp & comp )
{
    traceBuild< CondGoalDiff * >();
    std::shared_ptr< Cond > cond( new CondGoalDiff( goals, comp ) );
    add( cond );
}


void
MsgBuilder::buildCondUNum( const rcss::clang::UNum & unum )
{
    traceBuild< CondUNum * >();
    std::shared_ptr< UNumSet > unumset = checkAndGetUNumSet();
    std::shared_ptr< Cond > ptr( new CondUNum( unum, *unumset ) );
    add( ptr );
}


void
MsgBuilder::buildAddToCondList()
{
    traceBuild< std::list< Cond * > >();
    std::shared_ptr< Cond > c = checkAndGetCond();
    std::shared_ptr< CondList > l = checkAndGetCondList();
    l->push_back( c );
    add( l );
}

void
MsgBuilder::buildCreateCondList()
{
    traceBuild< std::list< Cond * > >();
    std::shared_ptr< Cond > c = checkAndGetCond();
    std::shared_ptr< CondList > l( new CondList() );
    l->push_back( c );
    add( l );
}
void
MsgBuilder::buildRegNull()
{
    traceBuild< RegNull * >();
    std::shared_ptr< Region > reg( new RegNull() );
    add( reg );
}


void
MsgBuilder::buildRegQuad()
{
    traceBuild< RegQuad * >();
    std::shared_ptr< Point > p4 = checkAndGetPoint();
    std::shared_ptr< Point > p3 = checkAndGetPoint();
    std::shared_ptr< Point > p2 = checkAndGetPoint();
    std::shared_ptr< Point > p1 = checkAndGetPoint();

    std::shared_ptr< Region > reg( new RegQuad( p1, p2, p3, p4 ) );
    add( reg );
}


void
MsgBuilder::buildRegArc( const double & start_rad,
                         const double & end_rad,
                         const double & start_ang,
                         const double & span_ang )
{
    traceBuild< RegArc * >();
    std::shared_ptr< Region > reg( new RegArc( checkAndGetPoint(),
                                               start_rad, end_rad,
                                               start_ang, span_ang ) );
    add( reg );
}


void
MsgBuilder::buildRegUnion()
{
    traceBuild< RegUnion * >();
    std::shared_ptr< RegUnion > reg( new RegUnion() );
    if ( checkIsItem( REG ) )
    {
        while ( isItem( REG ) )
        {
            std::shared_ptr< Region > r = getRegion();
            reg->getRegions().push_front( r );
        }
    }
    add( reg );
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

    std::shared_ptr< Region > reg( new RegNamed( name ) );
    add( reg );
}


void
MsgBuilder::buildRegPoint()
{
    traceBuild< RegPoint * >();
    std::shared_ptr< Region > reg( new RegPoint( checkAndGetPoint() ) );
    add( reg );
}


void
MsgBuilder::buildRegTri()
{
    traceBuild< RegTri * >();
    std::shared_ptr< Point > p3 = checkAndGetPoint();
    std::shared_ptr< Point > p2 = checkAndGetPoint();
    std::shared_ptr< Point > p1 = checkAndGetPoint();

    std::shared_ptr< Region > reg( new RegTri( p1, p2, p3 ) );
    add( reg );
}


void
MsgBuilder::buildRegRec()
{
    traceBuild< RegRec * >();
    std::shared_ptr< Point > p2 = checkAndGetPoint();
    std::shared_ptr< Point > p1 = checkAndGetPoint();

    std::shared_ptr< Region > reg( new RegRec( p1, p2 ) );
    add( reg );
}


void
MsgBuilder::buildPointSimple( const double & x,
                              const double & y )
{
    traceBuild< PointSimple * >();
    std::shared_ptr< Point > p( new PointSimple( x, y ) );
    add( p );
}


void
MsgBuilder::buildPointRel( const double & x,
                           const double & y )
{
    traceBuild< PointRel * >();
    std::shared_ptr< Point > p( new PointRel( x, y, checkAndGetPoint() ) );
    add( p );
}


void
MsgBuilder::buildPointBall()
{
    traceBuild< PointBall * >();
    std::shared_ptr< Point > p( new PointBall() );
    add( p );
}


void
MsgBuilder::buildPointPlayer( const bool & our_side,
                              const UNum & unum )
{
    traceBuild< PointPlayer * >();
    std::shared_ptr< Point > p( new PointPlayer( our_side, unum ) );
    add( p );
}


void
MsgBuilder::buildPointArith( const rcss::util::ArithOp & arith_op )
{
    traceBuild< PointArith * >();
    // we need to make sure ordering is maintained, otherwise you could end
    // up with 'b / a', when 'a / b' was what was actually sent.
    std::shared_ptr< Point > second = checkAndGetPoint();
    std::shared_ptr< Point > first = checkAndGetPoint();

    std::shared_ptr< Point > p( new PointArith( first, second, arith_op ) );
    add ( p );
}


void
MsgBuilder::buildUNum( const UNum & unum )
{
    traceBuild< UNum * >();
    std::shared_ptr< UNum > u( new UNum( unum ) );
    add( u );
}


void
MsgBuilder::buildUNumSet()
{
    traceBuild< UNumSet >();
    std::shared_ptr< UNumSet > unums( new UNumSet() );
    if( checkIsItem( UNUM ) )
    {
        while ( isItem( UNUM ) )
        {
            std::shared_ptr< UNum > u = getUNum();
            unums->add( *u );
        }
    }
    add( unums );
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
    std::shared_ptr< RuleMsg > msg( new RuleMsg() );
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
    std::shared_ptr< ActivateRules > ptr( new ActivateRules( on, RuleIDList() ) );
    add( ptr );
}


void
MsgBuilder::buildActivateRules( const bool & on )
{
    traceBuild< ActivateRules >();
    std::shared_ptr< ActivateRules > act( new ActivateRules( on, RuleIDList() ) );
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
    std::shared_ptr< RuleIDList > rids( new RuleIDList() );
    if ( checkIsItem( RID ) )
    {
        while ( isItem( RID ) )
        {
            std::shared_ptr< RuleID > r = getRID();
            rids->push_front( *r );
        }
        // by adding the items to the front, the original order is
        // maintained.
    }

    add( rids );
}


void
MsgBuilder::buildRuleIDListALL()
{
    traceBuild< RuleIDList >();
    std::shared_ptr< RuleIDList > rids( new RuleIDList() );
    add( rids ); // an empty list == all rules.
}


void
MsgBuilder::buildDelMsg()
{
    traceBuild< DelMsg * >();
    std::shared_ptr< DelMsg > msg( new DelMsg() );
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

    std::shared_ptr< Def > def( new DefRule( id, checkAndGetRule(), true ) );
    add( def );
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

    std::shared_ptr< Def > def( new DefRule( id, checkAndGetRule(), false ) );
    add( def );
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
            dirs.push_front( getDir() );
        }
        while( isItem( DIR ) );
    }

    std::shared_ptr< Rule > rule( new SimpleRule( checkAndGetCond(), dirs ) );
    add( rule );
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
            std::shared_ptr< Rule > r = getRule();
            rules.push_front( r );
        }
        while ( isItem( RULE ) );
    }

    std::shared_ptr< Rule > rule( new NestedRule( checkAndGetCond(), rules ) );
    add( rule );
}


void
MsgBuilder::buildIDRule()
{
    traceBuild< IDListRule * >();
    std::shared_ptr< RuleIDList > rids = checkAndGetRIDList();
    std::shared_ptr< Rule > rule( new IDListRule( *rids ) );
    add( rule );
}

}
}
