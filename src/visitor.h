// -*-c++-*-

/***************************************************************************
                                 visitor.h  
                      Classes for visiting and type extractiong
                             -------------------
    begin                : 20-FEB-2002
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

#ifndef _VISITOR_H_
#define _VISITOR_H_


#include "tuple.h"

namespace rcss
{
  namespace util
    {
      template< class X >
      class Visitor;

      template< class H, class T >
      class Visitor< TypeList< H, T > >
        : private Visitor< T >
      {
      private:
        virtual
        void
        visit( H h ) = 0;

      protected:
        ~Visitor()
        {}
      public:
        Visitor()
        {}

        template< class X >
        inline
        void
        startVisit( X x )
        { Visitor< T >::startVisit( x ); }
        
        inline
        void
        startVisit( H h )
        { visit( h ); }
        
      };
      
      
      template< class H >
      class Visitor< TypeList< H, NullType > >
      { 
      private:
        virtual
        void
        visit( H h ) = 0;
       
      protected:
        ~Visitor()
        {}
      public:
        Visitor()
        {}
        
        inline
        void
        startVisit( H h )
        { visit( h ); }
        
      };


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
      class Visitor10
      {
      private:
        virtual
        void
        visit( T1 ) {}

        virtual
        void
        visit( T2 ) {}

        virtual
        void
        visit( T3 ) {}

        virtual
        void
        visit( T4 ) {}

        virtual
        void
        visit( T5 ) {}

        virtual
        void
        visit( T6 ) {}

        virtual
        void
        visit( T7 ) {}

        virtual
        void
        visit( T8 ) {}

        virtual
        void
        visit( T9 ) {}

        virtual
        void
        visit( T10 ) {}

      protected:
          virtual
          ~Visitor10()
          {}
      public:
        Visitor10()
        {}

        template< class X >
        inline
        void
        startVisit( X x )
        { visit( x ); }       
      };


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
      class Visitor20
      {
      private:
        virtual
        void
        visit( T1 ) {}

        virtual
        void
        visit( T2 ) {}

        virtual
        void
        visit( T3 ) {}

        virtual
        void
        visit( T4 ) {}

        virtual
        void
        visit( T5 ) {}

        virtual
        void
        visit( T6 ) {}

        virtual
        void
        visit( T7 ) {}

        virtual
        void
        visit( T8 ) {}

        virtual
        void
        visit( T9 ) {}

        virtual
        void
        visit( T10 ) {}

        virtual
        void
        visit( T11 ) {}

        virtual
        void
        visit( T12 ) {}

        virtual
        void
        visit( T13 ) {}

        virtual
        void
        visit( T14 ) {}

        virtual
        void
        visit( T15 ) {}

        virtual
        void
        visit( T16 ) {}

        virtual
        void
        visit( T17 ) {}

        virtual
        void
        visit( T18 ) {}

        virtual
        void
        visit( T19 ) {}

        virtual
        void
        visit( T20 ) {}

      protected:
          virtual
        ~Visitor20()
        {}
      public:
        Visitor20()
        {}

        template< class X >
        inline
        void
        startVisit( X x )
        { visit( x ); }       
      };




      template< class TList >
      class TypeExtractor
        : public Visitor< TList >
      {
      private:
        Tuple< TList > M_holder;

      protected:
        template< class X >
        inline
        void
        setValue( const X& x )
        { M_holder = x; }

        ~TypeExtractor()
        {}
      public:
        TypeExtractor()
        {}


        template< class X >
        inline
        bool
        isTypeOf() const
        { return M_holder.template isTypeOf< X >(); }
        
        template< class X >
        inline
        bool
        isTypeOf( X* x ) const
        { return M_holder.isTypeOf( x ); }
        
        
        template< class X >
        inline
        X&
        get()
        { return M_holder.template get< X >(); }

        template< class X >
        inline
        X&
        get( X* x )
        { return M_holder.get( x ); }

        template< class X >
        inline
        const X&
        get() const
        { return M_holder.template get< X >(); }

        template< class X >
        inline
        const X&
        get( X* x ) const
        { return M_holder.get( x ); }
      };



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
      class TypeExtractor10
        : public Visitor10< T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 >
      {
      private:
        Tuple10< T1, T2, T3, T4, T5, T6, T7, T8, T9, T10 > M_holder;

      protected:
        template< class X >
        inline
        void
        setValue( const X& x )
        { M_holder = x; }

          virtual
          ~TypeExtractor10()
          {}
      public:
        TypeExtractor10()
        {}

        template< class X >
        inline
        bool
        isTypeOf() const
        { return M_holder.template isTypeOf< X >(); }
        
        template< class X >
        inline
        bool
        isTypeOf( X* x ) const
        { return M_holder.isTypeOf( x ); }
        
        template< class X >
        inline
        X&
        get()
        { return M_holder.template get< X >(); }

        template< class X >
        inline
        X&
        get( X* x )
        { return M_holder.get( x ); }

        template< class X >
        inline
        const X&
        get() const
        { return M_holder.template get< X >(); }

        template< class X >
        inline
        const X&
        get( X* x ) const
        { return M_holder.get( x ); }
      };


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
      class TypeExtractor20
        : public Visitor20< T1, T2, T3, T4, T5, T6, T7, T8, T9, T10,
        T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 >
      {
      private:
        Tuple20< T1, T2, T3, T4, T5, T6, T7, T8, T9, T10,
          T11, T12, T13, T14, T15, T16, T17, T18, T19, T20 > M_holder;

      protected:
        template< class X >
        inline
        void
        setValue( const X& x )
        { M_holder = x; }

          virtual
        ~TypeExtractor20()
        {}
      public:
        TypeExtractor20()
        {}

        template< class X >
        inline
        bool
        isTypeOf() const
        { return M_holder.template isTypeOf< X >(); }
        
        template< class X >
        inline
        bool
        isTypeOf( X* x ) const
        { return M_holder.isTypeOf( x ); }
        
        template< class X >
        inline
        X&
        get()
        { return M_holder.template get< X >(); }

        template< class X >
        inline
        X&
        get( X* x )
        { return M_holder.get( x ); }

        template< class X >
        inline
        const X&
        get() const
        { return M_holder.template get< X >(); }

        template< class X >
        inline
        const X&
        get( X* x ) const
        { return M_holder.get( x ); }
      };
  }
}


#endif


#endif
