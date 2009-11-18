// -*-c++-*-

/***************************************************************************
                                   cond.cpp
                               Clang conditions
                             -------------------
    begin                : ??-??-????
    copyright            : (C) 2002- by The RoboCup Soccer Server
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

#include "cond.h"

namespace rcss {
namespace clang {

const char * CondPlayMode::MODE_STRINGS[] = {
    "",
    "bko",
    "time_over",
    "play_on",
    "ko_our",
    "ko_opp",
    "ki_our",
    "ki_opp",
    "fk_our",
    "fk_opp",
    "ck_our",
    "ck_opp",
    "gk_our",
    "gk_opp",
    "gc_our",
    "gc_opp",
    "ag_our",
    "ag_opp" };


std::ostream &
CondBool::print( std::ostream & out ) const
{
    return out << "(" << ( getState() ? "true" : "false" ) << ")";
}

std::ostream &
CondBool::printPretty( std::ostream & out,
                       const std::string & line_header ) const
{
    return out << line_header << ( getState() ? "true" : "false" )
               << std::endl;
}


std::auto_ptr< Cond >
CondPlayerPos::deepCopy() const
{
    if ( M_reg.get() != NULL )
    {
        return std::auto_ptr< Cond >( new CondPlayerPos( M_our_side,
                                                         M_players,
                                                         M_min_match,
                                                         M_max_match,
                                                         M_reg->deepCopy() ) );
    }
    else
    {
        return std::auto_ptr< Cond >( new CondPlayerPos( M_our_side,
                                                         M_players,
                                                         M_min_match,
                                                         M_max_match,
                                                         std::auto_ptr< Region >() ) );
    }
}


std::ostream &
CondPlayerPos::print( std::ostream & out ) const
{
    out << "(ppos"
        << " " << ( M_our_side ? "our" : "opp" )
        << " " << M_players
        << " " << M_min_match
        << " " << M_max_match
        << " ";
    if ( M_reg.get() == NULL )
    {
        out << "(null)";
    }
    else
    {
        out << *M_reg;
    }
    out << ")";
    return out;
}

std::ostream &
CondPlayerPos::printPretty( std::ostream & out,
                            const std::string & line_header ) const
{
    out << line_header
        << "are "
        << M_min_match << "-" << M_max_match << " "
        << (M_our_side ? "our team" : "opponent") << " "
        << M_players << " "
        << "in:" << std::endl;
    if ( M_reg.get() == NULL )
    {
        out << line_header << " (null)\n";
    }
    else
    {
        M_reg->printPretty( out, line_header + " " );
    }
    return out;
}


std::ostream &
CondBallPos::print( std::ostream & out ) const
{
    out << "(bpos ";
    if ( getRegion() == NULL )
    {
        out << "(null)";
    }
    else
    {
        out << *getRegion();
    }
    return out << ")";
}

std::ostream &
CondBallPos::printPretty( std::ostream & out,
                          const std::string & line_header ) const
{
    out << line_header << "is ball position at: " << std::endl;
    if ( getRegion() == NULL )
    {
        out << line_header << " (null)\n";
    }
    else
    {
        getRegion()->printPretty( out, line_header + " " );
    }
    return out;
}

std::auto_ptr< Cond >
CondBallPos::deepCopy() const
{
    if ( getRegion() )
    {
        return std::auto_ptr< Cond >( new CondBallPos( M_reg->deepCopy() ) );
    }
    else
    {
        return std::auto_ptr< Cond >( new CondBallPos() );
    }
}


std::ostream&
CondBallOwner::print( std::ostream& out ) const
{
    return out << "(bowner"
               << " " << (M_our_side ? "our" : "opp")
               << " " << M_players
               << ")";
}

std::ostream&
CondBallOwner::printPretty( std::ostream& out, const std::string& line_header ) const
{
    return out << line_header
               << "is ball controlled by someone in "
               << (M_our_side ? "our team" : "opponent") << " "
               << M_players << std::endl;
}


std::ostream &
CondPlayMode::print( std::ostream & out ) const
{
    return out << "(playm " << MODE_STRINGS[ M_pm ] << ")";
}

std::ostream &
CondPlayMode::printPretty( std::ostream & out,
                           const std::string & line_header ) const
{
    return out << line_header << "is play mode '"
               << MODE_STRINGS[ M_pm ] << "'"
               << std::endl;
}



CondAnd::~CondAnd()
{
    for ( Storage::iterator i = M_conds.begin();
          i != M_conds.end();
          ++i )
    {
        delete *i;
    }
    M_conds.clear();
}

std::auto_ptr< Cond >
CondAnd::deepCopy() const
{
    Storage new_conds;
    for ( Storage::const_iterator i = M_conds.begin();
          i != M_conds.end();
          ++i )
    {
        new_conds.push_back( (*i)->deepCopy().release() );
    }
    return std::auto_ptr< Cond >( new CondAnd( new_conds ) );
}

std::ostream &
CondAnd::print( std::ostream & out ) const
{
    out << "(and";
    for ( Storage::const_iterator iter = getConds().begin();
          iter != getConds().end();
          ++iter )
    {
        if ( *iter == NULL )
        {
            out << " (null)";
        }
        else
        {
            out << " " << **iter;
        }
    }
    return out << ")";
}

std::ostream &
CondAnd::printPretty( std::ostream & out,
                      const std::string & line_header ) const
{
    out << line_header << "and" << std::endl;
    for ( Storage::const_iterator iter = getConds().begin();
          iter != getConds().end();
          ++iter )
    {
        if ( *iter == NULL )
        {
            out << line_header << " +(null)\n";
        }
        else
        {
            (*iter)->printPretty( out, line_header + " +" );
        }
    }
    return out;
}

bool
CondAnd::eval( const Context & context ) const
{
    for ( Storage::const_iterator iter = getConds().begin();
          iter != getConds().end();
          ++iter )
    {
        if ( *iter == NULL )
        {
            throw util::NullErr( __FILE__, __LINE__,
                                 "Null condition in CondAnd\n" );
        }

        if ( !(*iter)->eval( context ) )
        {
            return false;
        }
    }
    return true;
}


CondOr::~CondOr()
{
    for ( Storage::iterator i = M_conds.begin();
          i != M_conds.end();
          ++i )
    {
        delete *i;
    }
    M_conds.clear();
}

std::auto_ptr< Cond >
CondOr::deepCopy() const
{
    Storage new_conds;
    for ( Storage::const_iterator i = M_conds.begin();
          i != M_conds.end();
          ++i )
    {
        new_conds.push_back( (*i)->deepCopy().release() );
    }
    return std::auto_ptr< Cond >( new CondOr( new_conds ) );
}

std::ostream &
CondOr::print( std::ostream & out ) const
{
    out << "(or";
    for( Storage::const_iterator iter = getConds().begin();
         iter != getConds().end(); ++iter)
    {
        if( *iter == NULL )
            out << " (null)";
        else
            out << " " << **iter;
    }
    return out << ")";
}

std::ostream&
CondOr::printPretty( std::ostream& out, const std::string& line_header ) const
{
    out << line_header << "or" << std::endl;
    for( Storage::const_iterator iter = getConds().begin();
         iter != getConds().end(); ++iter )
    {
        if( *iter == NULL )
            out << line_header << " +(null)\n";
        else
            (*iter)->printPretty( out, line_header + " +" );
    }
    return out;
}

bool
CondOr::eval( const Context & context ) const
{
    for ( Storage::const_iterator iter = getConds().begin();
          iter != getConds().end();
          ++iter )
    {
        if ( *iter == NULL )
        {
            throw util::NullErr( __FILE__, __LINE__,
                                 "Null condition in CondOr\n" );
        }

        if ( (*iter)->eval( context ) )
        {
            return true;
        }
    }
    return false;
}


std::auto_ptr< Cond >
CondNot::deepCopy() const
{
    if ( M_cond.get() != NULL )
    {
        return std::auto_ptr< Cond >( new CondNot( M_cond->deepCopy() ) );
    }
    else
    {
        return std::auto_ptr< Cond >( new CondNot() );
    }
}

std::ostream &
CondNot::print( std::ostream & out ) const
{
    out << "(not ";
    if ( getCond() == NULL )
    {
        out << "(null)\n";
    }
    else
    {
        out << *getCond();
    }
    return out << ")";
}

std::ostream &
CondNot::printPretty( std::ostream & out,
                      const std::string & line_header ) const
{
    out << line_header << "not " << std::endl;
    if ( getCond() == NULL )
    {
        out << line_header << " (null)\n";
    }
    else
    {
        getCond()->printPretty( out, line_header + " +" );
    }
    return out;
}

bool
CondNot::eval( const Context & context ) const
{
    if ( getCond() == NULL )
    {
        throw util::NullErr( __FILE__, __LINE__,
                             "Null condition in CondNot\n" );
    }
    else
    {
        return ! ( getCond()->eval( context ) );
    }
}


std::ostream &
CondNamed::print( std::ostream & out ) const
{
    return out << "\"" << M_name << "\"";
}

std::ostream &
CondNamed::printPretty( std::ostream & out,
                        const std::string & line_header ) const
{
    return out << line_header << "cond named \""
               << M_name << "\"" << std::endl;
}


std::ostream &
CondTime::print( std::ostream & out ) const
{
    return out << "(time " << getComp() << " " << getValue() << ")";
}

std::ostream &
CondTime::printPretty( std::ostream & out,
                       const std::string & line_header ) const
{
    return print( out << line_header );
}


std::ostream &
CondOppGoal::print( std::ostream & out ) const
{
    return out << "(opp_goals " << getComp() << " " << getValue() << ")";
}

std::ostream &
CondOppGoal::printPretty( std::ostream & out,
                          const std::string & line_header ) const
{
    return print( out << line_header );
}


std::ostream &
CondOurGoal::print( std::ostream & out ) const
{
    return out << "(our_goals " << getComp() << " " << getValue() << ")";
}

std::ostream &
CondOurGoal::printPretty( std::ostream & out,
                          const std::string & line_header ) const
{
    return print( out << line_header );
}


std::ostream &
CondGoalDiff::print( std::ostream & out ) const
{
    return out << "(goal_diff " << getComp() << " " << getValue() << ")";
}

std::ostream &
CondGoalDiff::printPretty( std::ostream & out,
                           const std::string & line_header ) const
{
    return print( out << line_header );
}


std::ostream &
CondUNum::print( std::ostream & out ) const
{
    return out << "(unum " << M_unum << " " << M_set << ")";
}

std::ostream &
CondUNum::printPretty( std::ostream & out,
                       const std::string & line_header ) const
{
    return print( out << line_header );
}


}
}
