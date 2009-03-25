/* -*-C++-*-
 *Header:
 *File: object.h (for C++ & cc)
 *Author: Noda Itsuki
 *Date: 1995/02/21
 *EndHeader:
 */

/*
 *Copyright:

 Copyright (C) 1996-2000 Electrotechnical Laboratory.
 Itsuki Noda, Yasuo Kuniyoshi and Hitoshi Matsubara.
 Copyright (C) 2000, 2001 RoboCup Soccer Server Maintainance Group.
 Patrick Riley, Tom Howard, Daniel Polani, Itsuki Noda,
 Mikhail Prokopenko, Jan Wendler
 Copyright (C) 2002- RoboCup Soccer Simulator Maintainance Group.

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


/*
 *ModifyHistory
 *
 *EndModifyHistory
 */


#ifndef RCSSSERVER_OBJECT_H
#define RCSSSERVER_OBJECT_H

#include "types.h"
#include "utility.h"

#include <vector>
#include <string>
#include <algorithm>
#include <memory>
#include <map>
#include <iostream>
#include <cmath>

/*
 *===================================================================
 *Part: Plain Vector
 *===================================================================
 */
class PVector {
public:
    double x;
    double y;

    explicit
    PVector( const double & xx = 0.0, const double & yy = 0.0 )
        : x( xx )
        , y( yy )
      { }

    const
    PVector & assign( const double & xx, const double & yy )
      {
          x = xx;
          y = yy;
          return *this;
      }

    PVector	operator-() const
      {
          return PVector( -x, -y );
      }

    const
    PVector &	operator+() const
      {
          return *this;
      }

    const PVector& operator+=( const PVector & v )
      {
          x += v.x;
          y += v.y;
          return *this;
      }

    const PVector& operator-=( const PVector & v )
      {
          x -= v.x;
          y -= v.y;
          return *this;
      }

    const PVector& operator*=( const double & a )
      {
          x *= a;
          y *= a;
          return *this;
      }

    const PVector& operator/=( const double & a )
      {
          x /= a;
          y /= a;
          return *this;
      }

    double r2() const
      {
          return x*x + y*y;
      }

    double r() const
      {
          return std::sqrt( x*x + y*y );
      }

    double th() const
      {
          return ( (x == 0.0) && ( y == 0.0 )
                   ? 0.0
                   : std::atan2( y, x ) );
      }

    const PVector & normalize( const double & l = 1.0 )
      {
          *this *= ( l / std::max( r(), EPS ) );
          return *this;
      }

    double distance2( const PVector & orig ) const
      {
          return ( PVector( *this ) -= orig ).r2();
      }

    double distance( const PVector & orig ) const
      {
          return ( PVector( *this ) -= orig ).r();
      }

    double angle() const
      {
          return th();
      }

    double angle( const PVector & dir ) const
      {
          double ang = dir.th() - this->th();
          return normalize_angle( ang );
      }

    const
    PVector & rotate( const double & ang );

//     double vangle( const PVector & target,
//                    const PVector & origin ) const;

//     double vangle( const PVector & target,
//                    const double & origin ) const;

    bool between( const PVector & begin,
                  const PVector & end ) const;

    static
    PVector fromPolar( const double & r,
                       const double & ang )
      {
          return PVector( r * std::cos( ang ),
                          r * std::sin( ang ) );
      }

};

inline
bool
operator==( const PVector & lhs,
            const PVector & rhs )
{
    return ( lhs.x == rhs.x && lhs.y == rhs.y );
}

inline
bool
operator!=( const PVector & lhs,
            const PVector & rhs )
{
    return !( lhs == rhs );
}

inline
PVector
operator+( const PVector & lhs,
           const PVector & rhs )
{
    return PVector( lhs ) += rhs;
}

inline
PVector
operator-( const PVector & lhs,
           const PVector & rhs )
{
    return PVector( lhs ) -= rhs;
}


inline
std::ostream &
operator<<( std::ostream & o,
            const PVector & v )
{
    return o << "#V[" << v.x << "," << v.y << "]";
}



// inline
// double
// PVector::vangle( const PVector & target,
//                  const PVector & origin ) const
// {
//     return ( origin - *this ).angle( target - *this );
// }

// inline
// double
// PVector::vangle( const PVector & target,
//                  const double & origin ) const
// {
//     return normalize_angle( ( target - *this ).angle() - origin );
// }




/*
 *===================================================================
 *Part: Area
 *===================================================================
 */
class RArea {
public:
    double left;
    double right;
    double top;
    double bottom;

    RArea( const double & l,
           const double & r,
           const double & t,
           const double & b )
        : left( l ),
          right( r ),
          top( t ),
          bottom( b )
      {

      }

    RArea( const PVector & center,
           const PVector & size )
        : left( center.x - size.x*0.5 ),
          right( center.x + size.x*0.5 ),
          top( center.y - size.y*0.5 ),
          bottom( center.y + size.y*0.5 )
      {

      }

    bool inArea( const PVector & p ) const
      {
          return ( p.x >= left )
              && ( p.x <= right )
              && ( p.y >= top )
              && ( p.y <= bottom );
      }

    /*! find nearest horizontal line */
    PVector nearestHEdge( const PVector & p ) const;

		/*! find nearest vertical line */
    PVector nearestVEdge( const PVector & p ) const;

    PVector nearestEdge( const PVector & p ) const;

    PVector randomize() const;

    std::ostream & print( std::ostream & o ) const;
};

//inline
//std::ostream &
//operator<<( std::ostream & o, const RArea & a )
//{
//    return a.print( o );
//}

class CArea {
private:
    PVector M_center;
    double M_radius;

public:
    CArea( const PVector & center,
           const double & radius )
        : M_center( center ),
          M_radius( radius )
      { }

    const
    PVector & center() const
      {
          return M_center;
      }

    const
    double & radius() const
      {
          return M_radius;
      }

    bool inArea( const PVector & p ) const
      {
          return M_center.distance( p ) <= M_radius;
      }

    PVector nearestEdge( const PVector & p ) const;

};


inline
bool
operator==( const CArea & lhs,
            const CArea & rhs  )
{
    return ( lhs.center() == rhs.center()
             && lhs.radius() == rhs.radius() );
}

inline
bool
operator!=( const CArea & lhs,
            const CArea & rhs )
{
    return !( lhs == rhs );
}

//extern std::ostream& operator<< (std::ostream& o, const CArea& a);

// bool
// intersect( const PVector & begin,
//            const PVector & end,
//            const CArea & circle,
//            PVector & inter );
//CArea
//nearestPost( const PVector & pos,
//             const double & size );

/*
 *===================================================================
 *Part: PObject
 *===================================================================
 */
class PObject {
private:

    static int S_object_count;

    const int M_id;

    std::string M_name;
    std::string M_short_name;
    std::string M_close_name;
    std::string M_short_close_name;

    const double M_object_version;

protected:
    double	M_size; //! object's radiuos value
    PVector M_pos;
    bool M_enable;

private:

    // not used
    PObject();
    const PObject & operator=( const PObject & );

public:

    PObject( const std::string & name,
             const std::string & short_name,
             const std::string & close_name,
             const std::string & short_close_name,
             const PVector& p = PVector( 0.0,0.0 ),
             const double & v = 3.0 );

    virtual
    ~PObject()
      { }

    int id() const
      {
          return M_id;
      }

    const
    std::string & name() const
      {
          return M_name;
      }

    const
    std::string & shortName() const
      {
          return M_short_name;
      }

    const
    std::string & closeName() const
      {
          return M_close_name;
      }

    const std::string & shortCloseName() const
      {
          return M_short_close_name;
      }

    const
    double & objectVersion() const
      {
          return M_object_version;
      }

    const
    double & size() const
      {
          return M_size;
      }

    const
    PVector & pos() const
      {
          return M_pos;
      }

    bool isEnable() const
      {
          return M_enable;
      }

    std::ostream & print( std::ostream & o ) const;

protected:

    void setName( const std::string & name,
                  const std::string & short_name )
      {
          M_name = name;
          M_short_name = short_name;
      }

};

inline
std::ostream &
operator<<( std::ostream & o, const PObject & v )
{
    return v.print( o );
}


class Stadium;
//class Weather;

class MPObject
    : public PObject {

    friend class MPObjectTable;

protected:
    Stadium	& M_stadium;

    PVector	M_vel;

    PVector	M_accel;
    double	M_decay;
    double	M_randp;

    double M_weight;
    double M_max_speed;
    // th 6.3.00
    double M_max_accel;

private:
    //const Weather * M_weather;

    /* new collision stuff */
    PVector post_col_pos;
    int col_count;
    bool col;

private:

    // not used
    MPObject();
    const MPObject & operator=( const MPObject & );

public:

    MPObject( Stadium & stadium,
              const std::string & name,
              const std::string & short_name,
              const std::string & close_name,
              const std::string & short_close_name );

    virtual
    ~MPObject()
      { }

    const
    Stadium & stadium() const
      {
          return M_stadium;
      }

    const
    PVector & vel() const
      {
          return M_vel;
      }
    const
    PVector & accel() const
      {
          return M_accel;
      }

    void _inc();

    void _turn()
      {
          turnImpl();
      }
private:
    PVector noise();
    PVector wind();

public:
    void moveTo( const PVector & pos )
      {
          M_pos = pos;
      }

    void moveTo( const PVector & pos,
                 //const Angle & angle,
                 const PVector & vel,
                 const PVector & accel );

    void setConstant( const double & size,
                      const double & decay,
                      const double & randp,
                      const double & weight,
                      const double & max_speed,
                      const double & max_accel );

    void push( const PVector & f )
      {
          M_accel += f;
      }

    void clearCollide()
      {
          post_col_pos.assign( 0.0, 0.0 );
          col_count = 0;
          // col is reset in updateCollVel()
      }
    void collide( const PVector & col_pos )
      {
          post_col_pos += col_pos;
          ++col_count;
          col = true;
      }
    void updateCollVel()
      {
          if ( this->col )
          {
              this->M_vel *= -0.1;
              this->col = false;
          }
      }
    void moveToCollPos();

    std::ostream & print( std::ostream & o ) const;


protected:

    virtual
    void turnImpl() = 0;

    virtual
    void updateAngle() = 0;

    virtual
    void collidedWithPost() = 0;

    virtual
    double maxAccel() const = 0;

    virtual
    double maxSpeed() const = 0;

};

inline
std::ostream &
operator<<( std::ostream & o, const MPObject & v )
{
    return v.print( o );
}



class Ball
    : public MPObject {
private:


public:

    explicit
    Ball( Stadium & stadium );

    virtual
    void turnImpl()
      { }

    virtual
    void updateAngle()
      { }

    virtual
    void collidedWithPost()
      { }

    virtual
    double maxAccel() const
      {
          return M_max_accel;
      }

    virtual
    double maxSpeed() const
      {
          return M_max_speed;
      }
};

#endif // _H_OBJECT
