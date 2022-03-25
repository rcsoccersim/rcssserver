// -*-c++-*-

/***************************************************************************
                                clangaction.cc
                       Abstract base class for CLang Actions
                             -------------------
    begin                : 28-MAY-2002
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

#include "clangaction.h"

namespace rcss {
namespace clang {
//     void
//     Action::TypeExtractor::visit( ActPos* act )
//     { setValue( act ); }

//     void
//     Action::TypeExtractor::visit( ActHome* act )
//     { setValue( act ); }

//     void
//     Action::TypeExtractor::visit( ActBallToReg* act )
//     { setValue( act ); }

//     void
//     Action::TypeExtractor::visit( ActBallToPlayer* act )
//     { setValue( act ); }

//     void
//     Action::TypeExtractor::visit( ActMark* act )
//     { setValue( act ); }

//     void
//     Action::TypeExtractor::visit( ActMarkLinePlayer* act )
//     { setValue( act ); }

//     void
//     Action::TypeExtractor::visit( ActMarkLineReg* act )
//     { setValue( act ); }

//     void
//     Action::TypeExtractor::visit( ActOffsidesLine* act )
//     { setValue( act ); }

//     void
//     Action::TypeExtractor::visit( ActHetType* act )
//     { setValue( act ); }

//     void
//     Action::TypeExtractor::visit( ActNamed* act )
//     { setValue( act ); }

//     void
//     Action::TypeExtractor::visit( ActPassReg* act )
//     { setValue( act ); }

//     void
//     Action::TypeExtractor::visit( ActPassUNum* act )
//     { setValue( act ); }

//     void
//     Action::TypeExtractor::visit( ActDribble* act )
//     { setValue( act ); }

//     void
//     Action::TypeExtractor::visit( ActClear* act )
//     { setValue( act ); }

//     void
//     Action::TypeExtractor::visit( ActShoot* act )
//     { setValue( act ); }

//     void
//     Action::TypeExtractor::visit( ActHold* act )
//     { setValue( act ); }

//     void
//     Action::TypeExtractor::visit( ActIntercept* act )
//     { setValue( act ); }

//     void
//     Action::TypeExtractor::visit( ActTackle* act )
//     { setValue( act ); }

//     void
//     Action::ConstTypeExtractor::visit( const ActPos* act )
//     { setValue( act ); }

//     void
//     Action::ConstTypeExtractor::visit( const ActHome* act )
//     { setValue( act ); }

//     void
//     Action::ConstTypeExtractor::visit( const ActBallToReg* act )
//     { setValue( act ); }

//     void
//     Action::ConstTypeExtractor::visit( const ActBallToPlayer* act )
//     { setValue( act ); }

//     void
//     Action::ConstTypeExtractor::visit( const ActMark* act )
//     { setValue( act ); }

//     void
//     Action::ConstTypeExtractor::visit( const ActMarkLinePlayer* act )
//     { setValue( act ); }

//     void
//     Action::ConstTypeExtractor::visit( const ActMarkLineReg* act )
//     { setValue( act ); }

//     void
//     Action::ConstTypeExtractor::visit( const ActOffsidesLine* act )
//     { setValue( act ); }

//     void
//     Action::ConstTypeExtractor::visit( const ActHetType* act )
//     { setValue( act ); }

//     void
//     Action::ConstTypeExtractor::visit( const ActNamed* act )
//     { setValue( act ); }

//     void
//     Action::ConstTypeExtractor::visit( const ActPassReg* act )
//     { setValue( act ); }

//     void
//     Action::ConstTypeExtractor::visit( const ActPassUNum* act )
//     { setValue( act ); }

//     void
//     Action::ConstTypeExtractor::visit( const ActDribble* act )
//     { setValue( act ); }

//     void
//     Action::ConstTypeExtractor::visit( const ActClear* act )
//     { setValue( act ); }

//     void
//     Action::ConstTypeExtractor::visit( const ActShoot* act )
//     { setValue( act ); }

//     void
//     Action::ConstTypeExtractor::visit( const ActHold* act )
//     { setValue( act ); }

//     void
//     Action::ConstTypeExtractor::visit( const ActIntercept* act )
//     { setValue( act ); }

//     void
//     Action::ConstTypeExtractor::visit( const ActTackle* act )
//     { setValue( act ); }

Action::~Action()
{

}

ActPos::ActPos()
    : Action()
{

}

ActPos::ActPos( std::shared_ptr< Region > r )
	: Action(),
      m_reg( r )
{

}

ActPos::~ActPos()
{

}

std::shared_ptr< Action >
ActPos::deepCopy() const
{
	std::shared_ptr< Region > new_reg;
    if ( m_reg )
    {
	    new_reg = m_reg->deepCopy();
    }

	std::shared_ptr< Action > rval( new ActPos( new_reg ) );
    return rval;
}

//     void
//     ActPos::accept( Visitor& v )
//     { v.startVisit( this ); }

//     void
//     ActPos::accept( ConstVisitor& v ) const
//     { v.startVisit( this ); }

std::ostream &
ActPos::print( std::ostream & out ) const
{
    out << "(pos ";
    if ( ! m_reg )
    {
        out << "(null)";
    }
    else
    {
        out << *m_reg;
    }
    return out << ")";
}

std::ostream &
ActPos::printPretty( std::ostream & out,
                     const std::string & line_header ) const
{
    out << line_header << "position self at: " << std::endl;
    if ( ! m_reg )
    {
        out << line_header << " (null)\n";
    }
    else
    {
        m_reg->printPretty( out, line_header + " " );
    }
    return out;
}

const
Region *
ActPos::getRegion() const
{
    return m_reg.get();
}


ActHome::ActHome()
    : Action()
{

}

ActHome::ActHome( std::shared_ptr< Region > r )
    : Action(),
      m_reg( r )
{

}

ActHome::~ActHome()
{

}

std::shared_ptr< Action >
ActHome::deepCopy() const
{
	std::shared_ptr< Region > new_reg;
    if ( m_reg )
    {
	    new_reg = m_reg->deepCopy();
    }

	std::shared_ptr< Action > rval( new ActHome( new_reg ) );
    return rval;
}

//     void
//     ActHome::accept( Visitor& v )
//     { v.startVisit( this ); }

//     void
//     ActHome::accept( ConstVisitor& v ) const
//     { v.startVisit( this ); }

std::ostream &
ActHome::print( std::ostream & out ) const
{
    out << "(home ";
    if ( ! m_reg )
    {
        out << "(null)";
    }
    else
    {
        out << *m_reg;
    }
    return out << ")";
}

std::ostream &
ActHome::printPretty( std::ostream & out,
                      const std::string & line_header ) const
{
    out << line_header << "home position at: " << std::endl;
    if ( ! m_reg )
    {
        out << line_header << " (null)\n";
    }
    else
    {
        m_reg->printPretty( out, line_header + " " );
    }
    return out;
}

const
Region *
ActHome::getRegion() const
{
    return m_reg.get();
}


ActBallToReg::ActBallToReg()
    : Action()
{

}

ActBallToReg::ActBallToReg( std::shared_ptr< Region > reg,
                            const BallMove & bmtset )
    : Action(),
      m_reg( reg ),
      m_bmtset( bmtset )
{

}

ActBallToReg::~ActBallToReg()
{

}

std::shared_ptr< Action >
ActBallToReg::deepCopy() const
{
	std::shared_ptr< Region > new_reg;
    if ( m_reg )
    {
	    new_reg = m_reg->deepCopy();
    }

	std::shared_ptr< Action > rval( new ActBallToReg( new_reg, m_bmtset ) );
    return rval;
}

//     void
//     ActBallToReg::accept( Visitor& v )
//     { v.startVisit( this ); }

//     void
//     ActBallToReg::accept( ConstVisitor& v ) const
//     { v.startVisit( this ); }

std::ostream &
ActBallToReg::print( std::ostream & out ) const
{
    out << "(bto ";
    if ( ! m_reg )
    {
        out << "(null)";
    }
    else
    {
        out << *m_reg;
    }
    return out << " " << m_bmtset << ")";
}

std::ostream &
ActBallToReg::printPretty( std::ostream & out,
                           const std::string & line_header ) const
{
    out << line_header << "ball to: use "
        << m_bmtset << " to go to:" << std::endl;
    if ( ! m_reg )
    {
        out << line_header << " (null)\n";
    }
    else
    {
        m_reg->printPretty( out, line_header + " " );
    }
    return out;
}

const
Region *
ActBallToReg::getRegion() const
{
    return m_reg.get();
}

BallMove
ActBallToReg::getBallMove() const
{
    return m_bmtset;
}

void
ActBallToReg::addBMT( BallMoveToken t )
{
    m_bmtset.addToken( t );
}

void
ActBallToReg::removeBMT( BallMoveToken t )
{
    m_bmtset.removeToken( t );
}

void
ActBallToReg::clearBMTSet()
{
    m_bmtset.clear();
}


ActBallToPlayer::ActBallToPlayer()
    : Action()
{

}

ActBallToPlayer::ActBallToPlayer( const UNumSet& players )
    : Action(),
      M_players( players )
{

}

ActBallToPlayer::~ActBallToPlayer()
{

}

std::shared_ptr< Action >
ActBallToPlayer::deepCopy() const
{
    std::shared_ptr< Action > rval( new ActBallToPlayer( *this ) );
    return rval;
}

//     void
//     ActBallToPlayer::accept( Visitor& v )
//     { v.startVisit( this ); }

//     void
//     ActBallToPlayer::accept( ConstVisitor& v ) const
//     { v.startVisit( this ); }

std::ostream &
ActBallToPlayer::print( std::ostream & out ) const
{
    return out << "(bto " << M_players << ")";
}

std::ostream &
ActBallToPlayer::printPretty( std::ostream & out,
                              const std::string & line_header) const
{
    return out << line_header << "ball to players: " << M_players << std::endl;
}

UNumSet &
ActBallToPlayer::getPlayers()
{
    return M_players;
}

const UNumSet &
ActBallToPlayer::getPlayers() const
{
    return M_players;
}

void
ActBallToPlayer::setPlayers( const UNumSet & players )
{
    M_players = players;
}

void
ActBallToPlayer::addPlayer( const UNum & u )
{
    M_players.add( u );
}

void
ActBallToPlayer::clearPlayers()
{
    M_players.clear();
}


ActMark::ActMark()
    : Action()
{

}

ActMark::ActMark( const UNumSet & players )
    : Action(),
      M_players( players )
{

}

ActMark::~ActMark()
{

}

std::shared_ptr< Action >
ActMark::deepCopy() const
{
    std::shared_ptr< Action > rval( new ActMark( *this ) );
    return rval;
}

//     void
//     ActMark::accept( Visitor& v )
//     { v.startVisit( this ); }

//     void
//     ActMark::accept( ConstVisitor& v ) const
//     { v.startVisit( this ); }

std::ostream &
ActMark::print( std::ostream & out ) const
{
    return out << "(mark " << M_players << ")";
}

std::ostream &
ActMark::printPretty( std::ostream & out,
                      const std::string & line_header ) const
{
    return out << line_header << "mark opponent " << M_players
               << std::endl;
}

UNumSet &
ActMark::getPlayers()
{
    return M_players;
}

const
UNumSet &
ActMark::getPlayers() const
{
    return M_players;
}

void
ActMark::setPlayers( const UNumSet & players )
{
    M_players = players;
}

void
ActMark::addPlayer( const UNum & i )
{
    M_players.add(i);
}

void
ActMark::clearPlayers()
{
    M_players.clear();
}


ActMarkLinePlayer::ActMarkLinePlayer()
    : Action()
{

}

ActMarkLinePlayer::ActMarkLinePlayer( const UNumSet & players )
    : Action(),
      M_players( players )
{

}

ActMarkLinePlayer::~ActMarkLinePlayer()
{

}

std::shared_ptr< Action >
ActMarkLinePlayer::deepCopy() const
{
    std::shared_ptr< Action > rval( new ActMarkLinePlayer( *this ) );
    return rval;
}

//     void
//     ActMarkLinePlayer::accept( Visitor& v )
//     { v.startVisit( this ); }

//     void
//     ActMarkLinePlayer::accept( ConstVisitor& v ) const
//     { v.startVisit( this ); }

std::ostream &
ActMarkLinePlayer::print( std::ostream & out ) const
{
    return out << "(markl " << M_players << ")";
}

std::ostream &
ActMarkLinePlayer::printPretty( std::ostream & out,
                                const std::string & line_header ) const
{
    return out << line_header
               << "mark line to opponent "
               << M_players << std::endl;
}

UNumSet &
ActMarkLinePlayer::getPlayers()
{
    return M_players;
}

const
UNumSet &
ActMarkLinePlayer::getPlayers() const
{
    return M_players;
}

void
ActMarkLinePlayer::setPlayers( const UNumSet & players )
{
    M_players = players;
}

void
ActMarkLinePlayer::addPlayer( const UNum & i )
{
    M_players.add( i );
}

void
ActMarkLinePlayer::clearPlayers()
{
    M_players.clear();
}


ActMarkLineReg::ActMarkLineReg()
    : Action()
{

}

ActMarkLineReg::ActMarkLineReg( std::shared_ptr< Region > reg )
    : Action(),
      m_reg( reg )
{

}

ActMarkLineReg::~ActMarkLineReg()
{ }

std::ostream &
ActMarkLineReg::print( std::ostream & out ) const
{
    out << "(markl ";
    if ( ! getRegion() )
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
ActMarkLineReg::printPretty( std::ostream & out,
                             const std::string & line_header ) const
{
    out << line_header << "mark line to region:" << std::endl;
    if ( ! getRegion() )
    {
        out << line_header << " (null)\n";
    }
    else
    {
        getRegion()->printPretty( out, line_header + " " );
    }
    return out;
}

const
Region *
ActMarkLineReg::getRegion() const
{
    return m_reg.get();
}

std::shared_ptr< Region >
ActMarkLineReg::detachRegion()
{
    return m_reg;
}

std::shared_ptr< Action >
ActMarkLineReg::deepCopy() const
{
    std::shared_ptr< Action > rval;
    if ( getRegion() )
    {
        rval = std::shared_ptr< Action >( new ActMarkLineReg( getRegion()->deepCopy() ) );
    }
    else
    {
        rval = std::shared_ptr< Action >( new ActMarkLineReg() );
    }

    return rval;
}


ActOffsidesLine::ActOffsidesLine()
    : Action()
{

}

ActOffsidesLine::ActOffsidesLine( std::shared_ptr< Region > r )
    : Action(),
      m_reg( r )
{

}

ActOffsidesLine::~ActOffsidesLine()
{

}

std::shared_ptr< Action >
ActOffsidesLine::deepCopy() const
{
    std::shared_ptr< Action > rval;
    if ( getRegion() )
    {
        rval = std::shared_ptr< Action >( new ActOffsidesLine( getRegion()->deepCopy() ) );
    }
    else
    {
        rval = std::shared_ptr< Action >( new ActOffsidesLine() );
    }

    return rval;
}

std::ostream &
ActOffsidesLine::print( std::ostream & out ) const
{
    out << "(oline ";
    if ( ! getRegion() )
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
ActOffsidesLine::printPretty( std::ostream & out,
                              const std::string & line_header ) const
{
    out << line_header << "offsides line at: " << std::endl;
    if ( ! getRegion() )
    {
        out << line_header << " (null)\n";
    }
    else
    {
        getRegion()->printPretty( out, line_header + " " );
    }
    return out;
}

const
Region *
ActOffsidesLine::getRegion() const
{
    return m_reg.get();
}

std::shared_ptr< Region >
ActOffsidesLine::detachRegion()
{
    return m_reg;
}


ActHetType::ActHetType()
    : Action(),
      M_player_type( 0 )
{

}

ActHetType::ActHetType( const int & player_type )
    : Action(),
      M_player_type( player_type )
{

}

ActHetType::~ActHetType()
{

}

std::shared_ptr< Action >
ActHetType::deepCopy() const
{
    std::shared_ptr< Action > rval( new ActHetType( *this ) );
    return rval;
}

std::ostream &
ActHetType::print( std::ostream & out ) const
{
    return out << "(htype " << M_player_type << ")";
}

std::ostream &
ActHetType::printPretty( std::ostream & out,
                         const std::string & line_header ) const
{
    return out << line_header << "heterogenous type: "
               << M_player_type << std::endl;
}

int
ActHetType::getPlayerType() const
{
    return M_player_type;
}

void
ActHetType::setPlayerType( const int player_type )
{
    M_player_type = player_type;
}


ActNamed::ActNamed( const std::string & name )
    : Action(),
      M_name( name )
{

}

ActNamed::~ActNamed()
{

}

std::shared_ptr< Action >
ActNamed::deepCopy() const
{
    std::shared_ptr< Action > rval( new ActNamed( *this ) );
    return rval;
}

std::ostream &
ActNamed::print( std::ostream & out ) const
{
    return out << "\"" << M_name << "\"";
}

std::ostream &
ActNamed::printPretty( std::ostream & out,
                       const std::string & line_header ) const
{
    return out << line_header << "action named \""
               << M_name << "\"" << std::endl;
}

std::string &
ActNamed::getName()
{
    return M_name;
}

const
std::string &
ActNamed::getName() const
{
    return M_name;
}

void
ActNamed::setName( const std::string & name )
{
    M_name = name;
}


ActPassReg::ActPassReg()
    : Action()
{

}

ActPassReg::ActPassReg( std::shared_ptr< Region > reg )
    : Action(),
      m_reg( reg )
{

}

ActPassReg::~ActPassReg()
{

}

std::shared_ptr< Action >
ActPassReg::deepCopy() const
{
    std::shared_ptr< Action > rval;
    if ( getRegion() )
    {
        rval = std::shared_ptr< Action >( new ActPassReg( getRegion()->deepCopy() ) );
    }
    else
    {
        rval = std::shared_ptr< Action >( new ActPassReg() );
    }

    return rval;
}

std::ostream &
ActPassReg::print( std::ostream & out ) const
{
    out << "(pass ";
    if ( ! getRegion() )
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
ActPassReg::printPretty( std::ostream & out,
                         const std::string & line_header ) const
{
    out << line_header << "pass to region:" << std::endl;
    if ( ! getRegion() )
    {
        out << line_header << " (null)\n";
    }
    else
    {
        getRegion()->printPretty( out, line_header + " " );
    }
    return out;
}

const
Region *
ActPassReg::getRegion() const
{
    return m_reg.get();
}

std::shared_ptr< Region >
ActPassReg::detachRegion()
{
    return m_reg;
}


ActPassUNum::ActPassUNum()
    : Action()
{

}

ActPassUNum::ActPassUNum( const UNumSet & players )
    : Action(),
      m_players( players )
{

}

ActPassUNum::~ActPassUNum()
{

}

std::shared_ptr< Action >
ActPassUNum::deepCopy() const
{
    std::shared_ptr< Action > rval( new ActPassUNum( *this ) );
    return rval;
}

std::ostream &
ActPassUNum::print( std::ostream & out ) const
{
    return out << "(pass " << getPlayers() << ")";
}

std::ostream &
ActPassUNum::printPretty( std::ostream & out,
                          const std::string & line_header ) const
{
    return out << line_header
               << "pass to player(s) "
               << getPlayers() << std::endl;
}

UNumSet &
ActPassUNum::getPlayers()
{
    return m_players;
}

const
UNumSet &
ActPassUNum::getPlayers() const
{
    return m_players;
}

void
ActPassUNum::add( const UNum & unum )
{
    m_players.add( unum );
}

void
ActPassUNum::clearPlayers()
{
    m_players.clear();
}


ActDribble::ActDribble()
    : Action()
{

}

ActDribble::ActDribble( std::shared_ptr< Region > reg )
    : Action(),
      m_reg( reg )
{

}

ActDribble::~ActDribble()
{

}

std::shared_ptr< Action >
ActDribble::deepCopy() const
{
    std::shared_ptr< Action > rval;
    if ( getRegion() )
    {
        rval = std::shared_ptr< Action >( new ActDribble( getRegion()->deepCopy() ) );
    }
    else
    {
        rval = std::shared_ptr< Action >( new ActDribble() );
    }

    return rval;
}

std::ostream &
ActDribble::print( std::ostream & out ) const
{
    out << "(dribble ";
    if ( ! getRegion() )
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
ActDribble::printPretty( std::ostream & out,
                         const std::string & line_header ) const
{
    out << line_header << "dribble to region:" << std::endl;
    if ( ! getRegion() )
    {
        out << line_header << " (null)\n";
    }
    else
    {
        getRegion()->printPretty( out, line_header + " " );
    }
    return out;
}

const
Region *
ActDribble::getRegion() const
{
    return m_reg.get();
}

std::shared_ptr< Region >
ActDribble::detachRegion()
{
    return m_reg;
}


ActClear::ActClear()
    : Action()
{

}

ActClear::ActClear( std::shared_ptr< Region > reg )
    : Action(),
      m_reg( reg )
{

}

ActClear::~ActClear()
{

}

std::shared_ptr< Action >
ActClear::deepCopy() const
{
    std::shared_ptr< Action > rval;
    if ( getRegion() )
    {
        rval = std::shared_ptr< Action >( new ActClear( getRegion()->deepCopy() ) );
    }
    else
    {
        rval = std::shared_ptr< Action >( new ActClear() );
    }

    return rval;
}

std::ostream &
ActClear::print( std::ostream & out ) const
{
    out << "(clear ";
    if ( ! getRegion() )
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
ActClear::printPretty( std::ostream & out,
                       const std::string & line_header ) const
{
    out << line_header << "clear to region:" << std::endl;
    if ( ! getRegion() )
    {
        out << line_header << " (null)\n";
    }
    else
    {
        getRegion()->printPretty( out, line_header + " " );
    }
    return out;
}

const
Region *
ActClear::getRegion() const
{
    return m_reg.get();
}

std::shared_ptr< Region >
ActClear::detachRegion()
{
    return m_reg;
}


ActShoot::ActShoot()
    : Action()
{

}

ActShoot::~ActShoot()
{

}

std::shared_ptr< Action >
ActShoot::deepCopy() const
{
    std::shared_ptr< Action > rval( new ActShoot( *this ) );
    return rval;
}

std::ostream &
ActShoot::print( std::ostream & out ) const
{
    return out << "(shoot)";
}

std::ostream &
ActShoot::printPretty( std::ostream & out,
                       const std::string & line_header ) const
{
    return out << line_header << "shoot" << std::endl;
}


ActHold::ActHold()
    : Action()
{

}

ActHold::~ActHold()
{

}

std::shared_ptr< Action >
ActHold::deepCopy() const
{
    std::shared_ptr< Action > rval( new ActHold( *this ) );
    return rval;
}

std::ostream &
ActHold::print( std::ostream & out ) const
{
    return out << "(hold)";
}

std::ostream &
ActHold::printPretty( std::ostream & out,
                      const std::string & line_header ) const
{
    return out << line_header << "hold" << std::endl;
}


ActIntercept::ActIntercept()
    : Action()
{

}

ActIntercept::~ActIntercept()
{

}

std::shared_ptr< Action >
ActIntercept::deepCopy() const
{
    std::shared_ptr< Action > rval( new ActIntercept( *this ) );
    return rval;
}

std::ostream &
ActIntercept::print( std::ostream & out ) const
{
    return out << "(intercept)";
}

std::ostream &
ActIntercept::printPretty( std::ostream & out,
                           const std::string & line_header ) const
{
    return out << line_header << "intercept" << std::endl;
}


ActTackle::ActTackle()
    : Action()
{

}

ActTackle::ActTackle( const UNumSet & players )
    : Action(),
      M_players( players )
{

}

ActTackle::~ActTackle()
{

}

std::shared_ptr< Action >
ActTackle::deepCopy() const
{
    std::shared_ptr< Action > rval( new ActTackle( *this ) );
    return rval;
}

std::ostream &
ActTackle::print( std::ostream & out ) const
{
    return out << "(tackle " << M_players << ")";
}

std::ostream &
ActTackle::printPretty( std::ostream & out,
                        const std::string & line_header ) const
{
    return out << line_header
               << "tackle player(s) "
               << M_players << std::endl;
}

UNumSet &
ActTackle::getPlayers()
{
    return M_players;
}

const
UNumSet &
ActTackle::getPlayers() const
{
    return M_players;
}

void
ActTackle::setPlayers( const UNumSet & players )
{
    M_players = players;
}

void
ActTackle::addPlayer( const UNum & i )
{
    M_players.add( i );
}

void
ActTackle::clearPlayers()
{
    M_players.clear();
}




std::ostream &
operator<<( std::ostream & os,
            const Action & a )
{
    return a.print( os );
}

}
}
