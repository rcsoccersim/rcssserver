// -*-c++-*-

/***************************************************************************
                                    rule.cc
                           Classes for clang rules
                             -------------------
    begin                : 25-APR-2002
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

#include "rule.h"

#include "cond.h"
#include "coach_lang_comp.h"

namespace rcss {
namespace clang {

// bool
// RuleIDList::empty() const
// {
//     return std::list< RuleID >::empty();
// }

bool
RuleIDList::all() const
{
//     return empty();
    return M_list.empty();
}

bool
RuleIDList::unary() const
{
    //return size() == 1;
    return M_list.size() == 1;
}

std::ostream &
RuleIDList::print( std::ostream & o ) const
{
    if ( all() )
    {
        return o << "all";
    }
    else if ( unary() )
    {
        //return o << front();
        return o << M_list.front();
    }
    else
    {
        bool space = false;
        //for ( const_iterator i = begin(); i != end(); ++i )
        for ( std::list< RuleID >::const_iterator i = M_list.begin();
              i != M_list.end();
              ++i )
        {
            if ( space )
            {
                o << " " << *i;
            }
            else
            {
                o << "(" << *i;
                space = true;
            }
        }
        return o << ")";
    }
}

std::ostream &
RuleIDList::printPretty( std::ostream & out,
                         const std::string & line_header ) const
{
    return print( out << line_header ) << std::endl;
}



ActivateRules::ActivateRules()
{

}

ActivateRules::ActivateRules( const bool on,
                              const RuleIDList & list )
    : M_on( on ),
      M_rids( list )
{

}

ActivateRules::~ActivateRules()
{}

bool
ActivateRules::on() const
{
    return M_on;
}

bool
ActivateRules::all() const
{
    return M_rids.all();
}

bool
ActivateRules::unary() const
{
    return M_rids.unary();
}

const
RuleIDList &
ActivateRules::getList() const
{
    return M_rids;
}

void
ActivateRules::set( const bool on )
{
    M_on = on;
}

void
ActivateRules::set( const RuleIDList & rids )
{
    M_rids = rids;
}

std::ostream &
ActivateRules::print( std::ostream & o ) const
{
    o << "(" << ( on() ? "on" : "off" ) << " ";
    if ( all() )
    {
        o << "all";
    }
    else
    {
        o << getList();
    }
    return o << ")";
}

std::ostream&
ActivateRules::printPretty( std::ostream& o,
                            const std::string& line_header ) const
{
    o << line_header << "Turn " << ( on() ? "on" : "off" );
    if ( all() )
    {
        o << " all rules\n";
    }
    else
    {
        if ( unary() )
        {
            o << " rule " << getList() << std::endl;
        }
        else
        {
            o << " rules:\n";
            getList().printPretty( o, line_header + "  -" );
        }
    }
    return o;
}



Rule::~Rule()
{

}


CondRule::CondRule( std::auto_ptr< Cond > cond )
    : M_cond( cond )
{

}

CondRule::~CondRule()
{

}

const
Cond *
CondRule::getCond() const
{
    return M_cond.get();
}

std::auto_ptr< Cond >
CondRule::detachCond()
{
    return M_cond;
}




SimpleRule::SimpleRule( std::auto_ptr< Cond > cond )
    : CondRule( cond )
{

}

SimpleRule::SimpleRule( std::auto_ptr< Cond > cond,
                        const Storage & dirs )
    : CondRule( cond ),
      M_dirs( dirs )
{

}

SimpleRule::~SimpleRule()
{
	for ( Storage::iterator i = M_dirs.begin();
          i != M_dirs.end();
          ++i )
    {
	    delete *i;
    }
	M_dirs.clear();
}

std::ostream &
SimpleRule::print( std::ostream & out ) const
{
    out << "(";
    if ( getCond() == NULL )
    {
        out << "(null)";
    }
    else
    {
        out << *getCond();
    }

    for ( Storage::const_iterator i = getDirs().begin();
          i != getDirs().end();
          ++i )
    {
        out << " " << **i;
    }
    return out << ")";
}

std::ostream &
SimpleRule::printPretty( std::ostream & out,
                         const std::string & lineheader ) const
{
    out << lineheader << "Simple Rule:\n";
    if ( getCond() == NULL )
    {
        out << lineheader << " if:(null)\n";
    }
    else
    {
        getCond()->printPretty( out, lineheader + " if: " );
    }

    for ( Storage::const_iterator i = getDirs().begin();
          i != getDirs().end();
          ++i )
    {
        out << (*i)->printPretty( out, lineheader + " -" );
    }
    return out;
}

std::auto_ptr< Rule >
SimpleRule::deepCopy() const
{
	Storage new_dirs;
	for ( Storage::const_iterator i = getDirs().begin();
          i != getDirs().end();
          ++i )
    {
	    new_dirs.push_back( (*i)->deepCopy().release() );
    }
	return std::auto_ptr< Rule >( ( Rule* )new SimpleRule( getCond()->deepCopy(), new_dirs ) );
}


const
SimpleRule::Storage &
SimpleRule::getDirs() const
{
    return M_dirs;
}



NestedRule::NestedRule( std::auto_ptr< Cond > cond )
    : CondRule( cond )
{

}

NestedRule::NestedRule( std::auto_ptr< Cond > cond,
                        const Storage & rules )
    : CondRule( cond ),
      M_rules( rules )
{

}

NestedRule::~NestedRule()
{
	for ( Storage::iterator i = M_rules.begin();
          i != M_rules.end();
          ++i )
    {
	    delete *i;
    }
	M_rules.clear();
}


std::ostream &
NestedRule::print( std::ostream & out ) const
{
    out << "(";
    if ( getCond() == NULL )
    {
        out << "(null)";
    }
    else
    {
        out << *getCond();
    }

    for ( Storage::const_iterator i = getRules().begin();
          i != getRules().end();
          ++i )
    {
        out << " " << **i;
    }
    return out << ")";
}

std::ostream &
NestedRule::printPretty( std::ostream & out,
                         const std::string & lineheader ) const
{
    out << lineheader << "Nested Rule:\n";
    if ( getCond() == NULL )
    {
        out << lineheader << " if:(null)\n";
    }
    else
    {
        getCond()->printPretty( out, lineheader + " if: " );
    }

    for ( Storage::const_iterator i = getRules().begin();
          i != getRules().end();
          ++i )
    {
        out << (*i)->printPretty( out, lineheader + " -" );
    }
    return out;
}

std::auto_ptr< Rule >
NestedRule::deepCopy() const
{
	Storage new_rules;
	for ( Storage::const_iterator i = getRules().begin();
          i != getRules().end();
          ++i )
    {
	    new_rules.push_back( (*i)->deepCopy().release() );
    }
	return std::auto_ptr< Rule >( ( Rule* )new NestedRule( getCond()->deepCopy(), new_rules ) );
}


const
NestedRule::Storage &
NestedRule::getRules() const
{
    return M_rules;
}




IDListRule::IDListRule( const RuleIDList & rules )
    : M_rids( rules )
{

}

IDListRule::~IDListRule()
{

}


std::ostream &
IDListRule::print( std::ostream & out ) const
{
    return out << M_rids;
}

std::ostream &
IDListRule::printPretty( std::ostream & out,
                         const std::string & lineheader ) const
{
    out << lineheader << "IDList Rule:\n";
    return out << M_rids.printPretty( out, lineheader + " -" );
}

std::auto_ptr< Rule >
IDListRule::deepCopy() const
{
    return std::auto_ptr< Rule >( ( Rule* )new IDListRule( *this ) );
}

const
RuleIDList &
IDListRule::getIDList() const
{
    return M_rids;
}




std::ostream &
operator<<( std::ostream & o,
            const RuleIDList & id_list )
{
    return id_list.print( o );
}

std::ostream &
operator<<( std::ostream & o,
            const ActivateRules & activ_rules )
{
    return activ_rules.print( o );
}

std::ostream &
operator<<( std::ostream & os,
            const Rule & r )
{
    return r.print( os );
}

}
}
