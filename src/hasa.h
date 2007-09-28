// -*-c++-*-

/***************************************************************************
                                   hasa.h  
                      Template for a 'Has A' releationship
                             -------------------
    begin                : 12-APR-2002
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

#if 0

#ifndef _HASA_H_
#define _HASA_H_

#include <memory>
#include <list>


namespace rcss
{
  namespace util
  {

    template< typename T >
    class deepCopyClone
    {
    public:
      static
      std::auto_ptr< T >
      clone( const T& t )
      { return t.deepCopy(); }
    };


    template< typename T, template< typename > class CP = deepCopyClone >
    class HasA
    {
    public:
      typedef T Item;
      typedef CP< Item > CloningPolicy;

      void
      set( const Item& item )
      { M_item = item; }

    protected:
      HasA()
      {}

      HasA( const Item& item )
        : M_item( item )
      {}

      ~HasA()
      {}
 
      Item
      get() const
      { return M_item; } 

      Item&
      getRef()
      { return M_item; }

      const Item&
      getRef() const
      { return M_item; }
    private:
      Item M_item;
    };


    template< typename T, template< typename > class CP >
    class HasA< std::auto_ptr< T >, CP >
    {
    public:
      typedef T Item;     
      typedef CP< Item > CloningPolicy;

      void
      set( std::auto_ptr< Item > item )
      { M_item = item; }

    protected:
      HasA()
      {}

      HasA( std::auto_ptr< Item > item )
        : M_item( item )
      {}

      HasA( const HasA< std::auto_ptr< Item > >& ha )
        : M_item( CloningPolicy::clone( *ha.M_item ) )
      {}

      ~HasA()
      {}

      Item*
      get()
      { return M_item.get(); } 

      const Item*
      get() const
      { return M_item.get(); } 

      const Item&
      getRef() const
      { return *M_item; }

      std::auto_ptr< Item >
      detach()
      { return M_item; }
    private:
      std::auto_ptr< Item > M_item;
    };

    
    template< typename T, template< typename > class CP = deepCopyClone >
    class HasMany
    {
    public:
      typedef T Item;
      typedef std::list< Item > Storage;
      typedef CP< Item > CloningPolicy;
 
      void
      set( const Storage& items )
      { M_items = items; }
      
      void
      push_front( const Item& item )
      { M_items.push_front( item ); }
      
      void
      push_back( const Item& item )
      { M_items.push_back( item ); }
      
      void
      clear()
      { M_items.clear(); }
    protected:
      HasMany()
      {}

      HasMany( const Storage& items )
        : M_items( items )
      {}

      ~HasMany()
      {}

      Storage&
      get()
      { return M_items; }

      const Storage&
      get() const
      { return M_items; }
    private:
      Storage M_items;
    };


    template< typename T, template< typename > class CP >
    class HasMany< std::auto_ptr< T >, CP >
    {
    public:
      typedef T Item;
      typedef std::list< Item* > Storage;
      typedef CP< Item > CloningPolicy;

      void
      set( const Storage& items )
      { deleteAll(); M_items = items; }

      void
      push_front( std::auto_ptr< Item > item )
      { M_items.push_front( item.release() ); }

      void
      push_back( std::auto_ptr< Item > item )
      { M_items.push_back( item.release() ); }

      void
      clear()
      { deleteAll(); }

    protected:
      HasMany()
        : M_items()
      {}

      HasMany( const Storage& items )
        : M_items( items )
      {}

      HasMany( const HasMany< std::auto_ptr< Item > >& hm )
        : M_items()
      {
        for( typename Storage::const_iterator i = hm.M_items.begin();
             i != hm.M_items.end(); ++i )
          {
            M_items.push_front( CloningPolicy::clone( **i ).release() );
          }       
      }

      ~HasMany()
      { deleteAll(); }

      HasMany< std::auto_ptr< Item > >&
      operator=( const HasMany< std::auto_ptr< Item > >& vs )
      {
        deleteAll();
        for( typename Storage::const_iterator i = vs.M_items.begin();
             i != vs.M_items.end(); ++i )
          {
            M_items.push_front( CloningPolicy::clone( **i ).release() );
          }
      }

      Storage&
      get()
      { return M_items; }

      const Storage&
      get() const
      { return M_items; }
      
      Storage
      detach()
      {
        Storage rval = M_items;
        M_items.clear();
        return rval;
      }
    private:
      void
      deleteAll()
      {
        for( typename Storage::iterator i = M_items.begin();
             i != M_items.end(); ++i )
          delete *i;
        M_items.clear();
      }

      Storage M_items;
    };



    template< typename T, unsigned int N, template< typename > class CP = deepCopyClone >
    class HasN
    {
    public:
      typedef T Item;
      enum { Size = N };

      void
      set( const Item& item, unsigned int idx )
      { M_items[ idx ] = item; }


    protected:
      HasN()
      {}

      HasN( const Item items[] )
      {
        for( unsigned int i = 0; i < Size; ++i )
          M_items[ i ] = items[ i ];
      }

      HasN( const HasN< Item, Size >& hasn )
      {
        for( unsigned int i = 0; i < Size; ++i )
          M_items[ i ] = hasn.M_items[ i ];
      }

      ~HasN()
      {}

      Item&
      get( unsigned int idx )
      { return M_items[ idx ]; }

      const Item&
      get( unsigned int idx ) const
      { return M_items[ idx ]; }
    private:
      Item M_items[ Size ];
    };


    template< typename T, unsigned int N,  template< typename > class CP >
    class HasN< std::auto_ptr< T >, N, CP >
    {
    public:
      typedef T Item;
      enum { Size = N };
      typedef CP< T > CloningPolicy;

      void
      set( std::auto_ptr< Item > item, unsigned int idx )
      { M_items[ idx ] = item; }

    protected:
      HasN()
      {}

      HasN( std::auto_ptr< Item > items[] )
      {
        for( unsigned int i = 0; i < Size; ++i )
          M_items[ i ] = items[ i ];
      }

      HasN( const HasN< std::auto_ptr< Item >, Size >& hasn )
      {
        for( unsigned int i = 0; i < Size; ++i )
          set( CloningPolicy::clone( *(hasn.M_items[ i ] ) ), i );
      }

       ~HasN()
      {}

      Item*
      get( unsigned int idx )
      { return M_items[ idx ].get(); }

      const Item*
      get( unsigned int idx ) const
      { return M_items[ idx ].get(); }
      
      const Item&
      getRef( unsigned int idx ) const
      { return *( M_items[ idx ].get() ); }

      std::auto_ptr< Item >
      detach( unsigned int idx ) const
      { return M_items[ idx ]; }
    private:
      std::auto_ptr< Item > M_items[ Size ];
    };
  }
}
#endif

#endif
