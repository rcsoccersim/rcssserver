// -*-c++-*-

/***************************************************************************
                                 tuple.h
                      Classes for holding various types
                             -------------------
    begin                : 8-MAR-2002
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

#ifndef _TUPLE_H_
#define _TUPLE_H_


#include <exception>
#include <string>
#include "rcssexceptions.h"   // needed for NullErr
namespace rcss
{
  namespace util
  {
    template< class H, class T >
    class TypeList
    {
    public:
      typedef H HeadType;
      typedef T TailType;
    };

    class NullType
    {};


/*
    class NullErr
      : public std::exception
    {
    protected:
      std::string M_msg;
    public:
      NullErr ( const char* file, const int& line, const char* msg )
      {
        std::ostrstream tmp;
        tmp << file << ": " << line << ": " << msg << std::ends;

        M_msg = tmp.str ();
        tmp.freeze ( false );
      }

      ~NullErr () throw ()
      {
      }

      const char*
      what () const throw ()
      { return M_msg.c_str (); }

    };
*/

    // The generic Tuple class is great for an arbitory number of elements,
    // however, it usage results in may templates being instantiated.  This
    // wouldn't be a problem except that ANSI C++ programs cannot rely on
    // a template instatiation depth greater than 17.  The use of Tuple10
    // has a much lower instatiation depth, so it comes in handy when the
    // depth is greater than 17, plus it's easier to use than the convoluted
    // syntax of the typelists which are needed for the genric Tuple

    template< class T >
    class Tuple;


    template< class H, class T >
    class Tuple< TypeList< H, T > >
      : public Tuple< T >
    {
    private:
      H* M_holder;

    public:

      // I coudn't get template specializations to work properly,
      // so I use dumby parameters instead.
      bool
      isTypeOf( H* ) const
      { return M_holder != 0; }

      template< class X >
      bool
      isTypeOf( X* x ) const
      { return Tuple< T >::isTypeOf( x ); }

      H&
      get( H* )
      {
        if( M_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_holder;
      }

      template< class X >
      X&
      get( X* x )
      { return Tuple< T >::get( x ); }

      const H&
      get( H* ) const
      {
        if( M_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_holder;
      }

      template< class X >
      const X&
      get( X* x ) const
      { return Tuple< T >::get( x ); }

    protected:
      void
      clear()
      { delete M_holder; M_holder = 0; }

    public:
      Tuple()
        : M_holder( 0 )
      {}

      template< class X >
      Tuple( const X& x )
        : Tuple< T >(),
        M_holder( 0 )
      { setValue( x ); }

      Tuple( const Tuple< TypeList< H, T > >& tuple )
        : Tuple< T >(),
        M_holder( 0 )
      { setValue( tuple ); }

      ~Tuple()
      {}

      Tuple< TypeList< H, T > >&
      operator=( const Tuple< TypeList< H, T > >& tuple )
      { return setValue( tuple ); }

      template< class X >
      bool
      isTypeOf() const
      { X* x = NULL; return isTypeOf( x ); }

      template< class X >
      X&
      get()
      { X* x = NULL; return get( x ); }

      template< class X >
      const X&
      get() const
      { X* x = NULL; return get( x ); }

      H&
      setValue( const H& h )
      {
        clearAll();
        M_holder = new H( h );
        return *M_holder;
      }

      template< class X >
      X&
      setValue( const X& x )
      { clear(); return Tuple< T >::setValue( x ); }

      Tuple< TypeList< H, T > >&
      setValue( const Tuple< TypeList< H, T > >& tuple )
      {
        if( tuple.isTypeOf< H >() )
          {
            // no need to clear.  setValue( H ) will do that.
            setValue( tuple.get< H >() );
          }
        else
          {
            clear();
            Tuple< T >::setValue( (const Tuple< T >&)tuple );
          }
        return *this;
      }

      void
      clearAll()
      { clear(); Tuple< T >::clearAll(); }

      template< class X >
      X&
      operator=( const X& x )
      { return setValue( x ); }

    };


    template< class H >
    class Tuple< TypeList< H, NullType > >
    {
    private:
      H* M_holder;

    public:
      bool
      isTypeOf( H* ) const
      { return M_holder != 0; }

      H&
      get( H* )
      {
        if( M_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_holder;
      }

      const H&
      get( H* ) const
      {
        if( M_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_holder;
      }

    protected:
      void
      clear()
      { delete M_holder; M_holder = 0; }

    public:
      Tuple()
        : M_holder( 0 )
      {}

      Tuple( const H& h )
        : M_holder( 0 )
      { setValue( h ); }

      Tuple( const Tuple< TypeList< H, NullType > >& tuple )
        : M_holder( 0 )
      { setValue( tuple ); }

      ~Tuple()
      {}

      Tuple< TypeList< H, NullType > >&
      operator=( const Tuple< TypeList< H, NullType > >& tuple )
      { return setValue( tuple ); }

      template< class X >
      bool
      isTypeOf() const
      { X* x = NULL; return isTypeOf( x ); }

      template< class X >
      X&
      get()
      { X* x = NULL; return get( x ); }

      template< class X >
      const X&
      get() const
      { X* x = NULL; return get( x ); }

      H&
      setValue( const H& h )
      { clear(); M_holder = new H( h ); return *M_holder; }

      Tuple< TypeList< H, NullType > >&
      setValue( const Tuple< TypeList< H, NullType > >& tuple )
      {
        if( tuple.isTypeOf< H >() )
          {
            // no need to clear.  setValue( X ) will do that.
            setValue( tuple.get< H >() );
          }
        else
          clear();
        return *this;
      }

      void
      clearAll()
      { clear(); }

      template< class X >
      X&
      operator=( const X& x )
      { return setValue( x ); }

    };






    class NullType2
    {};

    class NullType3
    {};

    class NullType4
    {};

    class NullType5
    {};

    class NullType6
    {};

    class NullType7
    {};

    class NullType8
    {};

    class NullType9
    {};

    class NullType10
    {};

    template< typename T1 = NullType,
      typename T2 = NullType2,
      typename T3 = NullType3,
      typename T4 = NullType4,
      typename T5 = NullType5,
      typename T6 = NullType6,
      typename T7 = NullType7,
      typename T8 = NullType8,
      typename T9 = NullType9,
      typename T10 = NullType10 >
    class Tuple10
    {
    private:
      T1* M_t1_holder;
      T2* M_t2_holder;
      T3* M_t3_holder;
      T4* M_t4_holder;
      T5* M_t5_holder;
      T6* M_t6_holder;
      T7* M_t7_holder;
      T8* M_t8_holder;
      T9* M_t9_holder;
      T10* M_t10_holder;

    public:
      bool
      isTypeOf( T1* ) const
      { return M_t1_holder != 0; }

      bool
      isTypeOf( T2* ) const
      { return M_t2_holder != 0; }

      bool
      isTypeOf( T3* ) const
      { return M_t3_holder != 0; }

      bool
      isTypeOf( T4* ) const
      { return M_t4_holder != 0; }

      bool
      isTypeOf( T5* ) const
      { return M_t5_holder != 0; }

      bool
      isTypeOf( T6* ) const
      { return M_t6_holder != 0; }

      bool
      isTypeOf( T7* ) const
      { return M_t7_holder != 0; }

      bool
      isTypeOf( T8* ) const
      { return M_t8_holder != 0; }

      bool
      isTypeOf( T9* ) const
      { return M_t9_holder != 0; }

      bool
      isTypeOf( T10* ) const
      { return M_t10_holder != 0; }

      T1&
      get( T1* )
      {
        if( M_t1_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t1_holder;
      }

      T2&
      get( T2* )
      {
        if( M_t2_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t2_holder;
      }

      T3&
      get( T3* )
      {
        if( M_t3_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t3_holder;
      }

      T4&
      get( T4* )
      {
        if( M_t4_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t4_holder;
      }

      T5&
      get( T5* )
      {
        if( M_t5_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t5_holder;
      }

      T6&
      get( T6* )
      {
        if( M_t6_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t6_holder;
      }

      T7&
      get( T7* )
      {
        if( M_t7_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t7_holder;
      }

      T8&
      get( T8* )
      {
        if( M_t8_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t8_holder;
      }

      T9&
      get( T9* )
      {
        if( M_t9_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t9_holder;
      }

      T10&
      get( T10* )
      {
        if( M_t10_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t10_holder;
      }

      const T1&
      get( T1* ) const
      {
        if( M_t1_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t1_holder;
      }

      const T2&
      get( T2* ) const
      {
        if( M_t2_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t2_holder;
      }

      const T3&
      get( T3* ) const
      {
        if( M_t3_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t3_holder;
      }

      const T4&
      get( T4* ) const
      {
        if( M_t4_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t4_holder;
      }

      const T5&
      get( T5* ) const
      {
        if( M_t5_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t5_holder;
      }

      const T6&
      get( T6* ) const
      {
        if( M_t6_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t6_holder;
      }

      const T7&
      get( T7* ) const
      {
        if( M_t7_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t7_holder;
      }

      const T8&
      get( T8* ) const
      {
        if( M_t8_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t8_holder;
      }

      const T9&
      get( T9* ) const
      {
        if( M_t9_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t9_holder;
      }

      const T10&
      get( T10* ) const
      {
        if( M_t10_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t10_holder;
      }

    protected:

      void
      clear()
      {
        delete M_t1_holder; M_t1_holder = 0;
        delete M_t2_holder; M_t2_holder = 0;
        delete M_t3_holder; M_t3_holder = 0;
        delete M_t4_holder; M_t4_holder = 0;
        delete M_t5_holder; M_t5_holder = 0;
        delete M_t6_holder; M_t6_holder = 0;
        delete M_t7_holder; M_t7_holder = 0;
        delete M_t8_holder; M_t8_holder = 0;
        delete M_t9_holder; M_t9_holder = 0;
        delete M_t10_holder; M_t10_holder = 0;
      }

    public:
      Tuple10()
        : M_t1_holder( 0 ),
          M_t2_holder( 0 ),
          M_t3_holder( 0 ),
          M_t4_holder( 0 ),
          M_t5_holder( 0 ),
          M_t6_holder( 0 ),
          M_t7_holder( 0 ),
          M_t8_holder( 0 ),
          M_t9_holder( 0 ),
          M_t10_holder( 0 )
      {}

      template< typename T >
      Tuple10( const T& t )
        : M_t1_holder( 0 ),
          M_t2_holder( 0 ),
          M_t3_holder( 0 ),
          M_t4_holder( 0 ),
          M_t5_holder( 0 ),
          M_t6_holder( 0 ),
          M_t7_holder( 0 ),
          M_t8_holder( 0 ),
          M_t9_holder( 0 ),
          M_t10_holder( 0 )
      { setValue( t ); }

      Tuple10( const Tuple10< T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 >& tuple )
        : M_t1_holder( 0 ),
          M_t2_holder( 0 ),
          M_t3_holder( 0 ),
          M_t4_holder( 0 ),
          M_t5_holder( 0 ),
          M_t6_holder( 0 ),
          M_t7_holder( 0 ),
          M_t8_holder( 0 ),
          M_t9_holder( 0 ),
          M_t10_holder( 0 )
      { setValue( tuple ); }

      ~Tuple10()
      {}

      Tuple10< T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 >&
      operator=( const Tuple10< T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 >& tuple )
      { return setValue( tuple ); }

      template< class T >
      bool
      isTypeOf() const
      { T* t = NULL; return isTypeOf( t ); }

      template< class T >
      T&
      get()
      { T* t = NULL; return get( t ); }

      template< class T >
      const T&
      get() const
      { T* t = NULL; return get( t ); }

      T1&
      setValue( const T1& t1 )
      { clear(); M_t1_holder = new T1( t1 ); return *M_t1_holder; }

      T2&
      setValue( const T2& t2 )
      { clear(); M_t2_holder = new T2( t2 ); return *M_t2_holder; }

      T3&
      setValue( const T3& t3 )
      { clear(); M_t3_holder = new T3( t3 ); return *M_t3_holder; }

      T4&
      setValue( const T4& t4 )
      { clear(); M_t4_holder = new T4( t4 ); return *M_t4_holder; }

      T5&
      setValue( const T5& t5 )
      { clear(); M_t5_holder = new T5( t5 ); return *M_t5_holder; }

      T6&
      setValue( const T6& t6 )
      { clear(); M_t6_holder = new T6( t6 ); return *M_t6_holder; }

      T7&
      setValue( const T7& t7 )
      { clear(); M_t7_holder = new T7( t7 ); return *M_t7_holder; }

      T8&
      setValue( const T8& t8 )
      { clear(); M_t8_holder = new T8( t8 ); return *M_t8_holder; }

      T9&
      setValue( const T9& t9 )
      { clear(); M_t9_holder = new T9( t9 ); return *M_t9_holder; }

      T10&
      setValue( const T10& t10 )
      { clear(); M_t10_holder = new T10( t10 ); return *M_t10_holder; }

      Tuple10< T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 >&
      setValue( const Tuple10< T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 >& tuple )
      {
        if( tuple.template isTypeOf< T1 >() )
          setValue( tuple.template get< T1 >() );
        else if( tuple.template isTypeOf< T2 >() )
          setValue( tuple.template get< T2 >() );
        else if( tuple.template isTypeOf< T3 >() )
          setValue( tuple.template get< T3 >() );
        else if( tuple.template isTypeOf< T4 >() )
          setValue( tuple.template get< T4 >() );
        else if( tuple.template isTypeOf< T5 >() )
          setValue( tuple.template get< T5 >() );
        else if( tuple.template isTypeOf< T6 >() )
          setValue( tuple.template get< T6 >() );
        else if( tuple.template isTypeOf< T7 >() )
          setValue( tuple.template get< T7 >() );
        else if( tuple.template isTypeOf< T8 >() )
          setValue( tuple.template get< T8 >() );
        else if( tuple.template isTypeOf< T9 >() )
          setValue( tuple.template get< T9 >() );
        else if( tuple.template isTypeOf< T10 >() )
          setValue( tuple.template get< T10 >() );
        else
          clear();
        return *this;
      }

      void
      clearAll()
      { clear(); }

      template< class X >
      X&
      operator=( const X& x )
      { return setValue( x ); }
    };


    class NullType11
    {};
    class NullType12
    {};
    class NullType13
    {};
    class NullType14
    {};
    class NullType15
    {};
    class NullType16
    {};
    class NullType17
    {};
    class NullType18
    {};
    class NullType19
    {};
    class NullType20
    {};

    template< typename T1 = NullType,
      typename T2 = NullType2,
      typename T3 = NullType3,
      typename T4 = NullType4,
      typename T5 = NullType5,
      typename T6 = NullType6,
      typename T7 = NullType7,
      typename T8 = NullType8,
      typename T9 = NullType9,
      typename T10 = NullType10,
      typename T11 = NullType11,
      typename T12 = NullType12,
      typename T13 = NullType13,
      typename T14 = NullType14,
      typename T15 = NullType15,
      typename T16 = NullType16,
      typename T17 = NullType17,
      typename T18 = NullType18,
      typename T19 = NullType19,
      typename T20 = NullType20 >
    class Tuple20
    {
    private:
      typedef Tuple20< T1, T2, T3, T4, T5, T6, T7, T8, T9, T10,
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 > ThisType;

      T1* M_t1_holder;
      T2* M_t2_holder;
      T3* M_t3_holder;
      T4* M_t4_holder;
      T5* M_t5_holder;
      T6* M_t6_holder;
      T7* M_t7_holder;
      T8* M_t8_holder;
      T9* M_t9_holder;
      T10* M_t10_holder;
      T11* M_t11_holder;
      T12* M_t12_holder;
      T13* M_t13_holder;
      T14* M_t14_holder;
      T15* M_t15_holder;
      T16* M_t16_holder;
      T17* M_t17_holder;
      T18* M_t18_holder;
      T19* M_t19_holder;
      T20* M_t20_holder;

    public:
      bool
      isTypeOf( T1* ) const
      { return M_t1_holder != 0; }

      bool
      isTypeOf( T2* ) const
      { return M_t2_holder != 0; }

      bool
      isTypeOf( T3* ) const
      { return M_t3_holder != 0; }

      bool
      isTypeOf( T4* ) const
      { return M_t4_holder != 0; }

      bool
      isTypeOf( T5* ) const
      { return M_t5_holder != 0; }

      bool
      isTypeOf( T6* ) const
      { return M_t6_holder != 0; }

      bool
      isTypeOf( T7* ) const
      { return M_t7_holder != 0; }

      bool
      isTypeOf( T8* ) const
      { return M_t8_holder != 0; }

      bool
      isTypeOf( T9* ) const
      { return M_t9_holder != 0; }

      bool
      isTypeOf( T10* ) const
      { return M_t10_holder != 0; }

      bool
      isTypeOf( T11* ) const
      { return M_t11_holder != 0; }

      bool
      isTypeOf( T12* ) const
      { return M_t12_holder != 0; }

      bool
      isTypeOf( T13* ) const
      { return M_t13_holder != 0; }

      bool
      isTypeOf( T14* ) const
      { return M_t14_holder != 0; }

      bool
      isTypeOf( T15* ) const
      { return M_t15_holder != 0; }

      bool
      isTypeOf( T16* ) const
      { return M_t16_holder != 0; }

      bool
      isTypeOf( T17* ) const
      { return M_t17_holder != 0; }

      bool
      isTypeOf( T18* ) const
      { return M_t18_holder != 0; }

      bool
      isTypeOf( T19* ) const
      { return M_t19_holder != 0; }

      bool
      isTypeOf( T20* ) const
      { return M_t20_holder != 0; }

      T1&
      get( T1* )
      {
        if( M_t1_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t1_holder;
      }

      T2&
      get( T2* )
      {
        if( M_t2_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t2_holder;
      }

      T3&
      get( T3* )
      {
        if( M_t3_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t3_holder;
      }

      T4&
      get( T4* )
      {
        if( M_t4_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t4_holder;
      }

      T5&
      get( T5* )
      {
        if( M_t5_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t5_holder;
      }

      T6&
      get( T6* )
      {
        if( M_t6_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t6_holder;
      }

      T7&
      get( T7* )
      {
        if( M_t7_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t7_holder;
      }

      T8&
      get( T8* )
      {
        if( M_t8_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t8_holder;
      }

      T9&
      get( T9* )
      {
        if( M_t9_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t9_holder;
      }

      T10&
      get( T10* )
      {
        if( M_t10_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t10_holder;
      }

      T11&
      get( T11* )
      {
        if( M_t11_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t11_holder;
      }

      T12&
      get( T12* )
      {
        if( M_t12_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t12_holder;
      }

      T13&
      get( T13* )
      {
        if( M_t13_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t13_holder;
      }

      T14&
      get( T14* )
      {
        if( M_t14_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t14_holder;
      }

      T15&
      get( T15* )
      {
        if( M_t15_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t15_holder;
      }

      T16&
      get( T16* )
      {
        if( M_t16_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t16_holder;
      }

      T17&
      get( T17* )
      {
        if( M_t17_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t17_holder;
      }

      T18&
      get( T18* )
      {
        if( M_t18_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t18_holder;
      }

      T19&
      get( T19* )
      {
        if( M_t19_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t19_holder;
      }

      T20&
      get( T20* )
      {
        if( M_t20_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t20_holder;
      }

      const T1&
      get( T1* ) const
      {
        if( M_t1_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t1_holder;
      }

      const T2&
      get( T2* ) const
      {
        if( M_t2_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t2_holder;
      }

      const T3&
      get( T3* ) const
      {
        if( M_t3_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t3_holder;
      }

      const T4&
      get( T4* ) const
      {
        if( M_t4_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t4_holder;
      }

      const T5&
      get( T5* ) const
      {
        if( M_t5_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t5_holder;
      }

      const T6&
      get( T6* ) const
      {
        if( M_t6_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t6_holder;
      }

      const T7&
      get( T7* ) const
      {
        if( M_t7_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t7_holder;
      }

      const T8&
      get( T8* ) const
      {
        if( M_t8_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t8_holder;
      }

      const T9&
      get( T9* ) const
      {
        if( M_t9_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t9_holder;
      }

      const T10&
      get( T10* ) const
      {
        if( M_t10_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t10_holder;
      }

      const T11&
      get( T11* ) const
      {
        if( M_t11_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t11_holder;
      }

      const T12&
      get( T12* ) const
      {
        if( M_t12_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t12_holder;
      }

      const T13&
      get( T13* ) const
      {
        if( M_t13_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t13_holder;
      }

      const T14&
      get( T14* ) const
      {
        if( M_t14_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t14_holder;
      }

      const T15&
      get( T15* ) const
      {
        if( M_t15_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t15_holder;
      }

      const T16&
      get( T16* ) const
      {
        if( M_t16_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t16_holder;
      }

      const T17&
      get( T17* ) const
      {
        if( M_t17_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t17_holder;
      }

      const T18&
      get( T18* ) const
      {
        if( M_t18_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t18_holder;
      }

      const T19&
      get( T19* ) const
      {
        if( M_t19_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t19_holder;
      }

      const T20&
      get( T20* ) const
      {
        if( M_t20_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t20_holder;
      }

    protected:
      void
      clear()
      {
        delete M_t1_holder; M_t1_holder = 0;
        delete M_t2_holder; M_t2_holder = 0;
        delete M_t3_holder; M_t3_holder = 0;
        delete M_t4_holder; M_t4_holder = 0;
        delete M_t5_holder; M_t5_holder = 0;
        delete M_t6_holder; M_t6_holder = 0;
        delete M_t7_holder; M_t7_holder = 0;
        delete M_t8_holder; M_t8_holder = 0;
        delete M_t9_holder; M_t9_holder = 0;
        delete M_t10_holder; M_t10_holder = 0;
        delete M_t11_holder; M_t11_holder = 0;
        delete M_t12_holder; M_t12_holder = 0;
        delete M_t13_holder; M_t13_holder = 0;
        delete M_t14_holder; M_t14_holder = 0;
        delete M_t15_holder; M_t15_holder = 0;
        delete M_t16_holder; M_t16_holder = 0;
        delete M_t17_holder; M_t17_holder = 0;
        delete M_t18_holder; M_t18_holder = 0;
        delete M_t19_holder; M_t19_holder = 0;
        delete M_t20_holder; M_t20_holder = 0;
      }

    public:
      Tuple20()
        : M_t1_holder( 0 ),
          M_t2_holder( 0 ),
          M_t3_holder( 0 ),
          M_t4_holder( 0 ),
          M_t5_holder( 0 ),
          M_t6_holder( 0 ),
          M_t7_holder( 0 ),
          M_t8_holder( 0 ),
          M_t9_holder( 0 ),
          M_t10_holder( 0 ),
          M_t11_holder( 0 ),
          M_t12_holder( 0 ),
          M_t13_holder( 0 ),
          M_t14_holder( 0 ),
          M_t15_holder( 0 ),
          M_t16_holder( 0 ),
          M_t17_holder( 0 ),
          M_t18_holder( 0 ),
          M_t19_holder( 0 ),
          M_t20_holder( 0 )
      {}

      template< typename T >
      Tuple20( const T& t )
        : M_t1_holder( 0 ),
          M_t2_holder( 0 ),
          M_t3_holder( 0 ),
          M_t4_holder( 0 ),
          M_t5_holder( 0 ),
          M_t6_holder( 0 ),
          M_t7_holder( 0 ),
          M_t8_holder( 0 ),
          M_t9_holder( 0 ),
          M_t10_holder( 0 ),
          M_t11_holder( 0 ),
          M_t12_holder( 0 ),
          M_t13_holder( 0 ),
          M_t14_holder( 0 ),
          M_t15_holder( 0 ),
          M_t16_holder( 0 ),
          M_t17_holder( 0 ),
          M_t18_holder( 0 ),
          M_t19_holder( 0 ),
          M_t20_holder( 0 )
      { setValue( t ); }

      Tuple20( const ThisType& tuple )
        : M_t1_holder( 0 ),
          M_t2_holder( 0 ),
          M_t3_holder( 0 ),
          M_t4_holder( 0 ),
          M_t5_holder( 0 ),
          M_t6_holder( 0 ),
          M_t7_holder( 0 ),
          M_t8_holder( 0 ),
          M_t9_holder( 0 ),
          M_t10_holder( 0 ),
          M_t11_holder( 0 ),
          M_t12_holder( 0 ),
          M_t13_holder( 0 ),
          M_t14_holder( 0 ),
          M_t15_holder( 0 ),
          M_t16_holder( 0 ),
          M_t17_holder( 0 ),
          M_t18_holder( 0 ),
          M_t19_holder( 0 ),
          M_t20_holder( 0 )
      { setValue( tuple ); }

      ~Tuple20()
      {}

      ThisType&
      operator=( const ThisType& tuple )
      { return setValue( tuple ); }

      template< class T >
      bool
      isTypeOf() const
      { T* t = NULL; return isTypeOf( t ); }

      template< class T >
      T&
      get()
      { T* t = NULL; return get( t ); }

      template< class T >
      const T&
      get() const
      { T* t = NULL; return get( t ); }

      T1&
      setValue( const T1& t )
      { clear(); M_t1_holder = new T1( t ); return *M_t1_holder; }

      T2&
      setValue( const T2& t )
      { clear(); M_t2_holder = new T2( t ); return *M_t2_holder; }

      T3&
      setValue( const T3& t )
      { clear(); M_t3_holder = new T3( t ); return *M_t3_holder; }

      T4&
      setValue( const T4& t )
      { clear(); M_t4_holder = new T4( t ); return *M_t4_holder; }

      T5&
      setValue( const T5& t )
      { clear(); M_t5_holder = new T5( t ); return *M_t5_holder; }

      T6&
      setValue( const T6& t )
      { clear(); M_t6_holder = new T6( t ); return *M_t6_holder; }

      T7&
      setValue( const T7& t )
      { clear(); M_t7_holder = new T7( t ); return *M_t7_holder; }

      T8&
      setValue( const T8& t )
      { clear(); M_t8_holder = new T8( t ); return *M_t8_holder; }

      T9&
      setValue( const T9& t )
      { clear(); M_t9_holder = new T9( t ); return *M_t9_holder; }

      T10&
      setValue( const T10& t )
      { clear(); M_t10_holder = new T10( t ); return *M_t10_holder; }

      T11&
      setValue( const T11& t )
      { clear(); M_t11_holder = new T11( t ); return *M_t11_holder; }

      T12&
      setValue( const T12& t )
      { clear(); M_t12_holder = new T12( t ); return *M_t12_holder; }

      T13&
      setValue( const T13& t )
      { clear(); M_t13_holder = new T13( t ); return *M_t13_holder; }

      T14&
      setValue( const T14& t )
      { clear(); M_t14_holder = new T14( t ); return *M_t14_holder; }

      T15&
      setValue( const T15& t )
      { clear(); M_t15_holder = new T15( t ); return *M_t15_holder; }

      T16&
      setValue( const T16& t )
      { clear(); M_t16_holder = new T16( t ); return *M_t16_holder; }

      T17&
      setValue( const T17& t )
      { clear(); M_t17_holder = new T17( t ); return *M_t17_holder; }

      T18&
      setValue( const T18& t )
      { clear(); M_t18_holder = new T18( t ); return *M_t18_holder; }

      T19&
      setValue( const T19& t )
      { clear(); M_t19_holder = new T19( t ); return *M_t19_holder; }

      T20&
      setValue( const T20& t )
      { clear(); M_t20_holder = new T20( t ); return *M_t20_holder; }

      ThisType&
      setValue( const ThisType& tuple )
      {
        T1* t1 = 0;
        T2* t2 = 0;
        T3* t3 = 0;
        T4* t4 = 0;
        T5* t5 = 0;
        T6* t6 = 0;
        T7* t7 = 0;
        T8* t8 = 0;
        T9* t9 = 0;
        T10* t10 = 0;
        T11* t11 = 0;
        T12* t12 = 0;
        T13* t13 = 0;
        T14* t14 = 0;
        T15* t15 = 0;
        T16* t16 = 0;
        T17* t17 = 0;
        T18* t18 = 0;
        T19* t19 = 0;
        T20* t20 = 0;
        if( tuple.isTypeOf( t1 ) )
          setValue( tuple.get( t1 ) );
        else if( tuple.isTypeOf( t2 ) )
          setValue( tuple.get( t2 ) );
        else if( tuple.isTypeOf( t3 ) )
          setValue( tuple.get( t3 ) );
        else if( tuple.isTypeOf( t4 ) )
          setValue( tuple.get( t4 ) );
        else if( tuple.isTypeOf( t5 ) )
          setValue( tuple.get( t5 ) );
        else if( tuple.isTypeOf( t6 ) )
          setValue( tuple.get( t6 ) );
        else if( tuple.isTypeOf( t7 ) )
          setValue( tuple.get( t7 ) );
        else if( tuple.isTypeOf( t8 ) )
          setValue( tuple.get( t8 ) );
        else if( tuple.isTypeOf( t9 ) )
          setValue( tuple.get( t9 ) );
        else if( tuple.isTypeOf( t10 ) )
          setValue( tuple.get( t10 ) );
        else if( tuple.isTypeOf( t11 ) )
          setValue( tuple.get( t11 ) );
        else if( tuple.isTypeOf( t12 ) )
          setValue( tuple.get( t12 ) );
        else if( tuple.isTypeOf( t13 ) )
          setValue( tuple.get( t13 ) );
        else if( tuple.isTypeOf( t14 ) )
          setValue( tuple.get( t14 ) );
        else if( tuple.isTypeOf( t15 ) )
          setValue( tuple.get( t15 ) );
        else if( tuple.isTypeOf( t16 ) )
          setValue( tuple.get( t16 ) );
        else if( tuple.isTypeOf( t17 ) )
          setValue( tuple.get( t17 ) );
        else if( tuple.isTypeOf( t18 ) )
          setValue( tuple.get( t18 ) );
        else if( tuple.isTypeOf( t19 ) )
          setValue( tuple.get( t19 ) );
        else if( tuple.isTypeOf( t20 ) )
          setValue( tuple.get( t20 ) );
        else
          clear();
        return *this;
      }

      void
      clearAll()
      { clear(); }

      template< class X >
      X&
      operator=( const X& x )
      { return setValue( x ); }
    };

    class NullType21
    {};
    class NullType22
    {};
    class NullType23
    {};
    class NullType24
    {};
    class NullType25
    {};
    class NullType26
    {};
    class NullType27
    {};
    class NullType28
    {};
    class NullType29
    {};
    class NullType30
    {};

    template< typename T1 = NullType,
      typename T2 = NullType2,
      typename T3 = NullType3,
      typename T4 = NullType4,
      typename T5 = NullType5,
      typename T6 = NullType6,
      typename T7 = NullType7,
      typename T8 = NullType8,
      typename T9 = NullType9,
      typename T10 = NullType10,
      typename T11 = NullType11,
      typename T12 = NullType12,
      typename T13 = NullType13,
      typename T14 = NullType14,
      typename T15 = NullType15,
      typename T16 = NullType16,
      typename T17 = NullType17,
      typename T18 = NullType18,
      typename T19 = NullType19,
      typename T20 = NullType20,
      typename T21 = NullType21,
      typename T22 = NullType22,
      typename T23 = NullType23,
      typename T24 = NullType24,
      typename T25 = NullType25,
      typename T26 = NullType26,
      typename T27 = NullType27,
      typename T28 = NullType28,
      typename T29 = NullType29,
      typename T30 = NullType30 >
    class Tuple30
    {
    private:
      typedef Tuple30< T1, T2, T3, T4, T5, T6, T7, T8, T9, T10,
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20,
        T21, T22, T23, T24, T25, T26, T27, T28, T29, T30 > ThisType;

      T1* M_t1_holder;
      T2* M_t2_holder;
      T3* M_t3_holder;
      T4* M_t4_holder;
      T5* M_t5_holder;
      T6* M_t6_holder;
      T7* M_t7_holder;
      T8* M_t8_holder;
      T9* M_t9_holder;
      T10* M_t10_holder;
      T11* M_t11_holder;
      T12* M_t12_holder;
      T13* M_t13_holder;
      T14* M_t14_holder;
      T15* M_t15_holder;
      T16* M_t16_holder;
      T17* M_t17_holder;
      T18* M_t18_holder;
      T19* M_t19_holder;
      T20* M_t20_holder;
      T21* M_t21_holder;
      T22* M_t22_holder;
      T23* M_t23_holder;
      T24* M_t24_holder;
      T25* M_t25_holder;
      T26* M_t26_holder;
      T27* M_t27_holder;
      T28* M_t28_holder;
      T29* M_t29_holder;
      T30* M_t30_holder;

    public:
      bool
      isTypeOf( T1* ) const
      { return M_t1_holder != 0; }

      bool
      isTypeOf( T2* ) const
      { return M_t2_holder != 0; }

      bool
      isTypeOf( T3* ) const
      { return M_t3_holder != 0; }

      bool
      isTypeOf( T4* ) const
      { return M_t4_holder != 0; }

      bool
      isTypeOf( T5* ) const
      { return M_t5_holder != 0; }

      bool
      isTypeOf( T6* ) const
      { return M_t6_holder != 0; }

      bool
      isTypeOf( T7* ) const
      { return M_t7_holder != 0; }

      bool
      isTypeOf( T8* ) const
      { return M_t8_holder != 0; }

      bool
      isTypeOf( T9* ) const
      { return M_t9_holder != 0; }

      bool
      isTypeOf( T10* ) const
      { return M_t10_holder != 0; }

      bool
      isTypeOf( T11* ) const
      { return M_t11_holder != 0; }

      bool
      isTypeOf( T12* ) const
      { return M_t12_holder != 0; }

      bool
      isTypeOf( T13* ) const
      { return M_t13_holder != 0; }

      bool
      isTypeOf( T14* ) const
      { return M_t14_holder != 0; }

      bool
      isTypeOf( T15* ) const
      { return M_t15_holder != 0; }

      bool
      isTypeOf( T16* ) const
      { return M_t16_holder != 0; }

      bool
      isTypeOf( T17* ) const
      { return M_t17_holder != 0; }

      bool
      isTypeOf( T18* ) const
      { return M_t18_holder != 0; }

      bool
      isTypeOf( T19* ) const
      { return M_t19_holder != 0; }

      bool
      isTypeOf( T20* ) const
      { return M_t20_holder != 0; }

      bool
      isTypeOf( T21* ) const
      { return M_t21_holder != 0; }

      bool
      isTypeOf( T22* ) const
      { return M_t22_holder != 0; }

      bool
      isTypeOf( T23* ) const
      { return M_t23_holder != 0; }

      bool
      isTypeOf( T24* ) const
      { return M_t24_holder != 0; }

      bool
      isTypeOf( T25* ) const
      { return M_t25_holder != 0; }

      bool
      isTypeOf( T26* ) const
      { return M_t26_holder != 0; }

      bool
      isTypeOf( T27* ) const
      { return M_t27_holder != 0; }

      bool
      isTypeOf( T28* ) const
      { return M_t28_holder != 0; }

      bool
      isTypeOf( T29* ) const
      { return M_t29_holder != 0; }

      bool
      isTypeOf( T30* ) const
      { return M_t30_holder != 0; }

      T1&
      get( T1* )
      {
        if( M_t1_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t1_holder;
      }

      T2&
      get( T2* )
      {
        if( M_t2_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t2_holder;
      }

      T3&
      get( T3* )
      {
        if( M_t3_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t3_holder;
      }

      T4&
      get( T4* )
      {
        if( M_t4_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t4_holder;
      }

      T5&
      get( T5* )
      {
        if( M_t5_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t5_holder;
      }

      T6&
      get( T6* )
      {
        if( M_t6_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t6_holder;
      }

      T7&
      get( T7* )
      {
        if( M_t7_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t7_holder;
      }

      T8&
      get( T8* )
      {
        if( M_t8_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t8_holder;
      }

      T9&
      get( T9* )
      {
        if( M_t9_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t9_holder;
      }

      T10&
      get( T10* )
      {
        if( M_t10_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t10_holder;
      }

      T11&
      get( T11* )
      {
        if( M_t11_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t11_holder;
      }

      T12&
      get( T12* )
      {
        if( M_t12_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t12_holder;
      }

      T13&
      get( T13* )
      {
        if( M_t13_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t13_holder;
      }

      T14&
      get( T14* )
      {
        if( M_t14_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t14_holder;
      }

      T15&
      get( T15* )
      {
        if( M_t15_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t15_holder;
      }

      T16&
      get( T16* )
      {
        if( M_t16_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t16_holder;
      }

      T17&
      get( T17* )
      {
        if( M_t17_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t17_holder;
      }

      T18&
      get( T18* )
      {
        if( M_t18_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t18_holder;
      }

      T19&
      get( T19* )
      {
        if( M_t19_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t19_holder;
      }

      T20&
      get( T20* )
      {
        if( M_t20_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t20_holder;
      }

      T21&
      get( T21* )
      {
        if( M_t21_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t21_holder;
      }

      T22&
      get( T22* )
      {
        if( M_t22_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t22_holder;
      }

      T23&
      get( T23* )
      {
        if( M_t23_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t23_holder;
      }

      T24&
      get( T24* )
      {
        if( M_t24_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t24_holder;
      }

      T25&
      get( T25* )
      {
        if( M_t25_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t25_holder;
      }

      T26&
      get( T26* )
      {
        if( M_t26_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t26_holder;
      }

      T27&
      get( T27* )
      {
        if( M_t27_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t27_holder;
      }

      T28&
      get( T28* )
      {
        if( M_t28_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t28_holder;
      }

      T29&
      get( T29* )
      {
        if( M_t29_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t29_holder;
      }

      T30&
      get( T30* )
      {
        if( M_t30_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t30_holder;
      }

      const T1&
      get( T1* ) const
      {
        if( M_t1_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t1_holder;
      }

      const T2&
      get( T2* ) const
      {
        if( M_t2_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t2_holder;
      }

      const T3&
      get( T3* ) const
      {
        if( M_t3_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t3_holder;
      }

      const T4&
      get( T4* ) const
      {
        if( M_t4_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t4_holder;
      }

      const T5&
      get( T5* ) const
      {
        if( M_t5_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t5_holder;
      }

      const T6&
      get( T6* ) const
      {
        if( M_t6_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t6_holder;
      }

      const T7&
      get( T7* ) const
      {
        if( M_t7_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t7_holder;
      }

      const T8&
      get( T8* ) const
      {
        if( M_t8_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t8_holder;
      }

      const T9&
      get( T9* ) const
      {
        if( M_t9_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t9_holder;
      }

      const T10&
      get( T10* ) const
      {
        if( M_t10_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t10_holder;
      }

      const T11&
      get( T11* ) const
      {
        if( M_t11_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t11_holder;
      }

      const T12&
      get( T12* ) const
      {
        if( M_t12_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t12_holder;
      }

      const T13&
      get( T13* ) const
      {
        if( M_t13_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t13_holder;
      }

      const T14&
      get( T14* ) const
      {
        if( M_t14_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t14_holder;
      }

      const T15&
      get( T15* ) const
      {
        if( M_t15_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t15_holder;
      }

      const T16&
      get( T16* ) const
      {
        if( M_t16_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t16_holder;
      }

      const T17&
      get( T17* ) const
      {
        if( M_t17_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t17_holder;
      }

      const T18&
      get( T18* ) const
      {
        if( M_t18_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t18_holder;
      }

      const T19&
      get( T19* ) const
      {
        if( M_t19_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t19_holder;
      }

      const T20&
      get( T20* ) const
      {
        if( M_t20_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t20_holder;
      }

      const T21&
      get( T21* ) const
      {
        if( M_t21_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t21_holder;
      }

      const T22&
      get( T22* ) const
      {
        if( M_t22_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t22_holder;
      }

      const T23&
      get( T23* ) const
      {
        if( M_t23_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t23_holder;
      }

      const T24&
      get( T24* ) const
      {
        if( M_t24_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t24_holder;
      }

      const T25&
      get( T25* ) const
      {
        if( M_t25_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t25_holder;
      }

      const T26&
      get( T26* ) const
      {
        if( M_t26_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t26_holder;
      }

      const T27&
      get( T27* ) const
      {
        if( M_t27_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t27_holder;
      }

      const T28&
      get( T28* ) const
      {
        if( M_t28_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t28_holder;
      }

      const T29&
      get( T29* ) const
      {
        if( M_t29_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t29_holder;
      }

      const T30&
      get( T30* ) const
      {
        if( M_t30_holder == 0 )
          throw NullErr( __FILE__, __LINE__,
                         "Tried to access null element in Tuple" );
        else
          return *M_t30_holder;
      }

    protected:
      void
      clear()
      {
        delete M_t1_holder; M_t1_holder = 0;
        delete M_t2_holder; M_t2_holder = 0;
        delete M_t3_holder; M_t3_holder = 0;
        delete M_t4_holder; M_t4_holder = 0;
        delete M_t5_holder; M_t5_holder = 0;
        delete M_t6_holder; M_t6_holder = 0;
        delete M_t7_holder; M_t7_holder = 0;
        delete M_t8_holder; M_t8_holder = 0;
        delete M_t9_holder; M_t9_holder = 0;
        delete M_t10_holder; M_t10_holder = 0;
        delete M_t11_holder; M_t11_holder = 0;
        delete M_t12_holder; M_t12_holder = 0;
        delete M_t13_holder; M_t13_holder = 0;
        delete M_t14_holder; M_t14_holder = 0;
        delete M_t15_holder; M_t15_holder = 0;
        delete M_t16_holder; M_t16_holder = 0;
        delete M_t17_holder; M_t17_holder = 0;
        delete M_t18_holder; M_t18_holder = 0;
        delete M_t19_holder; M_t19_holder = 0;
        delete M_t20_holder; M_t20_holder = 0;
        delete M_t21_holder; M_t21_holder = 0;
        delete M_t22_holder; M_t22_holder = 0;
        delete M_t23_holder; M_t23_holder = 0;
        delete M_t24_holder; M_t24_holder = 0;
        delete M_t25_holder; M_t25_holder = 0;
        delete M_t26_holder; M_t26_holder = 0;
        delete M_t27_holder; M_t27_holder = 0;
        delete M_t28_holder; M_t28_holder = 0;
        delete M_t29_holder; M_t29_holder = 0;
        delete M_t30_holder; M_t30_holder = 0;
      }

    public:
      Tuple30()
        : M_t1_holder( 0 ),
          M_t2_holder( 0 ),
          M_t3_holder( 0 ),
          M_t4_holder( 0 ),
          M_t5_holder( 0 ),
          M_t6_holder( 0 ),
          M_t7_holder( 0 ),
          M_t8_holder( 0 ),
          M_t9_holder( 0 ),
          M_t10_holder( 0 ),
          M_t11_holder( 0 ),
          M_t12_holder( 0 ),
          M_t13_holder( 0 ),
          M_t14_holder( 0 ),
          M_t15_holder( 0 ),
          M_t16_holder( 0 ),
          M_t17_holder( 0 ),
          M_t18_holder( 0 ),
          M_t19_holder( 0 ),
          M_t20_holder( 0 ),
          M_t21_holder( 0 ),
          M_t22_holder( 0 ),
          M_t23_holder( 0 ),
          M_t24_holder( 0 ),
          M_t25_holder( 0 ),
          M_t26_holder( 0 ),
          M_t27_holder( 0 ),
          M_t28_holder( 0 ),
          M_t29_holder( 0 ),
          M_t30_holder( 0 )
      {}

      template< typename T >
      Tuple30( const T& t )
        : M_t1_holder( 0 ),
          M_t2_holder( 0 ),
          M_t3_holder( 0 ),
          M_t4_holder( 0 ),
          M_t5_holder( 0 ),
          M_t6_holder( 0 ),
          M_t7_holder( 0 ),
          M_t8_holder( 0 ),
          M_t9_holder( 0 ),
          M_t10_holder( 0 ),
          M_t11_holder( 0 ),
          M_t12_holder( 0 ),
          M_t13_holder( 0 ),
          M_t14_holder( 0 ),
          M_t15_holder( 0 ),
          M_t16_holder( 0 ),
          M_t17_holder( 0 ),
          M_t18_holder( 0 ),
          M_t19_holder( 0 ),
          M_t20_holder( 0 ),
          M_t21_holder( 0 ),
          M_t22_holder( 0 ),
          M_t23_holder( 0 ),
          M_t24_holder( 0 ),
          M_t25_holder( 0 ),
          M_t26_holder( 0 ),
          M_t27_holder( 0 ),
          M_t28_holder( 0 ),
          M_t29_holder( 0 ),
          M_t30_holder( 0 )
      { setValue( t ); }

      Tuple30( const ThisType& tuple )
        : M_t1_holder( 0 ),
          M_t2_holder( 0 ),
          M_t3_holder( 0 ),
          M_t4_holder( 0 ),
          M_t5_holder( 0 ),
          M_t6_holder( 0 ),
          M_t7_holder( 0 ),
          M_t8_holder( 0 ),
          M_t9_holder( 0 ),
          M_t10_holder( 0 ),
          M_t11_holder( 0 ),
          M_t12_holder( 0 ),
          M_t13_holder( 0 ),
          M_t14_holder( 0 ),
          M_t15_holder( 0 ),
          M_t16_holder( 0 ),
          M_t17_holder( 0 ),
          M_t18_holder( 0 ),
          M_t19_holder( 0 ),
          M_t20_holder( 0 ),
          M_t21_holder( 0 ),
          M_t22_holder( 0 ),
          M_t23_holder( 0 ),
          M_t24_holder( 0 ),
          M_t25_holder( 0 ),
          M_t26_holder( 0 ),
          M_t27_holder( 0 ),
          M_t28_holder( 0 ),
          M_t29_holder( 0 ),
          M_t30_holder( 0 )
      { setValue( tuple ); }

      ~Tuple30()
      {}

      ThisType&
      operator=( const ThisType& tuple )
      { return setValue( tuple ); }

      template< class T >
      bool
      isTypeOf() const
      { T* t = NULL; return isTypeOf( t ); }

      template< class T >
      T&
      get()
      { T* t = NULL; return get( t ); }

      template< class T >
      const T&
      get() const
      { T* t = NULL; return get( t ); }

      T1&
      setValue( const T1& t )
      { clear(); M_t1_holder = new T1( t ); return *M_t1_holder; }

      T2&
      setValue( const T2& t )
      { clear(); M_t2_holder = new T2( t ); return *M_t2_holder; }

      T3&
      setValue( const T3& t )
      { clear(); M_t3_holder = new T3( t ); return *M_t3_holder; }

      T4&
      setValue( const T4& t )
      { clear(); M_t4_holder = new T4( t ); return *M_t4_holder; }

      T5&
      setValue( const T5& t )
      { clear(); M_t5_holder = new T5( t ); return *M_t5_holder; }

      T6&
      setValue( const T6& t )
      { clear(); M_t6_holder = new T6( t ); return *M_t6_holder; }

      T7&
      setValue( const T7& t )
      { clear(); M_t7_holder = new T7( t ); return *M_t7_holder; }

      T8&
      setValue( const T8& t )
      { clear(); M_t8_holder = new T8( t ); return *M_t8_holder; }

      T9&
      setValue( const T9& t )
      { clear(); M_t9_holder = new T9( t ); return *M_t9_holder; }

      T10&
      setValue( const T10& t )
      { clear(); M_t10_holder = new T10( t ); return *M_t10_holder; }

      T11&
      setValue( const T11& t )
      { clear(); M_t11_holder = new T11( t ); return *M_t11_holder; }

      T12&
      setValue( const T12& t )
      { clear(); M_t12_holder = new T12( t ); return *M_t12_holder; }

      T13&
      setValue( const T13& t )
      { clear(); M_t13_holder = new T13( t ); return *M_t13_holder; }

      T14&
      setValue( const T14& t )
      { clear(); M_t14_holder = new T14( t ); return *M_t14_holder; }

      T15&
      setValue( const T15& t )
      { clear(); M_t15_holder = new T15( t ); return *M_t15_holder; }

      T16&
      setValue( const T16& t )
      { clear(); M_t16_holder = new T16( t ); return *M_t16_holder; }

      T17&
      setValue( const T17& t )
      { clear(); M_t17_holder = new T17( t ); return *M_t17_holder; }

      T18&
      setValue( const T18& t )
      { clear(); M_t18_holder = new T18( t ); return *M_t18_holder; }

      T19&
      setValue( const T19& t )
      { clear(); M_t19_holder = new T19( t ); return *M_t19_holder; }

      T20&
      setValue( const T20& t )
      { clear(); M_t20_holder = new T20( t ); return *M_t20_holder; }

      T21&
      setValue( const T21& t )
      { clear(); M_t21_holder = new T21( t ); return *M_t21_holder; }

      T22&
      setValue( const T22& t )
      { clear(); M_t22_holder = new T22( t ); return *M_t22_holder; }

      T23&
      setValue( const T23& t )
      { clear(); M_t23_holder = new T23( t ); return *M_t23_holder; }

      T24&
      setValue( const T24& t )
      { clear(); M_t24_holder = new T24( t ); return *M_t24_holder; }

      T25&
      setValue( const T25& t )
      { clear(); M_t25_holder = new T25( t ); return *M_t25_holder; }

      T26&
      setValue( const T26& t )
      { clear(); M_t26_holder = new T26( t ); return *M_t26_holder; }

      T27&
      setValue( const T27& t )
      { clear(); M_t27_holder = new T27( t ); return *M_t27_holder; }

      T28&
      setValue( const T28& t )
      { clear(); M_t28_holder = new T28( t ); return *M_t28_holder; }

      T29&
      setValue( const T29& t )
      { clear(); M_t29_holder = new T29( t ); return *M_t29_holder; }

      T30&
      setValue( const T30& t )
      { clear(); M_t30_holder = new T30( t ); return *M_t30_holder; }

      ThisType&
      setValue( const ThisType& tuple )
      {
        T1* t1 = 0;
        T2* t2 = 0;
        T3* t3 = 0;
        T4* t4 = 0;
        T5* t5 = 0;
        T6* t6 = 0;
        T7* t7 = 0;
        T8* t8 = 0;
        T9* t9 = 0;
        T10* t10 = 0;
        T11* t11 = 0;
        T12* t12 = 0;
        T13* t13 = 0;
        T14* t14 = 0;
        T15* t15 = 0;
        T16* t16 = 0;
        T17* t17 = 0;
        T18* t18 = 0;
        T19* t19 = 0;
        T20* t20 = 0;
        T21* t21 = 0;
        T22* t22 = 0;
        T23* t23 = 0;
        T24* t24 = 0;
        T25* t25 = 0;
        T26* t26 = 0;
        T27* t27 = 0;
        T28* t28 = 0;
        T29* t29 = 0;
        T30* t30 = 0;
        if( tuple.isTypeOf( t1 ) )
          setValue( tuple.get( t1 ) );
        else if( tuple.isTypeOf( t2 ) )
          setValue( tuple.get( t2 ) );
        else if( tuple.isTypeOf( t3 ) )
          setValue( tuple.get( t3 ) );
        else if( tuple.isTypeOf( t4 ) )
          setValue( tuple.get( t4 ) );
        else if( tuple.isTypeOf( t5 ) )
          setValue( tuple.get( t5 ) );
        else if( tuple.isTypeOf( t6 ) )
          setValue( tuple.get( t6 ) );
        else if( tuple.isTypeOf( t7 ) )
          setValue( tuple.get( t7 ) );
        else if( tuple.isTypeOf( t8 ) )
          setValue( tuple.get( t8 ) );
        else if( tuple.isTypeOf( t9 ) )
          setValue( tuple.get( t9 ) );
        else if( tuple.isTypeOf( t10 ) )
          setValue( tuple.get( t10 ) );
        else if( tuple.isTypeOf( t11 ) )
          setValue( tuple.get( t11 ) );
        else if( tuple.isTypeOf( t12 ) )
          setValue( tuple.get( t12 ) );
        else if( tuple.isTypeOf( t13 ) )
          setValue( tuple.get( t13 ) );
        else if( tuple.isTypeOf( t14 ) )
          setValue( tuple.get( t14 ) );
        else if( tuple.isTypeOf( t15 ) )
          setValue( tuple.get( t15 ) );
        else if( tuple.isTypeOf( t16 ) )
          setValue( tuple.get( t16 ) );
        else if( tuple.isTypeOf( t17 ) )
          setValue( tuple.get( t17 ) );
        else if( tuple.isTypeOf( t18 ) )
          setValue( tuple.get( t18 ) );
        else if( tuple.isTypeOf( t19 ) )
          setValue( tuple.get( t19 ) );
        else if( tuple.isTypeOf( t20 ) )
          setValue( tuple.get( t20 ) );
        else if( tuple.isTypeOf( t21 ) )
          setValue( tuple.get( t21 ) );
        else if( tuple.isTypeOf( t22 ) )
          setValue( tuple.get( t22 ) );
        else if( tuple.isTypeOf( t23 ) )
          setValue( tuple.get( t23 ) );
        else if( tuple.isTypeOf( t24 ) )
          setValue( tuple.get( t24 ) );
        else if( tuple.isTypeOf( t25 ) )
          setValue( tuple.get( t25 ) );
        else if( tuple.isTypeOf( t26 ) )
          setValue( tuple.get( t26 ) );
        else if( tuple.isTypeOf( t27 ) )
          setValue( tuple.get( t27 ) );
        else if( tuple.isTypeOf( t28 ) )
          setValue( tuple.get( t28 ) );
        else if( tuple.isTypeOf( t29 ) )
          setValue( tuple.get( t29 ) );
        else if( tuple.isTypeOf( t30 ) )
          setValue( tuple.get( t30 ) );
        else
          clear();
        return *this;
      }

      void
      clearAll()
      { clear(); }

      template< class X >
      X&
      operator=( const X& x )
      { return setValue( x ); }
    };
  }
}



#endif


#endif
