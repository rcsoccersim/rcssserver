// -*-c++-*-

/***************************************************************************
                                   cond.h
                               Clang conditions
                             -------------------
    begin                : 18-MAR-2002
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

#ifndef CLANG_COND_H
#define CLANG_COND_H

#include "clangutil.h"
#include "region.h"
#include "compop.h"
#include "rcssexceptions.h"

#include <iosfwd>
#include <memory>

namespace rcss {
namespace clang {

/******** Conditions ********************/
class CondBool;
class CondPlayerPos;
class CondBallPos;
class CondBallOwner;
class CondPlayMode;
class CondAnd;
class CondOr;
class CondNot;
class CondNamed;
class CondTime;
class CondOppGoal;
class CondOurGoal;
class CondGoalDiff;
class CondUNum;

class Cond {
public:
    class Context {
    public:
        Context()
          { }

        virtual
        ~Context()
          { }

        virtual
        bool lookup( const CondPlayerPos & cond ) const = 0;

        virtual
        bool lookup( const CondBallPos & cond ) const = 0;

        virtual
        bool lookup( const CondBallOwner & cond ) const = 0;

        virtual
        bool lookup( const CondPlayMode & cond ) const = 0;

        virtual
        bool lookup( const CondNamed & cond ) const = 0;

        virtual
        bool lookup( const CondTime & cond ) const = 0;

        virtual
        bool lookup( const CondOppGoal & cond ) const = 0;

        virtual
        bool lookup( const CondOurGoal & cond ) const = 0;

        virtual
        bool lookup( const CondGoalDiff & cond ) const = 0;

        virtual
        bool lookup( const CondUNum & cond ) const = 0;
    }; // end Context

    Cond()
      { }

    virtual
    ~Cond()
      { }

    virtual
    std::ostream & print( std::ostream & out ) const = 0;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const = 0;

    virtual
    bool eval( const Context & context ) const = 0;

    virtual
    std::auto_ptr< Cond > deepCopy() const = 0;
};

inline
std::ostream &
operator<<( std::ostream & os,
            const Cond & c )
{
    return c.print( os );
}


class CondBool
    : public Cond {
public:

    CondBool()
        : Cond(),
          M_state( false )
      { }

    CondBool( bool state )
        : Cond(),
          M_state( state )
      { }

    ~CondBool()
      { }

    virtual
    std::ostream & print( std::ostream & out ) const;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const;

    virtual
    bool eval( const Context & ) const
      {
          return M_state;
      }

    bool getState() const
      {
          return M_state;
      }

    virtual
    std::auto_ptr< Cond > deepCopy() const
      {
          return std::auto_ptr< Cond >( new CondBool( *this ) );
      }

private:
	bool M_state;
};


class CondPlayerPos
    : public Cond {
private:
    CondPlayerPos()
        : Cond(),
          M_our_side( false ),
          M_min_match( 1 ),
          M_max_match( 11 )
      {}
public:
    CondPlayerPos( const bool & our_side,
                   const UNumSet & players,
                   const int & min_match,
                   const int & max_match,
                   std::auto_ptr< Region > reg )
        : Cond(),
          M_our_side( our_side ),
          M_min_match( min_match ),
          M_max_match( max_match ),
          M_players( players ),
          M_reg( reg )
      {}

    virtual
    ~CondPlayerPos()
      { }

    virtual
    std::auto_ptr< Cond > deepCopy() const;


    virtual
    std::ostream & print( std::ostream & out ) const;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const;

    virtual
    bool eval( const Context & context ) const
      {
          return context.lookup( *this );
      }

    /* class specific stuff */
    bool isOurSide() const
      {
          return M_our_side;
      }

    bool isTheirSide() const
      {
          return !M_our_side;
      }

    int getMinMatch() const
      {
          return M_min_match;
      }

    int getMaxMatch() const
      {
          return M_max_match;
      }

    const Region * getRegion() const
      {
          return M_reg.get();
      }

    UNumSet getPlayerSet()
      {
          return M_players;
      }

    void setMinMatch( int x )
      {
          M_min_match = x;
      }

    void setMaxMatch( int x )
      {
          M_max_match = x;
      }

    void set( std::auto_ptr< Region > reg )
      {
          M_reg = reg;
      }

    std::auto_ptr< Region > detachRegion()
      {
          return M_reg;
      }

    void setOurSide( const bool & our_side )
      {
          M_our_side = our_side;
      }

    void set( const UNumSet & players )
      {
          M_players = players;
      }

    void addPlayer( const UNum & i )
      {
          M_players.add( i );
      }

    void clearPlayers()
      {
          M_players.clear();
      }

private:
    bool M_our_side;
    int M_min_match;
    int M_max_match;
	UNumSet M_players;
	std::auto_ptr< Region > M_reg;
};

class CondBallPos
    : public Cond {
private:
    CondBallPos()
        : Cond()
      { }

public:
    CondBallPos( std::auto_ptr< Region > reg )
        : Cond(),
          M_reg( reg )
      { }

    ~CondBallPos()
      { }

    virtual
    std::auto_ptr< Cond > deepCopy() const;

    virtual
    std::ostream & print( std::ostream & out ) const;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const;

    virtual
    bool eval( const Context & context ) const
      {
          return context.lookup( *this );
      }

    const Region * getRegion() const
      {
          return M_reg.get();
      }

    std::auto_ptr< Region > detachRegion()
      {
          return M_reg;
      }

private:
	std::auto_ptr< Region > M_reg;
};

class CondBallOwner
    : public Cond {
private:
    CondBallOwner()
        : Cond(),
          M_our_side( false )
      {}

public:
    CondBallOwner( const bool & our_side,
                   const UNumSet & players )
        : Cond(),
          M_our_side( our_side ),
          M_players( players )
      {}

    ~CondBallOwner()
      {}

    virtual
    std::auto_ptr< Cond > deepCopy() const
      {
          return std::auto_ptr< Cond >( new CondBallOwner( *this ) );
      }

    virtual
    std::ostream & print( std::ostream & out ) const;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const;

    virtual
    bool eval( const Context & context ) const
      {
          return context.lookup( *this );
      }

    /* class specific stuff */
    bool isOurSide() const
      {
          return M_our_side;
      }

    bool isTheirSide() const
      {
          return ! M_our_side;
      }

    UNumSet & getPlayerSet()
      {
          return M_players;
      }

    const UNumSet & getPlayerSet() const
      {
          return M_players;
      }

    void setOurSide( const bool & our_side )
      {
          M_our_side = our_side;
      }

    void setPlayers( const UNumSet & players )
      {
          M_players = players;
      }

    void addPlayer( const UNum & i )
      {
          M_players.add( i );
      }

    void clearPlayers()
      {
          M_players.clear();
      }

private:
    bool M_our_side;
    UNumSet M_players;
};



class CondPlayMode
    : public Cond {
public:
    static const char * MODE_STRINGS[];

private:
    CondPlayMode()
        : Cond(),
          M_pm( PM_None )
      { }

public:
    CondPlayMode( const PlayMode& pm )
        : Cond(),
          M_pm( pm )
      { }

    ~CondPlayMode()
      { }

    virtual
    std::auto_ptr< Cond > deepCopy() const
      {
          return std::auto_ptr< Cond >( new CondPlayMode( *this ) );
      }

    virtual
    std::ostream & print( std::ostream & out ) const;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const;

    virtual
    bool eval( const Context & context ) const
      {
          return context.lookup( *this );
      }

    /* class specific stuff */
    PlayMode getPlayMode() const
      {
          return M_pm;
      }

    void setPlayMode( const PlayMode & pm )
      {
          M_pm = pm;
      }

private:
    PlayMode M_pm;
};


class CondAnd
    : public Cond {
public:
	typedef std::list< Cond * > Storage;

private:
    CondAnd()
        : Cond()
      { }

public:
    CondAnd( const Storage & conds )
        : Cond(),
          M_conds( conds )
      { }

    virtual
    ~CondAnd();

    virtual
    std::auto_ptr< Cond > deepCopy() const;

    virtual
    std::ostream & print( std::ostream & out ) const;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const;

    virtual
    bool eval( const Context & context ) const;

    const Storage & getConds() const
      {
          return M_conds;
      }

private:
	Storage M_conds;

};


class CondOr
    : public Cond {
public:
 	typedef std::list< Cond * > Storage;

private:
    CondOr()
        : Cond()
      { }

public:
    CondOr( const Storage & conds )
        : Cond(),
          M_conds( conds )
      { }

    virtual
    ~CondOr();

    virtual
    std::auto_ptr< Cond > deepCopy() const;

    virtual
    std::ostream & print( std::ostream & out ) const;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const;

    virtual
    bool eval( const Context & context ) const;

    const Storage & getConds() const
      {
          return M_conds;
      }

private:
	Storage M_conds;
};


class CondNot
    : public Cond {
public:

    CondNot()
        : Cond()
      { }

    CondNot( std::auto_ptr< Cond > cond )
        : Cond(),
          M_cond( cond )
      { }

    virtual
    ~CondNot()
      { }

    virtual
    std::auto_ptr< Cond > deepCopy() const;

    virtual
    std::ostream & print( std::ostream & out ) const;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const;

    virtual
    bool eval( const Context & context ) const;

    const Cond * getCond() const
      {
          return M_cond.get();
      }

    std::auto_ptr< Cond > detachCond()
      {
          return M_cond;
      }

private:
	std::auto_ptr< Cond > M_cond;
};


class CondNamed
    : public Cond {
private:
    CondNamed()
        : Cond()
      { }

public:
    CondNamed( const std::string & name )
        : Cond(),
          M_name( name )
      {}

    ~CondNamed()
      {}

    virtual
    std::auto_ptr< Cond > deepCopy() const
      {
          return std::auto_ptr< Cond >( new CondNamed( *this ) );
      }

    virtual
    std::ostream & print( std::ostream & out ) const;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const;

    virtual
    bool eval( const Context & context ) const
      {
          return context.lookup ( *this );
      }

    /* class specific */
//     std::string & getName()
//       {
//           return M_name;
//       }

    const std::string & getName() const
      {
          return M_name;
      }

//     void setName( const std::string & name )
//       {
//           M_name = name;
//       }

private:
    std::string M_name;
};

template< typename VALUE >
class CondComp
    : public Cond
{
private:
    const util::CompOp* M_comp;
    VALUE M_value;

public:
    CondComp( const VALUE & value,
              const util::CompOp & comp )
        : M_comp( &comp ),
          M_value( value )
      { }

    ~CondComp()
      { }

    VALUE getValue() const
      {
          return M_value;
      }

    VALUE setValue( const VALUE & value )
      {
          return M_value = value;
      }

    const util::CompOp & getComp() const
      {
          return *M_comp;
      }

    const util::CompOp & setComp( const util::CompOp & comp )
      {
          return *(M_comp = &comp);
      }
};

class CondTime
    : public CondComp< int > {
public:
    CondTime( const int & value,
              const util::CompOp & comp )
        : CondComp< int >( value, comp )
      { }

    ~CondTime()
      { }

    virtual
    std::auto_ptr< Cond > deepCopy() const
      {
          return std::auto_ptr< Cond >( new CondTime( *this ) );
      }

    virtual
    std::ostream & print( std::ostream & out ) const;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const;

    virtual
    bool eval( const Context & context ) const
      {
          return context.lookup( *this );
      }
};


class CondOppGoal
    : public CondComp< int > {
public:
    CondOppGoal( const int & value,
                 const util::CompOp & comp )
        : CondComp< int >( value, comp )
      { }

    ~CondOppGoal()
      { }

    virtual
    std::auto_ptr< Cond > deepCopy() const
      {
          return std::auto_ptr< Cond >( new CondOppGoal( *this ) );
      }

    virtual
    std::ostream & print( std::ostream & out ) const;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const;

    virtual
    bool eval( const Context & context ) const
      {
          return context.lookup( *this );
      }
};

class CondOurGoal
    : public CondComp< int > {
public:
    CondOurGoal( const int & value,
                 const util::CompOp & comp )
        : CondComp< int >( value, comp )
      { }

    ~CondOurGoal()
      { }

    virtual
    std::auto_ptr< Cond > deepCopy() const
      {
          return std::auto_ptr< Cond >( new CondOurGoal( *this ) );
      }

    virtual
    std::ostream & print( std::ostream & out ) const;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const;

    virtual
    bool eval( const Context & context ) const
      {
          return context.lookup( *this );
      }
};

class CondGoalDiff
    : public CondComp< int > {
public:
    CondGoalDiff( const int & value,
                  const util::CompOp & comp )
        : CondComp< int >( value, comp )
      { }

    ~CondGoalDiff()
      { }

    virtual
    std::auto_ptr< Cond > deepCopy() const
      {
          return std::auto_ptr< Cond >( new CondGoalDiff( *this ) );
      }

    virtual
    std::ostream & print( std::ostream & out ) const;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const;

    virtual
    bool eval( const Context & context ) const
      {
          return context.lookup( *this );
      }
};


class CondUNum
    : public Cond {
private:
    CondUNum()
        : Cond(),
          M_unum(),
          M_set()
      { }

public:
    CondUNum( const UNum & unum )
        : Cond(),
          M_unum( unum ),
          M_set()
      { }

    CondUNum( const UNum & unum,
              const UNumSet & players )
        : Cond(),
          M_unum( unum ),
          M_set( players )
      { }

    ~CondUNum()
      { }

    virtual
    std::auto_ptr< Cond > deepCopy() const
      {
          return std::auto_ptr< Cond >( new CondUNum( *this ) );
      }

    virtual
    std::ostream & print( std::ostream & out ) const;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const;

    virtual
    bool eval( const Context & context ) const
      {
          return context.lookup( *this );
      }

//     UNum & setUNum( const UNum & unum )
//       {
//           return M_unum = unum;
//       }

    UNum getUNum() const
      {
          return M_unum;
      }

//     UNumSet & setUNum( const UNumSet & uset )
//       {
//           return M_set = uset;
//       }

//     UNumSet & getUNumSet()
//       {
//           return M_set;
//       }

    const UNumSet & getUNumSet() const
      {
          return M_set;
      }

//     UNum addUNum( const UNum & unum )
//       {
//           M_set.add( unum );
//           return unum;
//       }

private:
    UNum M_unum;
    UNumSet M_set;
};

}
}

#endif
