// -*-c++-*-

/***************************************************************************
                                factory.hpp
                             -------------------
    Template singleton for creating polymorphic objects based on some idx
    begin                : 2002-10-08
    copyright            : (C) 2002 by The RoboCup Soccer Simulator
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

#ifndef RCSSFACTORY_H
#define RCSSFACTORY_H
#include <typeinfo>
#include <map>
#include <list>
#include <stack>
#include <iostream>
#include <memory>
#include <cstring>

namespace rcss {

template< typename X >
class Less {
public:
    bool operator()( const X & a,
                     const X & b ) const
      {
          return a < b;
      }
};

template<>
class Less< const char * > {
public:
    bool operator()( const char * a,
                     const char * b ) const
      {
          return std::strcmp( a, b ) < 0;
      }
};

template<>
class Less< char * > {
public:
    bool operator()( char * a,
                     char * b ) const
      {
          return std::strcmp( a, b ) < 0;
      }
};


/*!
//===========================================================
//
//  CLASS: RegHolderImpl
//
//  DESC: Base type for AutoReger
//
//===========================================================
*/

class RegHolderImpl {
public:
    RegHolderImpl()
      { }

    virtual
    ~RegHolderImpl()
      { }
};

typedef std::auto_ptr< RegHolderImpl > RegHolder;


/*!
//===========================================================
//
//  CLASS: AutoReger
//
//  DESC: Used for automatic registration.
//  NOTE: Auto registration Cannot be used in dynamic libraries
//
//===========================================================
*/

template< typename OF >
class AutoReger
    : public RegHolderImpl {
public:
    typedef OF Factory;
    typedef typename Factory::Creator Creator;
    typedef typename Factory::Index Index;

    AutoReger( Factory & fact,
               Creator creator,
               const Index & idx )
        : M_fact( fact ),
          M_idx( idx )
      {
          M_fact.reg( creator, idx );
      }

    virtual
    ~AutoReger()
      {
          M_fact.dereg( M_idx );
      }

private:
    template< class OF2 >
    AutoReger( const AutoReger< OF2 > & ); // not used

    template< class OF2 >
    AutoReger & operator=( const AutoReger< OF2 > & ); // not used

private:
    Factory & M_fact;
    const Index M_idx;
};


/*!
//===================================================================
//
//  CLASS: Factory
//
//  DESC: An Generic Object Factory (aka Class Store)
//
//===================================================================
*/

template< class Cre,
          class I = const char*,
          class Com = Less< I > >
class Factory {
public:
    typedef Cre Creator;
    typedef I Index;
    typedef Com Compare;
private:
    typedef std::map< Index, std::stack< Creator >, Compare > Map;

public:
    Factory()
      { }

    ~Factory()
      { }

    void reg( Creator c,
              const Index & idx )
      {
          M_creators[ idx ].push( c );
      }

    void dereg( const Index & idx )
      {
          typename Map::iterator i = M_creators.find( idx );
          if ( i != M_creators.end() )
          {
              if ( ! i->second.empty() )
              {
                  i->second.pop();
              }

              if ( i->second.empty() )
              {
                  M_creators.erase( i );
              }
          }
      }

    bool getCreator( Creator & c,
                     const Index & idx )
      {
          typename Map::iterator i = M_creators.find( idx );
          if ( i != M_creators.end()
               && ! i->second.empty() )
          {
              c = i->second.top();
              return true;
          }
          return false;
      }

    std::list< Index > list() const
      {
          std::list< Index > rval;
          for ( typename Map::const_iterator i = M_creators.begin();
                i != M_creators.end();
                ++i )
          {
              rval.push_back( i->first );
          }
          return rval;
      }

    std::ostream & printList( std::ostream & o = std::cout ) const
      {
          for ( typename Map::const_iterator i = M_creators.begin();
                i != M_creators.end();
                ++i )
          {
              o << "\t" << i->first
                << "(" << i->second.size() << ")"
                << std::endl;
          }
          return o;
      }

    size_t size() const
      {
          return M_creators.size();
      }

    size_t size( const Index & idx ) const
      {
          for ( typename Map::const_iterator i = M_creators.begin();
                i != M_creators.end();
                ++i )
          {
              if ( ! Compare()( idx, i->first )
                   && ! Compare()( i->first, idx ) )
              {
                  return i->second.size();
              }
          }
          return 0;
      }

    RegHolder autoReg( Creator c,
                       const Index & i )
      {
          return RegHolder( new AutoReger< Factory< Creator, Index, Compare > >( *this, c, i ) );
      }

private:
    Map M_creators;
};

}

#endif
