// -*-c++-*-

/***************************************************************************
                                clangaction.h  
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

#ifndef CLANGACTION_H
#define CLANGACTION_H

#include <memory>
#include "hasa.h"
#include "region.h"

namespace rcss
{
  namespace clang
  {
    class ActPos;
    class ActHome;
    class ActBallToReg;
    class ActBallToPlayer;
    class ActMark;
    class ActMarkLinePlayer;
    class ActMarkLineReg;
    class ActOffsidesLine;
    class ActHetType;
    class ActNamed;
    class ActPassReg;
    class ActPassUNum;
    class ActDribble;
    class ActClear;
    class ActShoot;
    class ActHold;
    class ActIntercept;
    class ActTackle;
 
    class Action 
    {
    public:
//       typedef util::Visitor20< ActPos*,
//         ActHome*,
//         ActBallToReg*,
//         ActBallToPlayer*,
//         ActMark*,
//         ActMarkLinePlayer*,
//         ActMarkLineReg*,
//         ActOffsidesLine*,
//         ActHetType*,
//         ActNamed*,
//         ActPassReg*,
//         ActPassUNum*,
//         ActDribble*,
//         ActClear*,
//         ActShoot*,
//         ActHold*,
//         ActIntercept*,
//         ActTackle* > Visitor;

//       typedef util::Visitor20< const ActPos*,
//         const ActHome*,
//         const ActBallToReg*,
//         const ActBallToPlayer*,
//         const ActMark*,
//         const ActMarkLinePlayer*,
//         const ActMarkLineReg*,
//         const ActOffsidesLine*,
//         const ActHetType*,
//         const ActNamed*,
//         const ActPassReg*,
//         const ActPassUNum*,
//         const ActDribble*,
//         const ActClear*,
//         const ActShoot*,
//         const ActHold*,
//         const ActIntercept*,
//         const ActTackle* > ConstVisitor;

//       class TypeExtractor
//         : public util::TypeExtractor20< ActPos*,
//         ActHome*,
//         ActBallToReg*,
//         ActBallToPlayer*,
//         ActMark*,
//         ActMarkLinePlayer*,
//         ActMarkLineReg*,
//         ActOffsidesLine*,
//         ActHetType*,
//         ActNamed*,
//         ActPassReg*,
//         ActPassUNum*,
//         ActDribble*,
//         ActClear*,
//         ActShoot*,
//         ActHold*,
//         ActIntercept*,
//         ActTackle* >
//       {
//       protected:
//           virtual
//           ~TypeExtractor()
//           {}

//         virtual
//         void
//         visit( ActPos* act );

//         virtual
//         void
//         visit( ActHome* act );

//         virtual
//         void
//         visit( ActBallToReg* act );

//         virtual
//         void
//         visit( ActBallToPlayer* act );

//         virtual
//         void
//         visit( ActMark* act );

//         virtual
//         void
//         visit( ActMarkLinePlayer* act );

//         virtual
//         void
//         visit( ActMarkLineReg* act );

//         virtual
//         void
//         visit( ActOffsidesLine* act );

//         virtual
//         void
//         visit( ActHetType* act );

//         virtual
//         void
//         visit( ActNamed* act );

//         virtual
//         void
//         visit( ActPassReg* act );

//         virtual
//         void
//         visit( ActPassUNum* act );

//         virtual
//         void
//         visit( ActDribble* act );

//         virtual
//         void
//         visit( ActClear* act );

//         virtual
//         void
//         visit( ActShoot* act );

//         virtual
//         void
//         visit( ActHold* act );

//         virtual
//         void
//         visit( ActIntercept* act );

//         virtual
//         void
//         visit( ActTackle* act );
//       };

//       class ConstTypeExtractor
//         : public util::TypeExtractor20< const ActPos*,
//         const ActHome*,
//         const ActBallToReg*,
//         const ActBallToPlayer*,
//         const ActMark*,
//         const ActMarkLinePlayer*,
//         const ActMarkLineReg*,
//         const ActOffsidesLine*,
//         const ActHetType*,
//         const ActNamed*,
//         const ActPassReg*,
//         const ActPassUNum*,
//         const ActDribble*,
//         const ActClear*,
//         const ActShoot*,
//         const ActHold*,
//         const ActIntercept*,
//         const ActTackle* >
//       {
//       protected:
//           virtual
//           ~ConstTypeExtractor()
//           {}

//         virtual
//         void
//         visit( const ActPos* act );

//         virtual
//         void
//         visit( const ActHome* act );

//         virtual
//         void
//         visit( const ActBallToReg* act );

//         virtual
//         void
//         visit( const ActBallToPlayer* act );

//         virtual
//         void
//         visit( const ActMark* act );

//         virtual
//         void
//         visit( const ActMarkLinePlayer* act );

//         virtual
//         void
//         visit( const ActMarkLineReg* act );

//         virtual
//         void
//         visit( const ActOffsidesLine* act );

//         virtual
//         void
//         visit( const ActHetType* act );

//         virtual
//         void
//         visit( const ActNamed* act );

//         virtual
//         void
//         visit( const ActPassReg* act );

//         virtual
//         void
//         visit( const ActPassUNum* act );

//         virtual
//         void
//         visit( const ActDribble* act );

//         virtual
//         void
//         visit( const ActClear* act );

//         virtual
//         void
//         visit( const ActShoot* act );

//         virtual
//         void
//         visit( const ActHold* act );

//         virtual
//         void
//         visit( const ActIntercept* act );

//         virtual
//         void
//         visit( const ActTackle* act );
//       };

      virtual 
      ~Action();

      virtual
      std::auto_ptr< Action >
      deepCopy() const = 0;

//       virtual
//       void
//       accept( Visitor& v ) = 0;

//       virtual
//       void
//       accept( ConstVisitor& v ) const = 0;

      virtual 
      std::ostream& 
      print( std::ostream& out ) const = 0;

      virtual
      std::ostream& 
      printPretty( std::ostream& out, const std::string& line_header ) const = 0;
    }; // end Action


    class ActPos
      : public Action
    {
    public:

      ActPos();

      ActPos( std::auto_ptr< Region > r );

      virtual
      ~ActPos();

      virtual
      std::auto_ptr< Action >
      deepCopy() const;

//       virtual
//       void
//       accept( Visitor& v );

//       virtual
//       void
//       accept( ConstVisitor& v ) const;

      virtual
      std::ostream&
      print( std::ostream& out ) const;

      virtual
      std::ostream&
      printPretty( std::ostream& out, 
                   const std::string& line_header ) const;
   
      const Region* 
      getRegion() const;

    private:
	std::auto_ptr< Region > m_reg;
    };


    class ActHome
      : public Action
    {
    public:
	
      ActHome();

      ActHome( std::auto_ptr< Region > r );

      virtual
      ~ActHome();

      virtual
      std::auto_ptr< Action >
      deepCopy() const;

//       virtual
//       void
//       accept( Visitor& v );

//       virtual
//       void
//       accept( ConstVisitor& v ) const;

      virtual
      std::ostream&
      print( std::ostream& out ) const;

      virtual
      std::ostream&
      printPretty( std::ostream& out,
                   const std::string& line_header ) const;
  
      const Region*
      getRegion() const;

    private:
	std::auto_ptr< Region > m_reg;
    };


    class ActBallToReg
      : public Action
    {
    
    public:
      ActBallToReg();

      ActBallToReg( std::auto_ptr< Region > reg, 
                    const BallMove& bmtset );

      virtual
      ~ActBallToReg();

      virtual
      std::auto_ptr< Action >
      deepCopy() const;

//       virtual
//       void
//       accept( Visitor& v );

//       virtual
//       void
//       accept( ConstVisitor& v ) const;

      virtual
      std::ostream&
      print( std::ostream& out ) const;
  
      virtual
      std::ostream&
      printPretty( std::ostream& out,
                   const std::string& line_header ) const;

      const Region*
      getRegion() const;
  
      BallMove
      getBallMove() const;

      void
      addBMT( BallMoveToken t );
  
      void
      removeBMT( BallMoveToken t );

      void
      clearBMTSet();

    private:
	std::auto_ptr< Region > m_reg; 
	BallMove m_bmtset;
    };

    class ActBallToPlayer
      : public Action
    {
    public:
      ActBallToPlayer();
  
      ActBallToPlayer( const UNumSet& players );
 
      virtual
      ~ActBallToPlayer();

      virtual
      std::auto_ptr< Action >
      deepCopy() const;

//       virtual
//       void
//       accept( Visitor& v );

//       virtual
//       void
//       accept( ConstVisitor& v ) const;

      virtual
      std::ostream&
      print( std::ostream& out ) const;
  
      virtual
      std::ostream&
      printPretty( std::ostream& out, 
                   const std::string& line_header) const;
  
      UNumSet&
      getPlayers();

      const UNumSet&
      getPlayers() const;

      void 
      setPlayers( const UNumSet& players );

      void 
      addPlayer( const UNum& u );

      void
      clearPlayers();
    private:
      UNumSet M_players;
    } ;


    class ActMark
      : public Action
    {
    public:
      ActMark();
  
      ActMark( const UNumSet& players );

      virtual
      ~ActMark();

      virtual
      std::auto_ptr< Action >
      deepCopy() const;

//       virtual
//       void
//       accept( Visitor& v );

//       virtual
//       void
//       accept( ConstVisitor& v ) const;

      virtual
      std::ostream&
      print( std::ostream& out ) const;

      virtual
      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const;

      UNumSet& 
      getPlayers();

      const UNumSet& 
      getPlayers() const;

      void 
      setPlayers( const UNumSet& players );

      void
      addPlayer( const UNum& i );

      void
      clearPlayers();

    private:
      UNumSet M_players;
    } ;

    class ActMarkLinePlayer
      : public Action
    {
    public:
      ActMarkLinePlayer();
  
      ActMarkLinePlayer( const UNumSet& players );

      virtual
      ~ActMarkLinePlayer();

      virtual
      std::auto_ptr< Action >
      deepCopy() const;

//       virtual
//       void
//       accept( Visitor& v );

//       virtual
//       void
//       accept( ConstVisitor& v ) const;

      virtual
      std::ostream&
      print( std::ostream& out ) const;

      virtual
      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const;

      UNumSet&
      getPlayers();
  
      const
      UNumSet&
      getPlayers() const;
  
      void 
      setPlayers( const UNumSet& players );
 
      void
      addPlayer( const UNum& i );
  
      void
      clearPlayers();

    private:
      UNumSet M_players;
    } ;

    class ActMarkLineReg
      : public Action
    {
    public:

      ActMarkLineReg() 
        : Action()
      {}

      ActMarkLineReg( std::auto_ptr< Region > reg ) 
        : Action(),
          m_reg( reg ) 
      {}

      ~ActMarkLineReg()
      {}

      std::ostream&
      print( std::ostream& out ) const
      { 
        out << "(markl ";
        if( getRegion() == NULL )
          out << "(null)";
        else
          out << *getRegion();
        return out << ")";
      }

      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const
      { 
        out << line_header << "mark line to region:" << std::endl;
        if( getRegion() == NULL )
          out << line_header << " (null)\n";
        else
          getRegion()->printPretty( out, line_header + " " );
        return out;
      }

      const Region* 
      getRegion() const 
      { return m_reg.get(); }
  
      std::auto_ptr< Region >
      detachRegion()
      { return m_reg; }

 //      virtual
//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       virtual
//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }

      virtual
      std::auto_ptr< Action >
      deepCopy() const
      { 
	  if( getRegion() )
	      return std::auto_ptr< Action >( new ActMarkLineReg( getRegion()->deepCopy() ) );
	  else
	      return std::auto_ptr< Action >( new ActMarkLineReg() );
      }
    private:
	std::auto_ptr< Region > m_reg;
    };


    class ActOffsidesLine
      : public Action
    {
    public:

      ActOffsidesLine() 
        : Action()
      {}

      ActOffsidesLine( std::auto_ptr< Region > r ) 
        : Action(),
          m_reg( r )
      {}

      virtual
      ~ActOffsidesLine()
      {}

      std::ostream&
      print( std::ostream& out ) const
      { 
        out << "(oline ";
        if( getRegion() == NULL )
          out << "(null)";
        else
          out << *getRegion();
        return out << ")";
      }

      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const
      {
        out << line_header << "offsides line at: " << std::endl;
        if( getRegion() == NULL )
          out << line_header << " (null)\n";
        else
          getRegion()->printPretty( out, line_header + " " );
        return out;
      }

      const Region*
      getRegion() const
      { return m_reg.get(); }
  
      std::auto_ptr< Region >
      detachRegion()
      { return m_reg; }

//       virtual
//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       virtual
//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }

	virtual
	std::auto_ptr< Action >
	deepCopy() const
	    {
		if( getRegion() )
		    return std::auto_ptr< Action >( new ActOffsidesLine( getRegion()->deepCopy() ) );
		else
		    return std::auto_ptr< Action >( new ActOffsidesLine() );
	    }

    private:
	std::auto_ptr< Region > m_reg;
    };

    class ActHetType
      : public Action
    {
    public:
      ActHetType() 
        : Action(),
          M_player_type ( 0 )
      {}

      ActHetType( const int& player_type )
        : Action(),
          M_player_type( player_type )
      {}

      ~ActHetType() 
      {}

      std::ostream&
      print( std::ostream& out ) const
      { return out << "(htype " << M_player_type << ")"; }
  
      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const
      { return out << line_header << "heterogenous type: " 
                   << M_player_type << std::endl; }
  
      /* class specific stuff */
      int getPlayerType() const
      { return M_player_type; }

      void
      setPlayerType( const int& player_type )
      { M_player_type = player_type; }

//       virtual
//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       virtual
//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }

      std::auto_ptr< Action >
      deepCopy() const
      { return std::auto_ptr< Action >( new ActHetType( *this ) ); }

    private:
      int M_player_type;
    } ;


    class ActNamed
      : public Action
    {
    public:
      ActNamed( const std::string& name = "" ) 
        : Action(),
          M_name( name )
      {}

      ~ActNamed()
      {}

      std::ostream& 
      print( std::ostream& out ) const
      { return out << "\"" << M_name << "\""; }
  
      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const
      { return out << line_header << "action named \"" 
                   << M_name << "\"" << std::endl; }
  
      /* class specific */
      std::string&
      getName()
      { return M_name; }

      const std::string&
      getName() const
      { return M_name; }

      void setName( const std::string& name )
      { M_name = name; }

//       virtual
//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       virtual
//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }

      std::auto_ptr< Action >
      deepCopy() const
      { return std::auto_ptr< Action >( new ActNamed( *this ) ); }

    private:
      std::string M_name;
    } ;


    class ActPassReg
      : public Action
    {
    public:
      ActPassReg() 
        : Action()
      {}

      ActPassReg( std::auto_ptr< Region > reg ) 
        : Action(),
          m_reg( reg ) 
      {}

      ~ActPassReg()
      {}

      std::ostream&
      print( std::ostream& out ) const
      { 
        out << "(pass ";
        if( getRegion() == NULL )
          out << "(null)";
        else
          out << *getRegion();
        return out << ")";
      }

      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const
      { 
        out << line_header << "pass to region:" << std::endl;
        if( getRegion() == NULL )
          out << line_header << " (null)\n";
        else
          getRegion()->printPretty( out, line_header + " " );
        return out;
      }
      
      const Region* 
      getRegion() const 
      { return m_reg.get(); }
   
      std::auto_ptr< Region >
      detachRegion()
      { return m_reg; }

//       virtual
//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       virtual
//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }

      virtual
      std::auto_ptr< Action >
      deepCopy() const
      {
	  if( getRegion() )
	      return std::auto_ptr< Action >( new ActPassReg( getRegion()->deepCopy() ) ); 
	  else
	      return std::auto_ptr< Action >( new ActPassReg() );
      }

    private:
	std::auto_ptr< Region > m_reg;
    };

    class ActPassUNum
      : public Action
    {
    public:
      ActPassUNum() 
        : Action()
      {}
  
      ActPassUNum( const UNumSet& players ) 
        : Action(),
          m_players( players )
      {}

      ~ActPassUNum()
      {}

      std::ostream&
      print( std::ostream& out ) const
      { return out << "(pass " << getPlayers() << ")"; }

      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const
      { 
        return out << line_header 
                   << "pass to player(s) " 
                   << getPlayers() << std::endl; 
      }

      /* class specific stuff */
      UNumSet
      getPlayers()
      { return m_players; }
  
      const
      UNumSet&
      getPlayers() const 
      { return m_players; }
   
      void
      add( const UNum& unum )
      { m_players.add( unum ); }
  
      void
      clearPlayers()
      { m_players.clear(); }

//       virtual
//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       virtual
//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }

      virtual
      std::auto_ptr< Action >
      deepCopy() const
      { return std::auto_ptr< Action >( new ActPassUNum( *this ) ); }
	
    private:
	UNumSet m_players;
    };

    class ActDribble
      : public Action
    {
    public:
      ActDribble() 
        : Action()
      {}

      ActDribble( std::auto_ptr< Region > reg ) 
        : Action(),
          m_reg( reg ) 
      {}

      ~ActDribble()
      {}

      std::ostream&
      print( std::ostream& out ) const
      { 
        out << "(dribble ";
        if( getRegion() == NULL )
          out << "(null)";
        else
          out << *getRegion();
        return out << ")";
      }

      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const
      { 
        out << line_header << "dribble to region:" << std::endl;
        if( getRegion() == NULL )
          out << line_header << " (null)\n";
        else
          getRegion()->printPretty( out, line_header + " " );
        return out;
      }

      const Region* 
      getRegion() const 
      { return m_reg.get(); }
  
      std::auto_ptr< Region >
      detachRegion()
      { return m_reg; }

//       virtual
//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       virtual
//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }

      virtual
      std::auto_ptr< Action >
      deepCopy() const
      {
	  if( getRegion() )
	      return std::auto_ptr< Action >( new ActDribble( getRegion()->deepCopy() ) ); 
	  else
	      return std::auto_ptr< Action >( new ActDribble() ); 
      }

    private:
	std::auto_ptr< Region > m_reg;
    };

    class ActClear
      : public Action
    {
    public:
      ActClear() 
        : Action()
      {}

      ActClear( std::auto_ptr< Region > reg ) 
	  : Action(),
	    m_reg( reg )
      {}

      ~ActClear()
      {}

      std::ostream&
      print( std::ostream& out ) const
      { 
        out << "(clear ";
        if( getRegion() == NULL )
          out << "(null)";
        else
          out << *getRegion();
        return out << ")";
      }

      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const
      { 
        out << line_header << "clear to region:" << std::endl;
        if( getRegion() == NULL )
          out << line_header << " (null)\n";
        else
          getRegion()->printPretty( out, line_header + " " );
        return out;
      }

      const Region* 
      getRegion() const 
      { return m_reg.get(); }
    
      std::auto_ptr< Region >
      detachRegion()
      { return m_reg; }

//       virtual
//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       virtual
//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }

      virtual
      std::auto_ptr< Action >
      deepCopy() const
      {
	  if( getRegion() )
	      return std::auto_ptr< Action >( new ActClear( getRegion()->deepCopy() ) ); 
	  else
	      return std::auto_ptr< Action >( new ActClear() ); 
      }

    private:
	std::auto_ptr< Region > m_reg;
    };

    class ActShoot
      : public Action
    {
    public:
      ActShoot() 
        : Action()
      {}

      ~ActShoot()
      {}

      std::ostream&
      print( std::ostream& out ) const
      { return out << "(shoot)"; }

      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const
      { return out << line_header << "shoot" << std::endl; }

//       virtual
//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       virtual
//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }

      std::auto_ptr< Action >
      deepCopy() const
      { return std::auto_ptr< Action >( new ActShoot( *this ) ); }
    };

    class ActHold
      : public Action
    {
    public:
      ActHold() 
        : Action()
      {}

      ~ActHold()
      {}

      std::ostream&
      print( std::ostream& out ) const
      { return out << "(hold)"; }

      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const
      { return out << line_header << "hold" << std::endl; }

//       virtual
//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       virtual
//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }

      std::auto_ptr< Action >
      deepCopy() const
      { return std::auto_ptr< Action >( new ActHold( *this ) ); }
    };

    class ActIntercept
      : public Action
    {
    public:
      ActIntercept() 
        : Action()
      {}

      ~ActIntercept()
      {}

      std::ostream&
      print( std::ostream& out ) const
      { return out << "(intercept)"; }

      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const
      { return out << line_header << "intercept" << std::endl; }

//       virtual
//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       virtual
//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }

      std::auto_ptr< Action >
      deepCopy() const
      { return std::auto_ptr< Action >( new ActIntercept( *this ) ); }
    };


    class ActTackle
      : public Action
    {
    public:
      ActTackle() 
        : Action()
      {}
  
      ActTackle( const UNumSet& players ) 
        : Action(),
          M_players( players )
      {}

      ~ActTackle()
      {}

      std::ostream&
      print( std::ostream& out ) const
      { return out << "(tackle " << M_players << ")"; }

      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const
      { 
        return out << line_header 
                   << "tackle player(s) " 
                   << M_players << std::endl; 
      }

      /* class specific stuff */
      UNumSet&
      getPlayers()
      { return M_players; }
  
      const
      UNumSet&
      getPlayers() const 
      { return M_players; }
  
      void 
      setPlayers( const UNumSet& players )
      { M_players = players; }
 
      void
      addPlayer( const UNum& i )
      { M_players.add( i ); }
  
      void
      clearPlayers()
      { M_players.clear(); }

//       virtual
//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       virtual
//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }

      std::auto_ptr< Action >
      deepCopy() const
      { return std::auto_ptr< Action >( new ActTackle( *this ) ); }

    private:
      UNumSet M_players;
    };
      
      std::ostream&
      operator<<( std::ostream & os, const Action& a );

  }
}


#endif
