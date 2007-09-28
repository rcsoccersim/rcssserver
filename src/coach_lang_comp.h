/* -*- Mode: C++ -*- */

/*
 *Copyright:

    Copyright (C) 1996-2000 Electrotechnical Laboratory. 
    	Itsuki Noda, Yasuo Kuniyoshi and Hitoshi Matsubara.
    Copyright (C) 2000, 2001 RoboCup Soccer Server Maintainance Group.
    	Patrick Riley, Tom Howard, Daniel Polani, Itsuki Noda,
	Mikhail Prokopenko, Jan Wendler 

    This file is a part of SoccerServer.

    This code is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

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

#ifndef _H_COACH_LANG_COMP
#define _H_COACH_LANG_COMP

//  #ifdef HAVE_CONFIG_H
//  #include "config.h"
//  #endif

//  #ifdef HAVE_SSTREAM
//  #include <sstream>
//  #else
//  #include <strstream>
//  #endif


#include <iostream>
#include <vector>
#include <memory>

#include "region.h"
#include "visitor.h"
#include "clangutil.h"
#include "hasa.h"
#include "clangaction.h"

namespace rcss
{
  namespace clang
  {

    /******** Dir ********************/
    class DirComm;
    class DirNamed;

    class Dir
    {
    public:
//       typedef util::TypeList< DirComm*,
//         util::TypeList< DirNamed*,
//         util::NullType > > LocalTypeList;

//       typedef util::TypeList< const DirComm*,
//         util::TypeList< const DirNamed*,
//         util::NullType > > ConstLocalTypeList;

//       typedef util::Visitor10< DirComm*, DirNamed* > Visitor;
//       typedef util::Visitor10< const DirComm*, const DirNamed* > ConstVisitor;

//       class TypeExtractor
//         : public util::TypeExtractor10< DirComm*, DirNamed* >
//       {
//       protected:
//         void
//         visit( DirComm* dir )
//         { setValue( dir ); }

//         void
//         visit( DirNamed* dir )
//         { setValue( dir ); }
//       };

//       class ConstTypeExtractor
//         : public util::TypeExtractor10< const DirComm*, const DirNamed* >
//       {
//       protected:
//         void
//         visit( const DirComm* dir )
//         { setValue( dir ); }

//         void
//         visit( const DirNamed* dir )
//         { setValue( dir ); }
//       };


 
      Dir()
      {}

      virtual
      ~Dir()
      {}
  
      virtual
      std::ostream&
      print( std::ostream& out ) const = 0;

      virtual
      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const = 0;

//       virtual
//       void
//       accept( Visitor& v ) = 0;

//       virtual
//       void
//       accept( ConstVisitor& v ) const= 0;

      virtual
      std::auto_ptr< Dir >
      deepCopy() const = 0;

    }; // end Dir

      inline
      std::ostream&
      operator <<( std::ostream & os, const Dir& d )
      { return d.print(os); } 


    class DirComm
      : public Dir
    {
    public:
	typedef std::list< Action* > Storage;


      DirComm()
	  : Dir()
      {}

      DirComm( bool positive,
               bool our_side,
               const UNumSet& players,
               const Storage& actions )
	  : Dir(),
	    m_positive( positive ),
	    m_our_side( our_side ),
	    m_players( players ),
	    m_actions( actions )
      {}

      ~DirComm() 
      {
	  clearActions();
      }

      std::ostream&
      print( std::ostream& out ) const;

      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const;

      bool
      isPositive() const
      { return m_positive; }

      bool
      isNegative() const
      { return !isPositive(); }

      bool
      isOurSide() const
      { return m_our_side; }

      bool
      isTheirSide() const
      { return !isOurSide(); }

      UNumSet
      getPlayerSet() const 
      { return m_players; }

      const Storage&
      getActions() const
      { return m_actions; }

      void 
      setSense( const bool& positive )
      { m_positive = positive; }

      void
      setOurSide( const bool& our_side )
      { m_our_side = our_side; }

      void
      set( const UNumSet& players )
      { m_players =  players; }

      void
      add( const UNum& i )
      { m_players.add( i ); }
  
      void
      clearPlayers()
      { m_players.clear(); }

	void
	clearActions()
	    {
		for( Storage::iterator i = m_actions.begin();
		     i != m_actions.end(); ++i )
		{
		    delete *i;
		}
		m_actions.clear();
	    }
	
      void
      set( const Storage& actions )
      {
	  clearActions();
	  m_actions = actions; 
      }
  
      void
      add( std::auto_ptr< Action > action )
      { m_actions.push_front( action.release() ); }

      Storage
      detachActions()
      {
	  Storage rval = m_actions;
	  m_actions.clear();
	  return rval; 
      }

//       virtual
//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       virtual
//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }

      virtual
      std::auto_ptr< Dir >
      deepCopy() const
      {
	  Storage new_actions;
	  for( Storage::const_iterator i = getActions().begin();
               i != getActions().end(); ++i )
	      new_actions.push_back( (*i)->deepCopy().release() );
				     
	  return std::auto_ptr< Dir >( new DirComm( m_positive,
						    m_our_side,
						    m_players,
						    new_actions ) ); 
      }

    private:
	bool m_positive;
	bool m_our_side;
	UNumSet m_players;
	std::list< Action* > m_actions;
    };

    class DirNamed
      : public Dir
    {
    public:
      DirNamed( const std::string& name = "" ) 
        : Dir(),
          M_name( name )
      {}

      ~DirNamed() {}

      std::ostream& 
      print( std::ostream& out ) const
      { return out << "\"" << M_name << "\""; }

      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const
      { 
        return out << line_header 
                   << "directived named \"" 
                   << M_name << "\"" << std::endl;
      }
  
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

 //      void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }

      std::auto_ptr< Dir >
      deepCopy() const
      { return std::auto_ptr< Dir >( new DirNamed( *this ) ); }

    private:
      std::string M_name;
    };
  }
}

#include "cond.h"


namespace rcss
{
  namespace clang
  {

   /******** Tokens ********************/
    class TokClear;
    class TokRule;

    class Token 
    {
    public:
//       typedef util::Visitor10< TokClear*,
//         TokRule* > Visitor;

//       typedef util::Visitor10< const TokClear*,
//         const TokRule* > ConstVisitor;

//       class TypeExtractor
//         : public util::TypeExtractor10< TokClear*,
//         TokRule* >
//       {
//       protected:
//         void
//         visit( TokClear* tok )
//         { setValue( tok ); }

//         void
//         visit( TokRule* tok )
//         { setValue( tok ); }
//       };

//       class ConstTypeExtractor
//         : public util::TypeExtractor10< const TokClear*,
//         const TokRule* >
//       {
//       protected:
//         void
//         visit( const TokClear* tok )
//         { setValue( tok ); }

//         void
//         visit( const TokRule* tok )
//         { setValue( tok ); }
//       };

      Token()
      {}
  
      virtual
      ~Token()
      {}

      virtual
      std::ostream& 
      print( std::ostream& out ) const = 0;

      virtual
      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const = 0;

//       virtual
//       void
//       accept( Visitor& v ) = 0;

//       virtual
//       void
//       accept( ConstVisitor& v ) const = 0;
      
      virtual
      std::auto_ptr< Token >
      deepCopy() const = 0;

    };
 
      inline
      std::ostream&
      operator <<( std::ostream & os, const Token& t )
      { return t.print(os); } 
      

      class TokClear
	  : public Token
    {
    public:
      TokClear() 
        : Token() 
      {}

      ~TokClear() 
      {}

      std::ostream&
      print( std::ostream& out ) const 
      { return out << "(clear)"; }

      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const
      { return out << line_header << "CLEAR token" << std::endl; }
  
//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }

      std::auto_ptr< Token >
      deepCopy() const
      { return std::auto_ptr< Token >( new TokClear( *this ) ); }
    };


    class TokRule
	: public Token
    {
    public:
      typedef std::list< Dir* > Storage;

      TokRule( const int& ttl = 0 ) 
        : Token(),
          m_ttl( ttl )
      {}
  
      TokRule( const int& ttl,
               std::auto_ptr< Cond >& cond,
               const Storage& dirs ) 
        : Token(),
          m_ttl( ttl ),
          m_cond( cond ),
          m_dirs( dirs )
      {}
  
      virtual
      ~TokRule() 
      {
	  clearDirs();
      }
 
      std::ostream& 
      print( std::ostream& out ) const;

      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const;

      /* class specific stuff */
      int
      getTTL() const
      { return m_ttl; }

      void
      set( int ttl )
      { m_ttl = ttl; }
  
      Cond* 
      getCond() 
      { return m_cond.get(); }

      const Cond* 
      getCond() const 
      { return m_cond.get(); }
  
      void 
      set( std::auto_ptr< Cond > cond )
      { m_cond = cond; }

      std::auto_ptr< Cond >
      detachCond() 
      { return m_cond; }

      const Storage&
      getDirs() const
      { return m_dirs; }

      Storage&
      getDirs()
      { return m_dirs; }

	void
	clearDirs()
	    {
		for( Storage::iterator i = m_dirs.begin();
		     i != m_dirs.end(); ++i )
		    delete *i;
		m_dirs.clear();
	    }

      void 
      set( const Storage& dirs ) 
      {
	  clearDirs();
	  m_dirs =  dirs; 
      }

//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }

      virtual
      std::auto_ptr< Token >
      deepCopy() const
      { 
	  std::auto_ptr< Cond > new_cond;
	  if( getCond() )
	      new_cond = getCond()->deepCopy();

	  Storage new_dirs;
	  for( Storage::const_iterator i = m_dirs.begin();
	       i != m_dirs.end(); ++i )
	      new_dirs.push_back( (*i)->deepCopy().release() );
	  
	  return std::auto_ptr< Token >( new TokRule( m_ttl,
						      new_cond,
						      new_dirs ) ); 
      }

    private:
	int m_ttl;
	std::auto_ptr< Cond > m_cond;
	Storage m_dirs;
    };

    /******** Defs ********************/
    class DefCond;
    class DefDir;
    class DefReg;
    class DefAct;
    class DefRule;

    class Def 
    {
    public:
//       typedef util::Visitor10< DefCond*,
//         DefDir*,
//         DefReg*,
//         DefAct*,
//         DefRule* > Visitor;

//       typedef util::Visitor10< const DefCond*,
//         const DefDir*,
//         const DefReg*,
//         const DefAct*,
//         const DefRule* > ConstVisitor;

//       class TypeExtractor
//         : public util::TypeExtractor10< DefCond*,
//         DefDir*,
//         DefReg*,
//         DefAct*,
//         DefRule* >
//       {
//       protected:
//         void
//         visit( DefCond* def )
//         { setValue( def ); }

//         void
//         visit( DefDir* def )
//         { setValue( def ); }

//         void
//         visit( DefReg* def )
//         { setValue( def ); }

//         void
//         visit( DefAct* def )
//         { setValue( def ); }

//         void
//         visit( DefRule* def )
//         { setValue( def ); }
//      };

//       class ConstTypeExtractor
//         : public util::TypeExtractor10< const DefCond*,
//         const DefDir*,
//         const DefReg*,
//         const DefAct*,
//         const DefRule* >
//       {
//       protected:
//         void
//         visit( const DefCond* def )
//         { setValue( def ); }

//         void
//         visit( const DefDir* def )
//         { setValue( def ); }

//         void
//         visit( const DefReg* def )
//         { setValue( def ); }

//         void
//         visit( const DefAct* def )
//         { setValue( def ); }

//         void
//         visit( const DefRule* def )
//         { setValue( def ); }
//        };


      Def()
      {}

      Def( const std::string& name )
        : M_name( name )
      {}

      virtual 
      ~Def()
      {}

      virtual 
      std::ostream& 
      print( std::ostream& out ) const = 0;

      virtual
      std::ostream& 
      printPretty( std::ostream& out, const std::string& line_header ) const = 0;
      
      const std::string&
      getName() const 
      { return M_name; }

      void 
      setName( const std::string& name )
      { M_name = name; }
  
//       virtual
//       void
//       accept( Visitor& v ) = 0;
      
//       virtual
//       void
//       accept( ConstVisitor& v ) const = 0;
    
      virtual
      std::auto_ptr< Def >
      deepCopy() const = 0;

    protected:
      std::string M_name;
    };

      inline
      std::ostream&
      operator <<( std::ostream & os, const Def& t )
      { return t.print(os); }


    class DefCond
      : public Def
    {
    public:
      DefCond()
        : Def(),
          M_cond( (Cond*)NULL )
      {}

      DefCond( const std::string& name, std::auto_ptr< Cond > cond )
        : Def( name ),
          M_cond( cond )
      {}

      DefCond( const DefCond& def )
        : Def( def ),
          M_cond( def.M_cond->deepCopy() )
      {}

      ~DefCond()
      {}

      DefCond&
      operator=( const DefCond& def )
      {
        setName( def.getName() );
        std::auto_ptr< Cond > cond( def.M_cond->deepCopy() );
        M_cond = cond;
        return *this;
      }

      std::ostream&
      print( std::ostream& out ) const 
      { 
        out << "(definec \"" << M_name << "\" ";
        if( M_cond.get() == NULL )
          out << " (null)";
        else
          out << *M_cond;
        return out << ")"; 
      }

      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const
      { 
        out << line_header << "Cond \"" << M_name << "\"" << std::endl;
        if( M_cond.get() == NULL )
          out << line_header << " (null)\n";
        else
          M_cond->printPretty( out, line_header + " " );
        return out;
      }

      Cond*
      getCond()
      { return M_cond.get(); }

      const Cond*
      getCond() const
      { return M_cond.get(); }

      void
      setCond( std::auto_ptr< Cond >& cond )
      { M_cond = cond; }

      std::auto_ptr< Cond >
      detachCond()
      { return M_cond; }
  
//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }

      std::auto_ptr< Def >
      deepCopy() const
      { return std::auto_ptr< Def >( new DefCond( *this ) ); }

    private:
      std::auto_ptr< Cond > M_cond;
    };


    class DefDir
      : public Def
    {
    public:
      DefDir()
        : Def(),
          M_dir( (Dir*)NULL )
      {}

      DefDir( const std::string& name,
              std::auto_ptr< Dir > dir )
        : Def( name ),
          M_dir( dir )
      {}

      DefDir( const DefDir& def )
        : Def( def ),
          M_dir( def.M_dir->deepCopy() )
      {}

      ~DefDir() 
      {}

      DefDir&
      operator=( const DefDir& def )
      {
        setName( def.getName() );
        std::auto_ptr< Dir > dir( def.M_dir->deepCopy() );
        M_dir = dir;
        return *this;
      }

      std::ostream&
      print( std::ostream& out ) const 
      {
        out << "(defined \"" << M_name << "\" ";
        if( M_dir.get() == NULL )
          out << "(null)";
        else
          out << *M_dir;
        return out << ")";
      }

      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const
      { 
        out << line_header << "Dir \"" << M_name << "\"" << std::endl;
        if( M_dir.get() == NULL )
          out << line_header << " (null)\n";
        else
          M_dir->printPretty( out, line_header + " " );
        return out;
      }

      Dir*
      getDir()
      { return M_dir.get(); }

      const Dir*
      getDir() const
      { return M_dir.get(); }


      void
      setDir( std::auto_ptr< Dir >& dir )
      { M_dir = dir; }

      std::auto_ptr< Dir >
      detachDir()
      { return M_dir; }

//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }

      std::auto_ptr< Def >
      deepCopy() const
      { return std::auto_ptr< Def >( new DefDir( *this ) ); }

    private:
      std::auto_ptr< Dir > M_dir;
    };


    class DefReg
      : public Def
    {
    public:
      DefReg()
        : Def(),
          M_reg( (Region*)NULL )
      {}

      DefReg( const std::string& name,
              std::auto_ptr< Region > reg )
        : Def( name ),
          M_reg( reg )
      {}

      DefReg( const DefReg& def )
        : Def( def ),
          M_reg( def.M_reg->deepCopy() )
      {}

      ~DefReg() 
      {}

      DefReg&
      operator=( const DefReg& def )
      {
        setName( def.getName() );
        std::auto_ptr< Region > reg( def.M_reg->deepCopy() );
        M_reg = reg;
        return *this;
      }

      std::ostream&
      print( std::ostream& out ) const
      { 
        out << "(definer \"" << M_name << "\" ";
        if( M_reg.get() == NULL )
          out << "(null)";
        else
          out << *M_reg;
        return out << ")";
      }

      std::ostream&
      printPretty( std::ostream& out, const std::string& line_header ) const
      { 
        out << line_header << "Region \"" << M_name << "\"" << std::endl;
        if( M_reg.get() == NULL )
          out << line_header << " (null)\n";
        else
          M_reg->printPretty( out, line_header + " " );
        return out;
      }

      Region*
      getReg()
      { return M_reg.get(); }

      const Region*
      getReg() const
      { return M_reg.get(); }

      void
      setReg( std::auto_ptr< Region >& reg )
      { M_reg = reg; }

      std::auto_ptr< Region >
      detachReg()
      { return M_reg; }

//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }

      std::auto_ptr< Def >
      deepCopy() const
      { return std::auto_ptr< Def >( new DefReg( *this ) ); }

    private:
      std::auto_ptr< Region > M_reg;
    };

    class DefAct
      : public Def
    {
    public:
      DefAct()
        : Def(),
          M_act( (Action*)NULL )
      {}

      DefAct( const std::string& name,
              std::auto_ptr< Action > act )
        : Def( name ),
          M_act( act )
      {}

      DefAct( const DefAct& def )
        : Def( def ),
          M_act( def.M_act->deepCopy() )
      {}

      ~DefAct()
      {}

      DefAct&
      operator=( const DefAct& def )
      {
        setName( def.getName() );
        std::auto_ptr< Action > act( def.M_act->deepCopy() );
        M_act = act;
        return *this;
      }

      std::ostream&
      print( std::ostream& out ) const
      { 
        out << "(definea \"" << M_name << "\" ";
        if( M_act.get() == NULL )
          out << "(null)";
        else
          out << *M_act;
        return out << ")"; 
      }

      std::ostream& 
      printPretty( std::ostream& out, const std::string& line_header ) const
      { 
        out << line_header << "Action \"" << M_name << "\"" << std::endl;
        if( M_act.get() == NULL )
          out << line_header << " (null)\n";
        else
          M_act->printPretty( out, line_header + " ");
        return out;
      }

      Action*
      getAction()
      { return M_act.get(); }

      const Action*
      getAction() const
      { return M_act.get(); }

      void 
      setAction( std::auto_ptr< Action >& act )
      { M_act = act; }

      std::auto_ptr< Action >
      detachAction()
      { return M_act; }
 
//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }

      std::auto_ptr< Def >
      deepCopy() const
      { return std::auto_ptr< Def >( new DefAct( *this ) ); }

    private:
      std::auto_ptr< Action > M_act;
    };
  }
}

#include "rule.h"

namespace rcss
{
  namespace clang
  {
    class DefRule
      : public Def
    {
    public:

      DefRule()
	  : Def(),
          M_model( false )
      {}

      DefRule( const std::string& name,
               std::auto_ptr< Rule > rule,
               bool model )
        : Def( name ),
          m_rule( rule ),
          M_model( model )
      {}

      virtual
      ~DefRule()
      {}

      std::ostream&
      print( std::ostream& out ) const;

      std::ostream& 
      printPretty( std::ostream& out, const std::string& line_header ) const;

      const Rule*
      getRule() const
      { return m_rule.get(); }

      std::auto_ptr< Rule >
      detachRule()
      { return m_rule; }
 
      bool
      isModel() const
      { return M_model; }

      void
      setModel( bool model )
      { M_model = model; }

//       void
//       accept( Visitor& v )
//       { v.startVisit( this ); }

//       void
//       accept( ConstVisitor& v ) const
//       { v.startVisit( this ); }

      std::auto_ptr< Def >
      deepCopy() const
      {
	  if( getRule() )
	      return std::auto_ptr< Def >( new DefRule( getName(),
							getRule()->deepCopy(),
							M_model ) ); 
	  else
	      return std::auto_ptr< Def >( new DefRule( getName(),
							std::auto_ptr< Rule >(),
							M_model ) ); 
      }

    private:
	std::auto_ptr< Rule > m_rule;
      bool M_model;
    };
  }
}

#endif
