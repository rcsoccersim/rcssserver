// -*-c++-*-

/***************************************************************************
                                clangmetamsg.h  
                       Class for CLang Meta messages
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

#ifndef CLANGMETAMSG_H
#define CLANGMETAMSG_H

#include "clangmsg.h"
#include "hasa.h"
#include "visitor.h"
#include <list>

namespace rcss
{
  namespace clang
  {
    class MetaTokenVer;

    class MetaToken 
    {
    public:
//       typedef util::Visitor10< MetaTokenVer* > Visitor;
//       typedef util::Visitor10< const MetaTokenVer* > ConstVisitor;

//       class TypeExtractor
//         : public util::TypeExtractor10< MetaTokenVer* >
//       {
//       protected:
//         virtual
//         void
//         visit( MetaTokenVer* tok );
//       };

//       class ConstTypeExtractor
//         : public util::TypeExtractor10< const MetaTokenVer* >
//       {
//       protected:
//         virtual
//         void
//         visit( const MetaTokenVer* tok );
//       };

      MetaToken();
  
      virtual 
      ~MetaToken();

      virtual
      std::auto_ptr< MetaToken >
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
    };



    class MetaTokenVer 
      : public MetaToken
    {
    public:

      MetaTokenVer( const double& ver = 0.0 );

      virtual
      ~MetaTokenVer();

      virtual
      std::auto_ptr< MetaToken >
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

      double
      getVer() const;

    private:
	double m_ver;
    };
    


    class MetaMsg 
      : public Msg
    {
    public:
      typedef std::list< MetaToken* > Storage;

      MetaMsg();
      
      MetaMsg( const Storage& tokens );

      virtual
      ~MetaMsg();
      
      virtual
      std::auto_ptr< Msg >
      deepCopy() const;

//       virtual
//       void
//       accept( Visitor& v );

//       virtual
//       void
//       accept( ConstVisitor& v ) const;

      std::ostream&
      print( std::ostream& out ) const;
      
      std::ostream& 
      printPretty( std::ostream& out, const std::string& line_header ) const;
      
      Storage&
      getTokens();

      const Storage&
      getTokens() const;

	virtual
	Types
	getType() const { return META; }
	

    private:
	Storage m_tokens;
    };


      inline
      std::ostream&
      operator <<( std::ostream & os, const rcss::clang::MetaToken& mt )
      { return mt.print(os); } 
  }
}

#endif
