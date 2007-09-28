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
 *   Foundation; either version 2 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#ifndef _COND_H_
#define _COND_H_

#include "clangutil.h"
#include "visitor.h"
#include <memory>
#include "region.h"
#include "compop.h"
#include "hasa.h"
#include "rcssexceptions.h"

namespace rcss
{
  namespace clang
  {
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

    class Cond
    {
    public:
      class Context
      {
      public:
        Context()
        {}

        virtual 
        ~Context()
        {}

        virtual 
        bool
        lookup( const CondPlayerPos& cond ) const = 0;

        virtual 
        bool
        lookup( const CondBallPos& cond ) const = 0;

        virtual 
        bool
        lookup( const CondBallOwner& cond ) const = 0;

        virtual 
        bool
        lookup( const CondPlayMode& cond ) const = 0;

        virtual 
        bool
        lookup( const CondNamed& cond ) const = 0;

        virtual 
        bool
        lookup( const CondTime& cond ) const = 0;

        virtual 
        bool
        lookup( const CondOppGoal& cond ) const = 0;

        virtual 
        bool
        lookup( const CondOurGoal& cond ) const = 0;

        virtual 
        bool
        lookup( const CondGoalDiff& cond ) const = 0;

        virtual 
        bool
        lookup( const CondUNum& cond ) const = 0;
      }; // end Context

//       typedef util::Visitor20< CondBool*,
//         CondPlayerPos*,
//         CondBallPos*,
//         CondBallOwner*,
//         CondPlayMode*,
//         CondAnd*,
//         CondOr*,
//         CondNot*,
//         CondNamed*,
//         CondTime*,
//         CondOppGoal*,
//         CondOurGoal*,
//         CondGoalDiff*,
//         CondUNum* > Visitor;

//       typedef util::Visitor20< const CondBool*,
//         const CondPlayerPos*,
//         const CondBallPos*,
//         const CondBallOwner*,
//         const CondPlayMode*,
//         const CondAnd*,
//         const CondOr*,
//         const CondNot*,
//         const CondNamed*,
//         const CondTime*,
//         const CondOppGoal*,
//         const CondOurGoal*,
//         const CondGoalDiff*,
//         const CondUNum* > ConstVisitor;

//       class TypeExtractor
//         : public util::TypeExtractor20< CondBool*,
//         CondPlayerPos*,
//         CondBallPos*,
//         CondBallOwner*,
//         CondPlayMode*,
//         CondAnd*,
//         CondOr*,
//         CondNot*,
//         CondNamed*,
//         CondTime*,
//         CondOppGoal*,
//         CondOurGoal*,
//         CondGoalDiff*,
//         CondUNum* >
//       {
//       protected:
//         void
//         visit( CondBool* cond )
//         { setValue( cond ); }

//         void
//         visit( CondPlayerPos* cond )
//         { setValue( cond ); }

//         void
//         visit( CondBallPos* cond )
//         { setValue( cond ); }

//         void
//         visit( CondBallOwner* cond )
//         { setValue( cond ); }

//         void
//         visit( CondPlayMode* cond )
//         { setValue( cond ); }

//         void
//         visit( CondAnd* cond )
//         { setValue( cond ); }

//         void
//         visit( CondOr* cond )
//         { setValue( cond ); }

//         void
//         visit( CondNot* cond )
//         { setValue( cond ); }

//         void
//         visit( CondNamed* cond )
//         { setValue( cond ); }

//         void
//         visit( CondTime* cond )
//         { setValue( cond ); }

//         void
//         visit( CondOppGoal* cond )
//         { setValue( cond ); }

//         void
//         visit( CondOurGoal* cond )
//         { setValue( cond ); }

//         void
//         visit( CondGoalDiff* cond )
//         { setValue( cond ); }

//         void
//         visit( CondUNum* cond )
//         { setValue( cond ); }

//       };

//       class ConstTypeExtractor
//         : public util::TypeExtractor20< const CondBool*,
//         const CondPlayerPos*,
//         const CondBallPos*,
//         const CondBallOwner*,
//         const CondPlayMode*,
//         const CondAnd*,
//         const CondOr*,
//         const CondNot*,
//         const CondNamed*,
//         const CondTime*,
//         const CondOppGoal*,
//         const CondOurGoal*,
//         const CondGoalDiff*,
//         const CondUNum* >
//       {
//       protected:
//         void
//         visit( const CondBool* cond )
//         { setValue( cond ); }

//         void
//         visit( const CondPlayerPos* cond )
//         { setValue( cond ); }

//         void
//         visit( const CondBallPos* cond )
//         { setValue( cond ); }

//         void
//         visit( const CondBallOwner* cond )
//         { setValue( cond ); }

//         void
//         visit( const CondPlayMode* cond )
//         { setValue( cond ); }

//         void
//         visit( const CondAnd* cond )
//         { setValue( cond ); }

//         void
//         visit( const CondOr* cond )
//         { setValue( cond ); }

//         void
//         visit( const CondNot* cond )
//         { setValue( cond ); }

//         void
//         visit( const CondNamed* cond )
//         { setValue( cond ); }

//         void
//         visit( const CondTime* cond )
//         { setValue( cond ); }

//         void
//         visit( const CondOppGoal* cond )
//         { setValue( cond ); }

//         void
//         visit( const CondOurGoal* cond )
//         { setValue( cond ); }

//         void
//         visit( const CondGoalDiff* cond )
//         { setValue( cond ); }

//         void
//         visit( const CondUNum* cond )
//         { setValue( cond ); }

//        };


      Cond()
      {}
  
      virtual
      ~Cond()
      {}

      virtual
      std::ostream&
      print( std::ostream& out ) const = 0;

      virtual
      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const = 0;

      virtual
      bool 
      eval( const Context& context ) const = 0;

//       virtual
//       void
//       accept( Visitor& v ) = 0;

//       virtual
//       void
//       accept( ConstVisitor& v ) const = 0;

      virtual
      std::auto_ptr< Cond >
      deepCopy() const = 0;
    };

      inline 
      std::ostream& 
      operator<<( std::ostream & os, const Cond& c )
      { return c.print(os); }

    class CondBool
      : public Cond
    {
    public:

      CondBool() 
        : Cond(),
          m_state( false )
      {}

      CondBool( bool state ) 
        : Cond(),
          m_state( state )
      {}

      ~CondBool()
      {}

      std::ostream& 
      print( std::ostream& out ) const 
      { return out << "(" << ( getState() ? "true" : "false" ) << ")"; }

      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const
      { return out << line_header << ( getState() ? "true" : "false" ) << std::endl; }

      bool
      eval( const Context& ) const
      { return m_state; }

      bool
      getState() const
      { return m_state; }

//       virtual
//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       virtual
//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }

      virtual
      std::auto_ptr< Cond >
      deepCopy() const
      { return std::auto_ptr< Cond >( new CondBool( *this ) ); }

    private:
	bool m_state;
    };

    class CondPlayerPos
      : public Cond
    {
    public:
 
      CondPlayerPos()
	  : Cond(),
          M_our_side( false ),
          M_min_match( 1 ),
          M_max_match( 11 )
      {}

      CondPlayerPos( const bool& our_side,
                     const UNumSet& players,
                     const int& min_match,
                     const int& max_match,
                     std::auto_ptr< Region > reg )
        : Cond(),
          M_our_side( our_side ),
          M_min_match( min_match ),
          M_max_match( max_match ),
          m_players( players ),
          m_reg( reg )
      {}
      
      virtual
      ~CondPlayerPos()
      {}

      std::ostream&
      print( std::ostream& out ) const;

      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const;

      bool
      eval( const Context& context ) const
      { return context.lookup( *this ); }

      /* class specific stuff */
      bool
      isOurSide() const 
      { return M_our_side; }

      bool
      isTheirSide() const
      { return !M_our_side; }
  
      int
      getMinMatch() const
      { return M_min_match; }

      int
      getMaxMatch() const
      { return M_max_match; }

      const Region*
      getRegion() const
      { return m_reg.get(); }

      UNumSet
      getPlayerSet() 
      { return m_players; }

      void
      setMinMatch( int x )
      { M_min_match = x; }
  
      void
      setMaxMatch( int x )
      { M_max_match = x; }
  
      void
      set( std::auto_ptr< Region > reg )
      { m_reg = reg; }
      
      std::auto_ptr< Region >
      detachRegion()
      { return m_reg; }

      void
      setOurSide( const bool& our_side )
      { M_our_side = our_side; }
  
      void
      set( const UNumSet& players )
      { m_players = players; }
  
      void
      addPlayer( const UNum& i )
      { m_players.add( i ); }

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
      std::auto_ptr< Cond >
      deepCopy() const
      {
	  if( m_reg.get() != NULL )
	      return std::auto_ptr< Cond >( new CondPlayerPos( M_our_side,
							       m_players,
							       M_min_match,
							       M_max_match,
							       m_reg->deepCopy() ) );
	  else
	      return std::auto_ptr< Cond >( new CondPlayerPos( M_our_side,
							       m_players,
							       M_min_match,
							       M_max_match,
							       std::auto_ptr< Region >() ) );
      }

    private:
      bool M_our_side;
      int M_min_match;
      int M_max_match;
	UNumSet m_players;
	std::auto_ptr< Region > m_reg;
    };

    class CondBallPos
      : public Cond
    {
    public:

      CondBallPos()
        : Cond()
      {}

      CondBallPos( std::auto_ptr< Region > reg )
        : Cond(),
          m_reg( reg ) 
      {}

      ~CondBallPos()
      {}

      std::ostream&
      print( std::ostream& out ) const
      { 
        out << "(bpos ";
        if( getRegion() == NULL )
          out << "(null)";
        else
          out << *getRegion();
        return out << ")";
      }

      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const
      { 
        out << line_header << "is ball position at: " << std::endl;
        if( getRegion() == NULL )
          out << line_header << " (null)\n";
        else
          getRegion()->printPretty( out, line_header + " " );
        return out;
      }

      bool
      eval( const Context& context ) const 
      { return context.lookup( *this ); }
  
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
      std::auto_ptr< Cond >
      deepCopy() const
      { 
	  if( getRegion() )
	      return std::auto_ptr< Cond >( new CondBallPos( m_reg->deepCopy() ) );
	  else
	      return std::auto_ptr< Cond >( new CondBallPos() );
      }
    private:
	std::auto_ptr< Region > m_reg;
    };

    class CondBallOwner
      : public Cond
    {
    public:
      CondBallOwner()
        : Cond(),
          M_our_side( false )
      {}

      CondBallOwner( const bool& our_side, const UNumSet& players )
        : Cond(),
          M_our_side( our_side ),
          M_players( players )
      {}

      ~CondBallOwner()
      {}

      std::ostream&
      print( std::ostream& out ) const;

      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const;

      bool
      eval( const Context& context ) const
      { return context.lookup( *this ); }

      /* class specific stuff */
      bool
      isOurSide() const
      { return M_our_side; }
  
      bool
      isTheirSide() const
      { return !M_our_side; }

      UNumSet& 
      getPlayerSet() 
      { return M_players; }

      const UNumSet& 
      getPlayerSet() const 
      { return M_players; }

      void
      setOurSide( const bool& our_side )
      { M_our_side = our_side; }

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

      std::auto_ptr< Cond >
      deepCopy() const
      { return std::auto_ptr< Cond >( new CondBallOwner( *this ) ); }

    private:
      bool M_our_side;
      UNumSet M_players;
    };



    class CondPlayMode
      : public Cond
    {
    public:
      static const char* MODE_STRINGS[];

      CondPlayMode()
        : Cond(),
          M_pm( PM_None )
      {}

      CondPlayMode( const PlayMode& pm )
        : Cond(),
          M_pm( pm )
      {}

      ~CondPlayMode()
      {}

      std::ostream&
      print( std::ostream& out ) const
      { return out << "(playm " << MODE_STRINGS[ M_pm ] << ")"; }

      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const
      { return out << line_header << "is play mode '"
                   << MODE_STRINGS[ M_pm ] << "'" << std::endl; }

      bool
      eval( const Context& context ) const
      { return context.lookup( *this ); }
  
      /* class specific stuff */
      PlayMode 
      getPlayMode() const
      { return M_pm; }

      void
      setPlayMode( const PlayMode& pm)
      { M_pm = pm; }

//       virtual
//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       virtual
//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }

      std::auto_ptr< Cond >
      deepCopy() const
      { return std::auto_ptr< Cond >( new CondPlayMode( *this ) ); }

    private:
      PlayMode M_pm;
    };

    class CondAnd
      : public Cond
    {
    public:
	typedef std::list< Cond* > Storage;
	
      CondAnd()
        : Cond()
      {}

      CondAnd( const Storage& conds ) 
        : Cond(),
          m_conds( conds )
      {
      }

      virtual
      ~CondAnd()
      {
	  for( Storage::iterator i = m_conds.begin();
	       i != m_conds.end(); ++i )
	      delete *i;
	  m_conds.clear();
      }

      std::ostream&
      print( std::ostream& out ) const;

      std::ostream& 
      printPretty( std::ostream& out, const std::string& line_header ) const;

      bool
      eval( const Context& context ) const;

      const Storage& 
      getConds() const 
      { return m_conds; }

//       virtual
//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       virtual
//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }

      virtual
      std::auto_ptr< Cond >
      deepCopy() const
      {
	  Storage new_conds;
	  for( Storage::const_iterator i = m_conds.begin();
	       i != m_conds.end(); ++i )
	      new_conds.push_back( (*i)->deepCopy().release() );
	  return std::auto_ptr< Cond >( new CondAnd( new_conds ) ); 
      }

    private:
	Storage m_conds;
	
    };


    class CondOr
      : public Cond
    {
    public:
 	typedef std::list< Cond* > Storage;
 
      CondOr() 
        : Cond()
      {}

      CondOr( const Storage& conds ) 
        : Cond(),
          m_conds( conds )
      {}

      virtual
      ~CondOr()
      {	  
	  for( Storage::iterator i = m_conds.begin();
	       i != m_conds.end(); ++i )
	      delete *i;
	  m_conds.clear();
      }

      std::ostream&
      print( std::ostream& out ) const;

      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const;

      bool
      eval( const Context& context ) const;

      const Storage&
      getConds() const 
      { return m_conds; }

//       virtual
//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       virtual
//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }

      virtual
      std::auto_ptr< Cond >
      deepCopy() const
      {
	  Storage new_conds;
	  for( Storage::const_iterator i = m_conds.begin();
	       i != m_conds.end(); ++i )
	      new_conds.push_back( (*i)->deepCopy().release() );
	  return std::auto_ptr< Cond >( new CondOr( new_conds ) ); 
      }

    private:
	Storage m_conds;
    };


    class CondNot
      : public Cond
    {
    public:

      CondNot()
        : Cond()
      {}

      CondNot( std::auto_ptr< Cond > cond ) 
        : Cond(),
          m_cond( cond )
      {}

      virtual
      ~CondNot()
      {}

      std::ostream& 
      print( std::ostream& out ) const 
      { 
        out << "(not ";
        if( getCond() == NULL )
          out << "(null)\n";
        else
          out << *getCond();
        return out << ")";
      }

      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const
      { 
        out << line_header << "not " << std::endl;
        if( getCond() == NULL )
          out << line_header << " (null)\n";
        else
          getCond()->printPretty( out, line_header + " +" );
        return out;
      }

      bool
      eval( const Context& context ) const
      {
        if( getCond() == NULL )
          throw util::NullErr( __FILE__, __LINE__, 
                               "Null condition in CondNot\n" );
        else
	    return !( getCond()->eval( context ) );
      }
  
      const Cond*
      getCond() const
      { return m_cond.get(); }

      std::auto_ptr< Cond >
      detachCond()
      { return m_cond; }

//       virtual
//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       virtual
//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }

      virtual
      std::auto_ptr< Cond >
      deepCopy() const
      {
	  if( m_cond.get() != NULL )
	      return std::auto_ptr< Cond >( new CondNot( m_cond->deepCopy() ) ); 
	  else
	      return std::auto_ptr< Cond >( new CondNot() ); 
      }

    private:
	std::auto_ptr< Cond > m_cond;
    };


    class CondNamed
      : public Cond
    {
    public:
      CondNamed() 
        : Cond()
      {}
  
      CondNamed( const std::string& name ) 
        : Cond(),
          M_name( name )
      {}
  
      ~CondNamed()
      {}

      std::ostream&
      print( std::ostream& out ) const
      { return out << "\"" << M_name << "\""; }

      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const
      { return out << line_header << "cond named \"" 
                   << M_name << "\"" << std::endl; }
  
      bool
      eval( const Context& context ) const 
      { return context.lookup ( *this ); }

      /* class specific */
      std::string& 
      getName()
      { return M_name; }

      const std::string& 
      getName() const
      { return M_name; }

      void
      setName( const std::string& name )
      { M_name = name; }
 
//       virtual
//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       virtual
//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }

      std::auto_ptr< Cond >
      deepCopy() const
      { return std::auto_ptr< Cond >( new CondNamed( *this ) ); }

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
      CondComp( const VALUE& value, const util::CompOp& comp )
        : M_comp( &comp ),
          M_value( value )
      {}

      ~CondComp()
      {}

      VALUE
      getValue() const
      { return M_value; }

      VALUE
      setValue( const VALUE& value )
      { return M_value = value; }

      const util::CompOp&
      getComp() const
      { return *M_comp; }

      const util::CompOp&
      setComp( const util::CompOp& comp )
      { return *(M_comp = &comp); }      
    };

    class CondTime
      : public CondComp< int >
    {
    public:
      CondTime( const int& value, const util::CompOp& comp )
        : CondComp< int >( value, comp )
      {}

      ~CondTime()
      {}

      virtual
      std::ostream&
      print( std::ostream& out ) const
      { return out << "(time " << getComp() << " " << getValue() << ")"; }

      virtual
      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const
      { return print( out << line_header ); }

      virtual
      bool 
      eval( const Context& context ) const
      { return context.lookup( *this ); }

//       virtual
//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       virtual
//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }

      std::auto_ptr< Cond >
      deepCopy() const
      { return std::auto_ptr< Cond >( new CondTime( *this ) ); }
    };


    class CondOppGoal
      : public CondComp< int >
    {
    public:
      CondOppGoal( const int& value, const util::CompOp& comp )
        : CondComp< int >( value, comp )
      {}

      ~CondOppGoal()
      {}

      virtual
      std::ostream&
      print( std::ostream& out ) const
      { return out << "(opp_goals " << getComp() << " " << getValue() << ")"; }

      virtual
      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const
      { return print( out << line_header ); }

      virtual
      bool 
      eval( const Context& context ) const
      { return context.lookup( *this ); }

 //      virtual
//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       virtual
//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }

      std::auto_ptr< Cond >
      deepCopy() const
      { return std::auto_ptr< Cond >( new CondOppGoal( *this ) ); }
    };

    class CondOurGoal
      : public CondComp< int >
    {
    public:
      CondOurGoal( const int& value, const util::CompOp& comp )
        : CondComp< int >( value, comp )
      {}

      ~CondOurGoal()
      {}

      virtual
      std::ostream&
      print( std::ostream& out ) const
      { return out << "(our_goals " << getComp() << " " << getValue() << ")"; }

      virtual
      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const
      { return print( out << line_header ); }

      virtual
      bool 
      eval( const Context& context ) const
      { return context.lookup( *this ); }

//       virtual
//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       virtual
//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }

      std::auto_ptr< Cond >
      deepCopy() const
      { return std::auto_ptr< Cond >( new CondOurGoal( *this ) ); }
    };

    class CondGoalDiff
      : public CondComp< int >
    {
    public:
      CondGoalDiff( const int& value, const util::CompOp& comp )
        : CondComp< int >( value, comp )
      {}

      ~CondGoalDiff()
      {}

      virtual
      std::ostream&
      print( std::ostream& out ) const
      { return out << "(goal_diff " << getComp() << " " << getValue() << ")"; }

      virtual
      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const
      { return print( out << line_header ); }

      virtual
      bool 
      eval( const Context& context ) const
      { return context.lookup( *this ); }

 //      virtual
//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       virtual
//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }

      std::auto_ptr< Cond >
      deepCopy() const
      { return std::auto_ptr< Cond >( new CondGoalDiff( *this ) ); }
    };

    
    class CondUNum
      : public Cond
    {
    public:
     CondUNum()
        : Cond(),
          M_unum(),
          M_set()
      {}

     CondUNum( const UNum& unum )
        : Cond(),
          M_unum( unum ),
          M_set()
      {}

      CondUNum( const UNum& unum, const UNumSet& players )
        : Cond(),
          M_unum( unum ),
          M_set( players )
      {}

      ~CondUNum()
      {}

      virtual
      std::ostream&
      print( std::ostream& out ) const
      { return out << "(unum " << M_unum << " " << M_set << ")"; }

      virtual
      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const
      { return print( out << line_header ); }

      virtual
      bool 
      eval( const Context& context ) const
      { return context.lookup( *this ); }

//       virtual
//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       virtual
//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }

      std::auto_ptr< Cond >
      deepCopy() const
      { return std::auto_ptr< Cond >( new CondUNum( *this ) ); }

      UNum&
      setUNum( const UNum& unum )
      { return M_unum = unum; }

      UNum
      getUNum() const
      { return M_unum; }
      
      UNumSet&
      setUNum( const UNumSet& uset )
      { return M_set = uset; }

      UNumSet&
      getUNumSet()
      { return M_set; }
      
      const UNumSet&
      getUNumSet() const
      { return M_set; }
      
      UNum
      addUNum( const UNum& unum )
      { M_set.add( unum ); return unum; }

    private:
      UNum M_unum;
      UNumSet M_set;
    };
  }
}

#endif
