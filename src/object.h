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
#include "xpmholder.h"

#include <vector>
#include <string>
#include <algorithm>
#include <memory>
#include <map>
#include <cmath>

/*
 *===================================================================
 *Part: Plain Vector
 *===================================================================
 */
class PVector {
public:
    Value	x;
    Value	y;

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

    const PVector& operator*=( const Value & a )
      {
          x *= a;
          y *= a;
          return *this;
      }

    const PVector& operator/=( const Value & a )
      {
          x /= a;
          y /= a;
          return *this;
      }

    Value r2() const
      {
          return x*x + y*y;
      }

    Value r() const
      {
          return std::sqrt( x*x + y*y );
      }

    Value th() const
      {
          return ( (x == 0.0) && ( y == 0.0 )
                   ? 0.0
                   : std::atan2( y, x ) );
      }

    const PVector & normalize( const Value & l = 1.0 )
      {
          *this *= ( l / std::max( r(), EPS ) );
          return *this;
      }

    Value distance2( const PVector & orig ) const
      {
          return ( PVector( *this ) -= orig ).r2();
      }

    Value distance( const PVector & orig ) const
      {
          return ( PVector( *this ) -= orig ).r();
      }

    Value angle() const
      {
          return th();
      }

    Value angle( const PVector & dir ) const
      {
          Angle ang = dir.th() - this->th();
          return normalize_angle( ang );
      }

    const
    PVector & rotate( const Angle & ang );

    Value vangle( const PVector & target,
                  const PVector& origin ) const;

    Value vangle( const PVector & target,
                  const Angle & origin ) const;

    bool between( const PVector & begin,
                  const PVector & end ) const;

    static
    PVector fromPolar( const Value & r, const Angle & ang )
      {
          return PVector( r * std::cos( ang ),
                          r * std::sin( ang ) );
      }

};

inline
bool
operator==( const PVector & lhs, const PVector & rhs )
{
    return ( lhs.x == rhs.x && lhs.y == rhs.y );
}

inline
bool
operator!=( const PVector & lhs, const PVector & rhs )
{
    return !( lhs == rhs );
}

inline
PVector
operator+( const PVector & lhs, const PVector & rhs )
{
    return PVector( lhs ) += rhs;
}

inline
PVector
operator-( const PVector & lhs, const PVector & rhs )
{
    return PVector( lhs ) -= rhs;
}


inline
std::ostream &
operator<<( std::ostream & o, const PVector & v )
{
    return o << "#V[" << v.x << "," << v.y << "]";
}



inline
Value
PVector::vangle( const PVector & target,
                 const PVector& origin ) const
{
    return ( origin - *this ).angle( target - *this );
}

inline
Value
PVector::vangle( const PVector & target,
                 const Angle & origin ) const
{
    return normalize_angle( ( target - *this ).angle() - origin );
}




/*
 *===================================================================
 *Part: Area
 *===================================================================
 */
class RArea {
public:
    Value left;
    Value right;
    Value top;
    Value bottom;

    RArea( const Value & l,
           const Value & r,
           const Value & t,
           const Value & b )
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
    Value M_radius;

public:
    CArea( const PVector & center,
           const Value & radius )
        : M_center( center ),
          M_radius( radius )
      { }

    const
    PVector & center() const
      {
          return M_center;
      }

    const
    Value & radius() const
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
//             const Value & size );

/*
 *===================================================================
 *Part: PObject
 *===================================================================
 */
class PObject {
public:
    enum obj_type
        {
            OT_BALL,
            OT_PLAYER,
            OT_FLAG,
            OT_GOAL,
            OT_LINE
        };


private:

    static TheNumber S_object_count;

    const PObject::obj_type M_object_type;
    const ID M_id;

    std::string M_name;
    std::string M_short_name;
    std::string M_close_name;
    std::string M_short_close_name;

    const Value M_obj_ver;

protected:
    Value	M_size; //! object's radiuos value

    PVector M_pos;
    //Angle M_angle;

    bool M_enable;

private:

    // not used
    PObject();
    const PObject & operator=( const PObject & );

public:

    PObject( const PObject::obj_type & object_type,
             const std::string & name,
             const std::string & short_name,
             const std::string & close_name,
             const std::string & short_close_name,
             const PVector& p = PVector( 0.0,0.0 ),
             const Value & v = 3.0 );

    virtual
    ~PObject()
      { }

//     void Set( const PVector & p,
//               const Value & s )
//       {
//           pos = p;
//           size = s;
//       }


    ID id() const
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
    Value & objVer() const
      {
          return M_obj_ver;
      }

    const
    Value & size() const
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

    PObject::obj_type getObjectType() const
      {
          return M_object_type;
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
class Weather;
class MPObjectTable;

class MPObject
    : public PObject {

    friend class MPObjectTable;

protected:
    Stadium	* M_stadium;

    PVector	M_vel;

    PVector	M_accel;
    Value	M_decay;
    Value	M_randp;

    Value M_weight;
    Value M_max_speed;
    // th 6.3.00
    Value M_max_accel;

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

    MPObject( Stadium * stadium,
              const PObject::obj_type & object_type,
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
          return *M_stadium;
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

    void setConstant( const Value & size,
                      const Value & decay,
                      const Value & randp,
                      const Value & weight,
                      const Value & max_speed,
                      const Value & max_accel );

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
    void turnImpl()
      { }

    virtual
    void updateAngle()
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
    Ball( Stadium * stadium );

};


/*
 *===================================================================
 *Part: Player & Team
 *===================================================================
 */


class Player;
class OnlineCoach;

class Team {
private:
    Stadium * M_stadium;

    std::string M_name;
    const Side M_side;

    Player * M_players[MAX_PLAYER];
    int M_size; //! the number of connected players

    int M_point;

    int M_pen_taken;        /* goals attempted in penalty_mode */
    int M_pen_point;        /* goals made in penalty_mode */
    bool M_pen_won;


#ifdef	NEW_QSTEP
    Value	M_dist_qstep_team;
    Value	M_land_qstep_team;
    Value	M_dir_qstep_team;
#endif

    // pfr 8/14/00: for RC2000 evaluation
    Value M_prand_factor_team; //factor to multiply prand by
    Value M_kick_rand_factor_team; //factor to multiple kick_rand by

    int M_subs_count;
    std::map< int, int > ptype_count;

    OnlineCoach *M_olcoach;

    typedef std::pair< unsigned int, unsigned int > GraphKey;
    typedef std::map< GraphKey, XPMHolder* > GraphCont;
    GraphCont M_graphics;


    // not used
    Team();
    const Team & operator=( const Team & );

public:

    Team( Stadium *stad, const Side side );
    ~Team();

    Player* newPlayer( Value version, int goalie_flag );

    void assignPlayer( int n, Player * player );
    void assignCoach( OnlineCoach * coach );

    void setName( const char * name )
      {
          M_name = name;
      }
    const
    std::string & name() const
      {
          return M_name;
      }

    bool enabled() const
      {
          return ! M_name.empty();
      }

    Side side() const
      {
          return M_side;
      }

    int size() const
      {
          return M_size;
      }

    const
    Player * player( const int i ) const
      {
          if ( i < 0 || M_size <= i ) return NULL;
          return M_players[i];
      }

    void setPoint( int point )
      {
          M_point = point;
      }
    int incPoint()
      {
          return ++M_point;
      }
    int point() const
      {
          return M_point;
      }

    void setPenaltyScore()
      {
          ++M_pen_point;
          ++M_pen_taken;
      }
    void setPenaltyMiss()
      {
          ++M_pen_taken;
      }
    int penaltyTaken() const
      {
          return M_pen_taken;
      }
    int penaltyPoint() const
      {
          return M_pen_point;
      }

    void setPenaltyWinner()
      {
          M_pen_won = true;
      }
    bool penaltyWon() const
      {
          return M_pen_won;
      }
#ifdef NEW_QSTEP
    const
    Value & distQstepTeam() const
      {
          return M_dist_qstep_team;
      }
    const
    Value & landQstepTeam() const
      {
          return M_land_qstep_team;
      }
    const
    Value & dirQstepTeam() const
      {
          return M_dir_qstep_team;
      }
#endif
    const
    Value & prandFactorTeam() const
      {
          return M_prand_factor_team;
      }
    const
    Value & kickRandFactorTeam() const
      {
          return M_kick_rand_factor_team;
      }

    int subsCount() const
      {
          return M_subs_count;
      }

    int ptypeCount( const int player_type ) const;

    void substitute( const Player * player,
                     const int player_type );

    OnlineCoach * olcoach()
      {
          return M_olcoach;
      }

    const
    OnlineCoach * olcoach() const
      {
          return M_olcoach;
      }

    void
    addGraphic( unsigned int x, unsigned int y,
                std::auto_ptr< XPMHolder > holder );
};

#endif // _H_OBJECT
