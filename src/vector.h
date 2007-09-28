// -*-c++-*-

/***************************************************************************
                          vector.h  -  Geometry classes
                             -------------------
    begin                : 31-DEC-2001
    copyright            : (C) 2001 by The RoboCup Soccer Server 
                           Maintainance Group.
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



#ifndef _VECTOR_H_
#define _VECTOR_H_
#include "utility.h"

#include <iostream>
#include <algorithm>


namespace rcss
{
  namespace geom
  {
    inline
    double
    normalize_angle( double ang )
    {
      while(1) {
        if (ang < -M_PI)
          ang += 2 * M_PI ;
        else if (ang > M_PI)
          ang -= 2 * M_PI ;
        else
          break ;
      }
      return ang ;
    }
    
    class Vector2D
    {
    public:
      typedef double              value;
      typedef value&              reference;
      typedef const value&        const_reference;
      typedef value*              pointer;
      typedef const value*        const_pointer;
      typedef value*              iterator;
      typedef const value*        const_iterator;
      typedef std::size_t         size_type;

      enum index
      {
        X,
        Y,
        DIM
      };
      
      
    protected:

      value M_data [ DIM ];
      
    public:

      explicit
      Vector2D ( const_reference a, const_reference b )
      {
        M_data [ X ] = a;
        M_data [ Y ] = b;
      }

      explicit 
      Vector2D ()
      { null (); }
    
      explicit Vector2D ( const value vec[ DIM ] )
      {
        for ( size_type i = X; i < DIM; ++i )
          {
            M_data [ i ] = vec [ i ];
          }
      }

      inline void null ()
      {
        for ( size_type i = X; i < DIM; ++i )
          M_data [ i ] = 0.0; 
      }

      inline iterator begin() 
      { 
        return M_data; 
      }

      inline const_iterator begin() const 
      {
        return M_data; 
      }
      
      inline iterator end() 
      { 
        return M_data + DIM; 
      }
      
      inline const_iterator end() const 
      { 
        return M_data + DIM; 
      }

      inline reference operator[] ( const index& i ) 
      { 
        return M_data [ i ]; 
      }

      inline const_reference operator[] ( const index& i ) const 
      { 
        return M_data [ i ]; 
      }

      inline Vector2D operator+ () const
      {
        return *this;
      }

      inline Vector2D operator- () const
      {
        Vector2D rval ( *this );
        for ( size_type i = X; i < DIM; ++i )
          {
            rval.M_data [ i ] = -M_data [ i ];
          }
        return rval;
      }

      
      inline Vector2D& operator+= ( const Vector2D& vec )
      {
        for ( size_type i = X; i < DIM; ++i )
          {
            M_data [ i ] += vec.M_data [ i ];
          }
        return *this;
      }
      
      inline Vector2D& operator-= ( const Vector2D& vec )
      {
        return *this += -vec;
      }

      
      inline Vector2D& operator*= ( const_reference scaler )
      {
        for ( size_type i = X; i < DIM; ++i )
          {
            M_data [ i ] *= scaler;
          }
        return *this;
      }

      
      inline Vector2D& operator/= ( const_reference scaler )
      {
        for ( size_type i = X; i < DIM; ++i )
          {
            M_data [ i ] /= scaler;
          }
        return *this;
      }

      inline value getMag () const
      {
        value sum = 0.0;
        for ( size_type i = X; i < DIM; ++i )
          {
            sum += pow ( M_data [ i ], 2 );
          }
        return sqrt ( sum );
      }

      
      void 
      setMag ( const_reference mag );

      inline 
      value 
      getHead () const
      { return atan2 ( M_data [ Y ], M_data [ X ] ); }

      void 
      setHead ( const_reference head );

      Vector2D 
      getNorm () const;

      inline reference getX ()
      {
        return M_data [ X ];
      }

      inline const_reference getX () const
      {
        return M_data [ X ];
      }

      inline reference getY ()
      {
        return M_data [ Y ];
      }

      inline const_reference getY () const
      {
        return M_data [ Y ];
      }

      inline Vector2D& rotate ( const_reference ang )
      {
        setHead ( getHead () + ang );
        return *this;
      }
    }; // end class Vector2D
  };
};
    
inline bool operator== ( const rcss::geom::Vector2D& a, 
                         const rcss::geom::Vector2D& b )
{
  return std::equal( a.begin(), a.end(), b.begin() );
}

inline bool operator!= ( const rcss::geom::Vector2D& a,
                         const rcss::geom::Vector2D& b )
{
  return !( a == b );
}

inline std::ostream& operator<< ( std::ostream& o,
                                  const rcss::geom::Vector2D& data )
{
  return o << "rcs::geom::Vector2D( " 
           << data.getX () << ", " 
           << data.getY () << " )";
}
    
    
inline 
rcss::geom::Vector2D
operator+ ( const rcss::geom::Vector2D& a, 
            const rcss::geom::Vector2D& b )
{
  rcss::geom::Vector2D rval ( a );
  return rval += b;
}
    
inline
rcss::geom::Vector2D
operator- ( const rcss::geom::Vector2D& a, 
            const rcss::geom::Vector2D& b )
{
  rcss::geom::Vector2D rval ( a );
  return rval -= b;
}
        
inline 
rcss::geom::Vector2D 
operator* ( const rcss::geom::Vector2D& vec, 
            rcss::geom::Vector2D::const_reference scaler )
{
  rcss::geom::Vector2D rval ( vec );
  return rval *= scaler;
}
    
inline
rcss::geom::Vector2D 
operator* ( rcss::geom::Vector2D::const_reference scaler,
            const rcss::geom::Vector2D& vec )
{ return vec * scaler; }


inline
rcss::geom::Vector2D
operator/ ( const rcss::geom::Vector2D& vec, 
            rcss::geom::Vector2D::const_reference scaler )
{
  rcss::geom::Vector2D rval ( vec );
  return rval /= scaler;
} 


namespace rcss
{
  namespace geom
  {
    inline
    void 
    Vector2D::setMag ( const_reference mag )
    { *this = getNorm () * mag; }
    
    inline
    void 
    Vector2D::setHead ( const_reference head )
    { *this = Vector2D ( cos ( head ), sin ( head ) ) * getMag (); }

    inline
    Vector2D 
    Vector2D::getNorm () const
    { return *this / getMag (); }

    inline 
    Vector2D::value 
    dot ( const Vector2D& vec1, 
          const Vector2D& vec2 )
    {
      return vec1.getX () * vec2.getX () 
        + vec1.getY () * vec2.getY ();
    }
    
    inline 
    Vector2D::value
    absAngle ( const Vector2D& a, 
               const Vector2D& b )
    { return acos ( dot ( a.getNorm (), b.getNorm () ) ); }

    inline
    Vector2D::value
    angle ( const Vector2D& a, 
            const Vector2D& b )
    { return normalize_angle ( b.getHead () - a.getHead () ) ; }

    
    inline
    Vector2D 
    unitVector2D ( Vector2D::const_reference head )
    {
      Vector2D tmp;
      tmp.getX () = cos ( head );
      tmp.getY () = sin ( head );
      return tmp;
    }
    
    inline
    Vector2D 
    polarVector2D ( Vector2D::const_reference mag,
                    Vector2D::const_reference head )
    {
      Vector2D tmp = unitVector2D ( head );
      tmp.setMag ( mag );
      return tmp;
    }

    inline 
    Vector2D::value 
    dist (  const Vector2D& a, 
            const Vector2D& b )
    {
      return ( a - b ).getMag ();
    }
  }; // end namespace geom
}; // end namespace rcs

   
#endif
