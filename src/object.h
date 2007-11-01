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

    double vangle( const PVector & target,
                   const PVector& origin ) const;

    double vangle( const PVector & target,
                   const double & origin ) const;

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



inline
double
PVector::vangle( const PVector & target,
                 const PVector & origin ) const
{
    return ( origin - *this ).angle( target - *this );
}

inline
double
PVector::vangle( const PVector & target,
                 const double & origin ) const
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

    static int S_object_count;

    const PObject::obj_type M_object_type;
    const int M_id;

    std::string M_name;
    std::string M_short_name;
    std::string M_close_name;
    std::string M_short_close_name;

    const double M_object_version;

protected:
    double	M_size; //! object's radiuos value

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
             const double & v = 3.0 );

    virtual
    ~PObject()
      { }

    //     void Set( const PVector & p,
    //               const double & s )
    //       {
    //           pos = p;
    //           size = s;
    //       }


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
    double	M_dist_qstep_team;
    double	M_land_qstep_team;
    double	M_dir_qstep_team;
#endif

    // pfr 8/14/00: for RC2000 evaluation
    double M_prand_factor_team; //factor to multiply prand by
    double M_kick_rand_factor_team; //factor to multiple kick_rand by

    int M_subs_count;
    std::map< int, int > M_ptype_count;
    std::map< int, int > M_ptype_used_count;

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

    Player* newPlayer( const double & version,
                       const bool goalie_flag );

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
    double & distQstepTeam() const
      {
          return M_dist_qstep_team;
      }
    const
    double & landQstepTeam() const
      {
          return M_land_qstep_team;
      }
    const
    double & dirQstepTeam() const
      {
          return M_dir_qstep_team;
      }
#endif
    const
    double & prandFactorTeam() const
      {
          return M_prand_factor_team;
      }
    const
    double & kickRandFactorTeam() const
      {
          return M_kick_rand_factor_team;
      }

    int subsCount() const
      {
          return M_subs_count;
      }

    const
    std::map< int, int > & ptypeCount() const
      {
          return M_ptype_count;
      }

    const
    std::map< int, int > & ptypeUsedCount() const
      {
          return M_ptype_used_count;
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
