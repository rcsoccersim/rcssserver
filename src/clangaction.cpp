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
 *   Foundation; either version 2 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#include "clangaction.h"

namespace rcss
{
  namespace clang
  {
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
    {}


    
    ActPos::ActPos() 
      : Action()
    {}
    
    ActPos::ActPos( std::auto_ptr< Region > r )
	: Action(),
	  m_reg( r )
    {}
    
    ActPos::~ActPos() 
    {}
    
    std::auto_ptr< Action >
    ActPos::deepCopy() const
    {
	std::auto_ptr< Region > new_reg;
	if( m_reg.get() != NULL )
	    new_reg = m_reg->deepCopy();
	return std::auto_ptr< Action >( new ActPos( new_reg ) ); 
    }
    
//     void
//     ActPos::accept( Visitor& v )
//     { v.startVisit( this ); }
    
//     void
//     ActPos::accept( ConstVisitor& v ) const
//     { v.startVisit( this ); }
    
    std::ostream&
    ActPos::print( std::ostream& out ) const
    { 
      out << "(pos ";
      if( m_reg.get() == NULL )
        out << "(null)";
      else
        out << *m_reg;
      return out << ")";
    }
    
    std::ostream&
    ActPos::printPretty( std::ostream& out, 
                         const std::string& line_header ) const
    { 
      out << line_header << "position self at: " << std::endl;
      if( m_reg.get() == NULL )
        out << line_header << " (null)\n";
      else
        m_reg->printPretty( out, line_header + " " );
      return out;
    }
    
    const Region* 
    ActPos::getRegion() const 
    { return m_reg.get(); }

    
    ActHome::ActHome() 
      : Action()
    {}
    
    ActHome::ActHome( std::auto_ptr< Region > r ) 
      : Action(),
        m_reg( r )
    {}
    
    ActHome::~ActHome()
    {}
    
    std::auto_ptr< Action >
    ActHome::deepCopy() const
    { 
	std::auto_ptr< Region > new_reg;
	if( m_reg.get() != NULL )
	    new_reg = m_reg->deepCopy();
	return std::auto_ptr< Action >( new ActHome( new_reg ) ); 
    }
    
//     void
//     ActHome::accept( Visitor& v )
//     { v.startVisit( this ); }
    
//     void
//     ActHome::accept( ConstVisitor& v ) const
//     { v.startVisit( this ); }
    
    std::ostream&
    ActHome::print( std::ostream& out ) const
    { 
      out << "(home ";
      if( m_reg.get() == NULL )
        out << "(null)";
      else
        out << *m_reg;
      return out << ")";
    }
    
    std::ostream&
    ActHome::printPretty( std::ostream& out,
                          const std::string& line_header ) const
    { 
      out << line_header << "home position at: " << std::endl;
      if( m_reg.get() == NULL )
        out << line_header << " (null)\n";
      else
        m_reg->printPretty( out, line_header + " " ); 
      return out;
    }
    
    const Region*
    ActHome::getRegion() const
    { return m_reg.get(); }

    
    ActBallToReg::ActBallToReg() 
      : Action()
    {}

    ActBallToReg::ActBallToReg( std::auto_ptr< Region > reg, 
                                const BallMove& bmtset ) 
      : Action(),
        m_reg( reg ),
        m_bmtset( bmtset ) 
    {}
    
    ActBallToReg::~ActBallToReg()
    {}
    
    std::auto_ptr< Action >
    ActBallToReg::deepCopy() const
    { 
	std::auto_ptr< Region > new_reg;
	if( m_reg.get() != NULL )
	    new_reg = m_reg->deepCopy();	
	return std::auto_ptr< Action >( new ActBallToReg( new_reg, m_bmtset ) ); 
    }
    
//     void
//     ActBallToReg::accept( Visitor& v )
//     { v.startVisit( this ); }
    
//     void
//     ActBallToReg::accept( ConstVisitor& v ) const
//     { v.startVisit( this ); }
    
    std::ostream&
    ActBallToReg::print( std::ostream& out ) const 
    {
      out << "(bto ";
      if( m_reg.get() == NULL )
        out << "(null)";
      else
        out << *m_reg;
      return out << " " << m_bmtset << ")";
    }
    
    std::ostream&
    ActBallToReg::printPretty( std::ostream& out,
                               const std::string& line_header ) const
    {
      out << line_header << "ball to: use " 
          << m_bmtset << " to go to:" << std::endl;
      if( m_reg.get() == NULL )
        out << line_header << " (null)\n";
      else
        m_reg->printPretty( out, line_header + " " );
      return out;
    }
    
    const Region*
    ActBallToReg::getRegion() const
    { return m_reg.get(); }
    
    BallMove
    ActBallToReg::getBallMove() const
    { return m_bmtset; }
    
    void
    ActBallToReg::addBMT( BallMoveToken t )
    { m_bmtset.addToken( t ); }
    
    void
    ActBallToReg::removeBMT( BallMoveToken t )
    { m_bmtset.removeToken( t ); }
    
    void
    ActBallToReg::clearBMTSet()
    { m_bmtset.clear(); }


    ActBallToPlayer::ActBallToPlayer() 
      : Action()
    {}
  
    ActBallToPlayer::ActBallToPlayer( const UNumSet& players )
      : Action(),
        M_players( players )
    {}
 
    ActBallToPlayer::~ActBallToPlayer()
    {}

    std::auto_ptr< Action >
    ActBallToPlayer::deepCopy() const
    { return std::auto_ptr< Action >( new ActBallToPlayer( *this ) ); }

//     void
//     ActBallToPlayer::accept( Visitor& v )
//     { v.startVisit( this ); }
    
//     void
//     ActBallToPlayer::accept( ConstVisitor& v ) const
//     { v.startVisit( this ); }
    
    std::ostream&
    ActBallToPlayer::print( std::ostream& out ) const
    { return out << "(bto " << M_players << ")"; }
    
    std::ostream&
    ActBallToPlayer::printPretty( std::ostream& out, 
                                  const std::string& line_header) const
    { 
      return out << line_header << "ball to players: " << M_players << std::endl;
    }
    
    UNumSet&
    ActBallToPlayer::getPlayers() 
    { return M_players; }
    
    const UNumSet&
    ActBallToPlayer::getPlayers() const 
    { return M_players; }
    
    void 
    ActBallToPlayer::setPlayers( const UNumSet& players )
    { M_players = players; }
    
    void 
    ActBallToPlayer::addPlayer( const UNum& u )
    { M_players.add( u ); }
    
    void
    ActBallToPlayer::clearPlayers() 
    { M_players.clear(); } 


    ActMark::ActMark() 
      : Action()
    {}
  
    ActMark::ActMark( const UNumSet& players ) 
      : Action(),
        M_players( players )
    {}

    ActMark::~ActMark()
    {}

    std::auto_ptr< Action >
    ActMark::deepCopy() const
    { return std::auto_ptr< Action >( new ActMark( *this ) ); }

//     void
//     ActMark::accept( Visitor& v )
//     { v.startVisit( this ); }

//     void
//     ActMark::accept( ConstVisitor& v ) const
//     { v.startVisit( this ); }

    std::ostream&
    ActMark::print( std::ostream& out ) const
    { return out << "(mark " << M_players << ")"; }

    std::ostream&
    ActMark::printPretty( std::ostream& out, const std::string& line_header ) const
    { return out << line_header << "mark opponent " << M_players << std::endl; }

    UNumSet& 
    ActMark::getPlayers() 
    { return M_players; }

    const UNumSet& 
    ActMark::getPlayers() const
    { return M_players; }

    void 
    ActMark::setPlayers( const UNumSet& players )
    { M_players = players; }

    void
    ActMark::addPlayer( const UNum& i )
    { M_players.add(i); }

    void
    ActMark::clearPlayers()
    { M_players.clear(); }


    ActMarkLinePlayer::ActMarkLinePlayer() 
      : Action()
    {}
  
    ActMarkLinePlayer::ActMarkLinePlayer( const UNumSet& players ) 
      : Action(),
        M_players( players )
    {}

    ActMarkLinePlayer::~ActMarkLinePlayer()
    {}

    std::auto_ptr< Action >
    ActMarkLinePlayer::deepCopy() const
    { return std::auto_ptr< Action >( new ActMarkLinePlayer( *this ) ); }

//     void
//     ActMarkLinePlayer::accept( Visitor& v )
//     { v.startVisit( this ); }

//     void
//     ActMarkLinePlayer::accept( ConstVisitor& v ) const
//     { v.startVisit( this ); }

    std::ostream&
    ActMarkLinePlayer::print( std::ostream& out ) const
    { return out << "(markl " << M_players << ")"; }

    std::ostream&
    ActMarkLinePlayer::printPretty( std::ostream& out,
                                    const std::string& line_header ) const
    { 
      return out << line_header 
                 << "mark line to opponent " 
                 << M_players << std::endl; 
    }

    UNumSet&
    ActMarkLinePlayer::getPlayers()
    { return M_players; }
  
    const
    UNumSet&
    ActMarkLinePlayer::getPlayers() const 
    { return M_players; }
  
    void 
    ActMarkLinePlayer::setPlayers( const UNumSet& players )
    { M_players = players; }
 
    void
    ActMarkLinePlayer::addPlayer( const UNum& i )
    { M_players.add( i ); }
  
    void
    ActMarkLinePlayer::clearPlayers()
    { M_players.clear(); }

      std::ostream&
      operator<<( std::ostream & os, const Action& a )
      { return a.print( os ); }

  }
}
