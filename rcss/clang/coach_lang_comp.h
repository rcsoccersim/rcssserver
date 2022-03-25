/* -*- Mode: C++ -*- */

/*
 *Copyright:

 Copyright (C) 1996-2000 Electrotechnical Laboratory.
 Itsuki Noda, Yasuo Kuniyoshi and Hitoshi Matsubara.
 Copyright (C) 2000, 2001 RoboCup Soccer Server Maintainance Group.
 Patrick Riley, Tom Howard, Daniel Polani, Itsuki Noda,
 Mikhail Prokopenko, Jan Wendler
 Copyright (C) 2002- RoboCup Soccer Simulator Maintainance Group.

 This file is a part of SoccerServer.

 This code is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 3 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 *EndCopyright:
 */


/* This file contains miscellaneous components for the coach language */

#ifndef RCSS_COACH_LANG_COMP_H
#define RCSS_COACH_LANG_COMP_H

#include <rcss/clang/region.h>
#include <rcss/clang/clangutil.h>
#include <rcss/clang/clangaction.h>

#include <iosfwd>
#include <vector>
#include <memory>

namespace rcss {
namespace clang {

/******** Dir ********************/
class DirComm;
class DirNamed;

class Dir {
protected:
    Dir() = default;

public:
    virtual
    ~Dir()
      { }

    virtual
    std::ostream & print( std::ostream & out ) const = 0;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const = 0;

    virtual
    std::shared_ptr< Dir > deepCopy() const = 0;

}; // end Dir

inline
std::ostream &
operator<<( std::ostream & os,
            const Dir & d )
{
    return d.print(os);
}


class DirComm
    : public Dir {
public:
	typedef std::list< std::shared_ptr< Action > > Storage;

    DirComm()
        : Dir()
      { }

    DirComm( bool positive,
             bool our_side,
             const UNumSet & players,
             const Storage & actions )
        : Dir(),
          M_positive( positive ),
          M_our_side( our_side ),
          M_players( players ),
          M_actions( actions )
      { }

    ~DirComm() override
      {
          clearActions();
      }

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

    bool isPositive() const
      {
          return M_positive;
      }

    bool isNegative() const
      {
          return ! isPositive();
      }

    bool isOurSide() const
      {
          return M_our_side;
      }

    bool isTheirSide() const
      {
          return ! isOurSide();
      }

    UNumSet getPlayerSet() const
      {
          return M_players;
      }

    const Storage & getActions() const
      {
          return M_actions;
      }

    void setSense( const bool & positive )
      {
          M_positive = positive;
      }

    void setOurSide( const bool & our_side )
      {
          M_our_side = our_side;
      }

    void set( const UNumSet & players )
      {
          M_players =  players;
      }

    void add( const UNum & i )
      {
          M_players.add( i );
      }

    void clearPlayers()
      {
          M_players.clear();
      }

	void clearActions();

    void set( const Storage & actions )
      {
          clearActions();
          M_actions = actions;
      }

    void add( std::shared_ptr< Action > action )
      {
          M_actions.push_front( action );
      }

    virtual
    std::shared_ptr< Dir > deepCopy() const override;


private:
	bool M_positive;
	bool M_our_side;
	UNumSet M_players;
	Storage M_actions;
};

class DirNamed
    : public Dir {
public:
    DirNamed( const std::string & name = "" )
        : Dir(),
          M_name( name )
      { }

    ~DirNamed() override
      { }

    virtual
    std::shared_ptr< Dir > deepCopy() const
      {
          return std::shared_ptr< Dir >( new DirNamed( *this ) );
      }

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

    /* class specific */
    std::string & getName()
      {
          return M_name;
      }

    const std::string & getName() const
      {
          return M_name;
      }

    void setName( const std::string & name )
      {
          M_name = name;
      }

private:
    std::string M_name;
};

}
}

#include "cond.h"


namespace rcss {
namespace clang {

/******** Tokens ********************/
class TokClear;
class TokRule;

class Token {
public:
    Token()
      { }

    virtual
    ~Token()
      { }

    virtual
    std::ostream & print( std::ostream & out ) const = 0;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const = 0;

    virtual
    std::shared_ptr< Token > deepCopy() const = 0;

};

inline
std::ostream &
operator <<( std::ostream & os,
             const Token & t )
{
    return t.print( os );
}


class TokClear
    : public Token {
public:
    TokClear()
        : Token()
      { }

    ~TokClear() override
      { }

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

    virtual
    std::shared_ptr< Token > deepCopy() const
      {
          return std::shared_ptr< Token >( new TokClear( *this ) );
      }
};


class TokRule
	: public Token {
public:
    typedef std::list< std::shared_ptr< Dir > > Storage;

    TokRule( const int & ttl = 0 )
        : Token(),
          M_ttl( ttl )
      { }

    TokRule( const int & ttl,
             std::shared_ptr< Cond > & cond,
             const Storage & dirs )
        : Token(),
          M_ttl( ttl ),
          M_cond( cond ),
          M_dirs( dirs )
      { }

    virtual
    ~TokRule() override
      {
          clearDirs();
      }

    virtual
    std::shared_ptr< Token > deepCopy() const override;

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

    /* class specific stuff */
    int getTTL() const
      {
          return M_ttl;
      }

    void setCond( std::shared_ptr< Cond > cond )
      {
          M_cond = cond;
      }

    const Storage & getDirs() const
      {
          return M_dirs;
      }

    Storage & getDirs()
      {
          return M_dirs;
      }

 	void clearDirs();

private:
	int M_ttl;
	std::shared_ptr< Cond > M_cond;
	Storage M_dirs;
};

/******** Defs ********************/
class DefCond;
class DefDir;
class DefReg;
class DefAct;
class DefRule;

class Def {
protected:
    Def()
      { }

    explicit
    Def( const std::string & name )
        : M_name( name )
      { }

public:

    virtual
    ~Def()
      { }

    virtual
    std::ostream & print( std::ostream & out ) const = 0;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const = 0;

    const std::string & getName() const
      {
          return M_name;
      }

    void setName( const std::string & name )
      {
          M_name = name;
      }

    virtual
    std::shared_ptr< Def > deepCopy() const = 0;

protected:
    std::string M_name;
};

inline
std::ostream &
operator <<( std::ostream & os,
             const Def & t )
{
    return t.print( os );
}


class DefCond
    : public Def {
public:
    DefCond()
        : Def(),
          M_cond()
      { }

    DefCond( const std::string & name,
             std::shared_ptr< Cond > cond )
        : Def( name ),
          M_cond( cond )
      { }

    DefCond( const DefCond & def )
        : Def( def ),
          M_cond( def.M_cond->deepCopy() )
      { }

    ~DefCond() override
      { }

    DefCond & operator=( const DefCond & def )
      {
          setName( def.getName() );
          std::shared_ptr< Cond > cond( def.M_cond->deepCopy() );
          M_cond = cond;
          return *this;
      }

    virtual
    std::shared_ptr< Def > deepCopy() const override
      {
          return std::shared_ptr< Def >( new DefCond( *this ) );
      }

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

private:
    std::shared_ptr< Cond > M_cond;
};


class DefDir
    : public Def {
public:
    DefDir()
        : Def(),
          M_dir()
      { }

    DefDir( const std::string & name,
            std::shared_ptr< Dir > dir )
        : Def( name ),
          M_dir( dir )
      { }

    DefDir( const DefDir & def )
        : Def( def ),
          M_dir( def.M_dir->deepCopy() )
      { }

    ~DefDir() override
      { }

    DefDir & operator=( const DefDir & def )
      {
          setName( def.getName() );
          std::shared_ptr< Dir > dir( def.M_dir->deepCopy() );
          M_dir = dir;
          return *this;
      }

    virtual
    std::shared_ptr< Def > deepCopy() const override
      {
          return std::shared_ptr< Def >( new DefDir( *this ) );
      }

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

private:
    std::shared_ptr< Dir > M_dir;
};


class DefReg
    : public Def {
public:
    DefReg()
        : Def(),
          M_reg()
      { }

    DefReg( const std::string & name,
            std::shared_ptr< Region > reg )
        : Def( name ),
          M_reg( reg )
      { }

    DefReg( const DefReg & def )
        : Def( def ),
          M_reg( def.M_reg->deepCopy() )
      { }

    ~DefReg() override
      { }

    DefReg & operator=( const DefReg & def )
      {
          setName( def.getName() );
          std::shared_ptr< Region > reg( def.M_reg->deepCopy() );
          M_reg = reg;
          return *this;
      }

    virtual
    std::shared_ptr< Def > deepCopy() const override
      {
          return std::shared_ptr< Def >( new DefReg( *this ) );
      }

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

private:
    std::shared_ptr< Region > M_reg;
};


class DefAct
    : public Def {
public:
    DefAct()
        : Def(),
          M_act()
      { }

    DefAct( const std::string & name,
            std::shared_ptr< Action > act )
        : Def( name ),
          M_act( act )
      { }

    DefAct( const DefAct & def )
        : Def( def ),
          M_act( def.M_act->deepCopy() )
      { }

    ~DefAct() override
      { }

    DefAct & operator=( const DefAct & def )
      {
          setName( def.getName() );
          std::shared_ptr< Action > act( def.M_act->deepCopy() );
          M_act = act;
          return *this;
      }

    virtual
    std::shared_ptr< Def > deepCopy() const override
      {
          return std::shared_ptr< Def >( new DefAct( *this ) );
      }

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

private:
    std::shared_ptr< Action > M_act;
};

}
}

#include "rule.h"

namespace rcss {
namespace clang {
class DefRule
    : public Def {
public:

    DefRule()
        : Def(),
          M_model( false )
      { }

    DefRule( const std::string & name,
             std::shared_ptr< Rule > rule,
             bool model )
        : Def( name ),
          M_rule( rule ),
          M_model( model )
      { }

    virtual
    ~DefRule()
      { }

    virtual
    std::shared_ptr< Def > deepCopy() const override;

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

private:
	std::shared_ptr< Rule > M_rule;
    bool M_model;
};

}
}

#endif
