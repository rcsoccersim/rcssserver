// -*-c++-*-

/***************************************************************************
                                    rule.h
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

#ifndef CLANG_RULE_H
#define CLANG_RULE_H

#include <iosfwd>
#include <string>
#include <list>
#include <memory>

namespace rcss {
namespace clang {

typedef std::string RuleID;

// If the rule list is empty then it counts a containing all
// rules.  This also means that there is no support for an empty
// rule ID list, but that is just the way we want it.
class RuleIDList
//    : public std::list< RuleID >
{
// private:
//     bool empty() const;
public:
    bool all() const;
    bool unary() const;

    void push_front( const RuleID & id )
      {
          M_list.push_front( id );
      }

    std::ostream & print( std::ostream & o ) const;
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const;
private:
    std::list< RuleID > M_list;
};

class ActivateRules {
public:
    ActivateRules();
    ActivateRules( const bool on,
                   const RuleIDList & list );
    ~ActivateRules();

    bool on() const;
    bool all() const;
    bool unary() const;
    const RuleIDList & getList() const;

    void set( const bool on );
    void set( const RuleIDList & rids );

    std::ostream & print( std::ostream & o ) const;
    std::ostream & printPretty( std::ostream & o,
                                const std::string & line_header ) const;
private:
	bool M_on;
	RuleIDList M_rids;
};

class Cond;
class Dir;

class Rule {
public:
    virtual
    ~Rule();

    virtual
    std::shared_ptr< Rule > deepCopy() const = 0;

    virtual
    std::ostream & print( std::ostream & out ) const = 0;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const = 0;
};

class CondRule
    : public Rule {
public:

	CondRule( std::shared_ptr< Cond > cond );

	virtual
	~CondRule() override;

	const std::shared_ptr< const Cond > & getCond() const
      {
          return M_cond;
      }

private:
	std::shared_ptr< const Cond > M_cond;
};


class SimpleRule
    : public CondRule {
public:
    typedef std::list< std::shared_ptr< Dir > > Storage;

    SimpleRule( std::shared_ptr< Cond > cond );
    SimpleRule( std::shared_ptr< Cond > cond,
                const Storage & dirs );
    ~SimpleRule() override;

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

    virtual
    std::shared_ptr< Rule > deepCopy() const;

    const Storage & getDirs() const;

private:
	Storage M_dirs;
};


class NestedRule
    : public CondRule {
public:
    typedef std::list< std::shared_ptr< Rule > > Storage;

    NestedRule( std::shared_ptr< Cond > cond );

    NestedRule( std::shared_ptr< Cond > cond,
                const Storage & rules );

    virtual
    ~NestedRule() override;

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

    virtual
    std::shared_ptr< Rule > deepCopy() const override;

    const Storage & getRules() const;

private:
	Storage M_rules;
};

class IDListRule
    : public Rule {
public:

    IDListRule();
    IDListRule( const RuleIDList & rules );
    ~IDListRule() override;

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

    virtual
    std::shared_ptr< Rule > deepCopy() const override;

    const RuleIDList & getIDList() const;

private:
	RuleIDList M_rids;
};


std::ostream&
operator<<( std::ostream& o, const rcss::clang::RuleIDList& id_list );

std::ostream&
operator<<( std::ostream& o, const rcss::clang::ActivateRules& activ_rules );

std::ostream&
operator<<( std::ostream & os, const rcss::clang::Rule& r );

}
}

#endif
