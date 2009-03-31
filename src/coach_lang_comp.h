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

#ifndef COACH_LANG_COMP_H
#define COACH_LANG_COMP_H

#include <iosfwd>
#include <vector>
#include <memory>

#include "region.h"
#include "clangutil.h"
#include "clangaction.h"

namespace rcss {
namespace clang {

/******** Dir ********************/
class DirComm;
class DirNamed;

class Dir {
public:
    Dir()
      { }

    virtual
    ~Dir()
      { }

    virtual
    std::ostream & print( std::ostream & out ) const = 0;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const = 0;

    virtual
    std::auto_ptr< Dir > deepCopy() const = 0;

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
	typedef std::list< Action * > Storage;

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

    ~DirComm()
      {
          clearActions();
      }

    virtual
    std::ostream & print( std::ostream & out ) const;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const;

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

    void add( std::auto_ptr< Action > action )
      {
          M_actions.push_front( action.release() );
      }

    Storage detachActions();

    virtual
    std::auto_ptr< Dir > deepCopy() const;


private:
	bool M_positive;
	bool M_our_side;
	UNumSet M_players;
	std::list< Action* > M_actions;
};

class DirNamed
    : public Dir {
public:
    DirNamed( const std::string & name = "" )
        : Dir(),
          M_name( name )
      { }

    ~DirNamed()
      { }

    virtual
    std::auto_ptr< Dir > deepCopy() const
      {
          return std::auto_ptr< Dir >( new DirNamed( *this ) );
      }

    virtual
    std::ostream & print( std::ostream & out ) const;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const;

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
    std::auto_ptr< Token > deepCopy() const = 0;

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

    ~TokClear()
      { }

    virtual
    std::ostream & print( std::ostream & out ) const;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const;

    virtual
    std::auto_ptr< Token > deepCopy() const
      {
          return std::auto_ptr< Token >( new TokClear( *this ) );
      }
};


class TokRule
	: public Token {
public:
    typedef std::list< Dir * > Storage;

    TokRule( const int & ttl = 0 )
        : Token(),
          M_ttl( ttl )
      { }

    TokRule( const int & ttl,
             std::auto_ptr< Cond > & cond,
             const Storage & dirs )
        : Token(),
          M_ttl( ttl ),
          M_cond( cond ),
          M_dirs( dirs )
      { }

    virtual
    ~TokRule()
      {
          clearDirs();
      }

    virtual
    std::auto_ptr< Token > deepCopy() const;

    virtual
    std::ostream & print( std::ostream & out ) const;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const;

    /* class specific stuff */
    int getTTL() const
      {
          return M_ttl;
      }

//     void set( int ttl )
//       {
//           M_ttl = ttl;
//       }

//     Cond * getCond()
//       {
//           return M_cond.get();
//       }

    const Cond * getCond() const
      {
          return M_cond.get();
      }

    void set( std::auto_ptr< Cond > cond )
      {
          M_cond = cond;
      }

//     std::auto_ptr< Cond > detachCond()
//       {
//           return M_cond;
//       }

    const Storage & getDirs() const
      {
          return M_dirs;
      }

    Storage & getDirs()
      {
          return M_dirs;
      }

 	void clearDirs();

//     void set( const Storage & dirs )
//       {
//           clearDirs();
//           M_dirs =  dirs;
//       }

private:
	int M_ttl;
	std::auto_ptr< Cond > M_cond;
	Storage M_dirs;
};

/******** Defs ********************/
class DefCond;
class DefDir;
class DefReg;
class DefAct;
class DefRule;

class Def {
public:
    Def()
      { }

    Def( const std::string & name )
        : M_name( name )
      { }

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
    std::auto_ptr< Def > deepCopy() const = 0;

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
          M_cond( (Cond*)NULL )
      { }

    DefCond( const std::string & name,
             std::auto_ptr< Cond > cond )
        : Def( name ),
          M_cond( cond )
      { }

    DefCond( const DefCond & def )
        : Def( def ),
          M_cond( def.M_cond->deepCopy() )
      { }

    ~DefCond()
      { }

    DefCond & operator=( const DefCond & def )
      {
          setName( def.getName() );
          std::auto_ptr< Cond > cond( def.M_cond->deepCopy() );
          M_cond = cond;
          return *this;
      }

    virtual
    std::auto_ptr< Def > deepCopy() const
      {
          return std::auto_ptr< Def >( new DefCond( *this ) );
      }

    virtual
    std::ostream & print( std::ostream & out ) const;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const;

    Cond * getCond()
      {
          return M_cond.get();
      }

    const Cond * getCond() const
      {
          return M_cond.get();
      }

    void setCond( std::auto_ptr< Cond > & cond )
      {
          M_cond = cond;
      }

    std::auto_ptr< Cond > detachCond()
      {
          return M_cond;
      }

private:
    std::auto_ptr< Cond > M_cond;
};


class DefDir
    : public Def {
public:
    DefDir()
        : Def(),
          M_dir( (Dir*)NULL )
      { }

    DefDir( const std::string & name,
            std::auto_ptr< Dir > dir )
        : Def( name ),
          M_dir( dir )
      { }

    DefDir( const DefDir & def )
        : Def( def ),
          M_dir( def.M_dir->deepCopy() )
      { }

    ~DefDir()
      { }

    DefDir & operator=( const DefDir & def )
      {
          setName( def.getName() );
          std::auto_ptr< Dir > dir( def.M_dir->deepCopy() );
          M_dir = dir;
          return *this;
      }

    virtual
    std::auto_ptr< Def > deepCopy() const
      {
          return std::auto_ptr< Def >( new DefDir( *this ) );
      }

    virtual
    std::ostream & print( std::ostream & out ) const;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const;

    Dir * getDir()
      {
          return M_dir.get();
      }

    const Dir * getDir() const
      {
          return M_dir.get();
      }

    void setDir( std::auto_ptr< Dir > & dir )
      {
          M_dir = dir;
      }

    std::auto_ptr< Dir > detachDir()
      {
          return M_dir;
      }

private:
    std::auto_ptr< Dir > M_dir;
};


class DefReg
    : public Def {
public:
    DefReg()
        : Def(),
          M_reg( (Region*)NULL )
      { }

    DefReg( const std::string & name,
            std::auto_ptr< Region > reg )
        : Def( name ),
          M_reg( reg )
      { }

    DefReg( const DefReg & def )
        : Def( def ),
          M_reg( def.M_reg->deepCopy() )
      { }

    ~DefReg()
      { }

    DefReg & operator=( const DefReg & def )
      {
          setName( def.getName() );
          std::auto_ptr< Region > reg( def.M_reg->deepCopy() );
          M_reg = reg;
          return *this;
      }

    virtual
    std::auto_ptr< Def > deepCopy() const
      {
          return std::auto_ptr< Def >( new DefReg( *this ) );
      }

    virtual
    std::ostream & print( std::ostream & out ) const;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const;

    Region * getReg()
      {
          return M_reg.get();
      }

    const Region * getReg() const
      {
          return M_reg.get();
      }

    void setReg( std::auto_ptr< Region > & reg )
      {
          M_reg = reg;
      }

    std::auto_ptr< Region > detachReg()
      {
          return M_reg;
      }

private:
    std::auto_ptr< Region > M_reg;
};


class DefAct
    : public Def {
public:
    DefAct()
        : Def(),
          M_act( (Action*)NULL )
      { }

    DefAct( const std::string & name,
            std::auto_ptr< Action > act )
        : Def( name ),
          M_act( act )
      { }

    DefAct( const DefAct & def )
        : Def( def ),
          M_act( def.M_act->deepCopy() )
      { }

    ~DefAct()
      { }

    DefAct & operator=( const DefAct & def )
      {
          setName( def.getName() );
          std::auto_ptr< Action > act( def.M_act->deepCopy() );
          M_act = act;
          return *this;
      }

    virtual
    std::auto_ptr< Def > deepCopy() const
      {
          return std::auto_ptr< Def >( new DefAct( *this ) );
      }

    virtual
    std::ostream & print( std::ostream & out ) const;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const;

    Action * getAction()
      {
          return M_act.get();
      }

    const Action * getAction() const
      {
          return M_act.get();
      }

    void setAction( std::auto_ptr< Action > & act )
      {
          M_act = act;
      }

    std::auto_ptr< Action > detachAction()
      {
          return M_act;
      }

private:
    std::auto_ptr< Action > M_act;
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
             std::auto_ptr< Rule > rule,
             bool model )
        : Def( name ),
          M_rule( rule ),
          M_model( model )
      { }

    virtual
    ~DefRule()
      { }

    virtual
    std::auto_ptr< Def > deepCopy() const;

    virtual
    std::ostream & print( std::ostream & out ) const;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const;

    const Rule * getRule() const
      {
          return M_rule.get();
      }

    std::auto_ptr< Rule > detachRule()
      {
          return M_rule;
      }

    bool isModel() const
      {
          return M_model;
      }

    void setModel( bool model )
      {
          M_model = model;
      }

private:
	std::auto_ptr< Rule > M_rule;
    bool M_model;
};

}
}

#endif
