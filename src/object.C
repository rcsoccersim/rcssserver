/* -*- Mode: C++ -*-
 *Header:
 *File: object.C (for C++ & cc)
 *Author: Noda Itsuki
 *Date: 1995/02/21
 *EndHeader:
 */

/*
 *Copyright:

 Copyright (C) 1996-2000 Electrotechnical Laboratory.
 Itsuki Noda, Yasuo Kuniyoshi and Hitoshi Matsubara.
 Copyright (C) 2000-2007 RoboCup Soccer Server Maintainance Group.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "object.h"

//#include "audio.h"
#include "field.h"
#include "param.h"
#include "player.h"
#include "random.h"
#include "types.h"
#include "utility.h"

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <cassert>

/*
 *===================================================================
 *Part: Plane Vector
 *===================================================================
 */

const
PVector &
PVector::rotate(const Angle& ang)
{
    Value r1 = r();
    Angle th1 = th();

    x = r1 * std::cos( th1 + ang );
    y = r1 * std::sin( th1 + ang );

    return *this;
}

// Angle PVector::vangle(const PVector& target, const PVector& origin) const
// {
//     PVector axis = origin - *this;
//     PVector dir = target - *this;
//     return axis.angle( dir );
// }

// Angle PVector::vangle(const PVector& target, const Angle& origin) const
// {
//     PVector dir = target - *this;
//     Angle ang = dir.angle() - origin;

//     return normalize_angle( ang );
// }

bool
PVector::between( const PVector& begin, const PVector& end ) const
{
    if( begin.x > end.x )
        return between( end, begin );

    if( begin.x <= x && x <= end.x )
    {
        if( begin.y < end.y )
        {
            return begin.y <= y && y <= end.y;
        }
        else
        {
            return begin.y >= y && y >= end.y;
        }
    }
    //      std::cout << begin.x << " > " << x
    //                << "\n|| " << x << " > " << end.x;
    //      if( begin.y < end.y )
    //      {
    //          std::cout << "\n|| " << begin.y << " > " << y
    //                    << "\n|| " << y << " > " << end.y << std::endl;
    //      }
    //      else
    //      {
    //          std::cout << "\n|| " << begin.y << " < " << y
    //                    << "\n|| " << y << " < " << end.y << std::endl;
    //      }
    return false;
}
/*
 *===================================================================
 *Part: Area
 *===================================================================
 */

PVector
RArea::nearestHEdge( const PVector & p ) const
{
    return PVector( std::min( std::max( p.x, left ), right ),
                    ( std::fabs( p.y - top ) < std::fabs( p.y - bottom )
                    ? top : bottom ) );
}

PVector
RArea::nearestVEdge( const PVector & p ) const
{
    return PVector( ( std::fabs( p.x - left ) < std::fabs( p.x - right )
                    ? left : right ),
                    std::min( std::max( p.y, top ), bottom ) );
}

PVector
RArea::nearestEdge( const PVector & p ) const
{
    if ( std::min( std::fabs( p.x - left ), std::fabs( p.x - right ) )
        < std::min( std::fabs( p.y - top ), std::fabs( p.y - bottom ) ) )
    {
        return nearestVEdge( p );
    }
    else
    {
        return nearestHEdge( p );
    }
}

PVector
RArea::randomize() const
{
    return PVector( drand( left, right ),
                    drand( bottom, top ) );
}

std::ostream &
RArea::print( std::ostream& o ) const
{
    return o << "#A[h:" << left << "~" << right
             << ",v:" << top << "~" << bottom << "]";
}




PVector
CArea::nearestEdge( const PVector & p ) const
{
    PVector dif = p - M_center;
    if ( dif.x == 0.0 && dif.y == 0.0 )
        dif = PVector( EPS, EPS );

    dif.normalize( M_radius );

    return M_center + dif;
}

//std::ostream &
//operator<<( std::ostream & o, const CArea & a )
//{
//    return o << "#A[x:" << a.center.x <<
//				",y:" << a.center.y <<
//				",r:" << a.r << "]";
//}

namespace {
bool
intersect( const PVector & begin,
           const PVector & end,
           const CArea & circle,
           PVector & inter )
{
    if ( begin == end )
        return false;

    if ( ( begin - end ).r() < ( begin - circle.center() ).r() - circle.radius() )
        // object wont get within circles range
        return false;

    if ( circle.center() == PVector() )
    {
        double dx = end.x - begin.x;
        double dy = end.y - begin.y;
        //          std::cout << dx << endl;
        //          std::cout << dy << endl;
        double dr = sqrt( dx*dx + dy*dy );
        //          std::cout << dr << endl;
        double D = begin.x * end.y - end.x * begin.y;
        double descrim = circle.radius() * circle.radius() * dr*dr - D*D;
        //          std::cout << descrim << endl;
        if( descrim <= 0.0 )
        {
            // no collision of tagent
            //              std::cout << "Descrim < 0\n";
            return false;
        }
        else
        {
            descrim = std::sqrt( descrim );
            //              std::cout << descrim << endl;

            double x1 = (D*dy + dx * descrim) / (dr*dr);
            double x2 = (D*dy - dx * descrim) / (dr*dr);
            double y1 = (-D*dx + std::fabs( dy ) * descrim) / (dr*dr);
            double y2 = (-D*dx - std::fabs( dy ) * descrim) / (dr*dr);
            //              std::cout << x1 << endl;
            //              std::cout << x2 << endl;
            //              std::cout << y1 << endl;
            //              std::cout << y2 << endl;
            PVector first, second;
            if ( dy < 0 )
            {
                first = PVector( x2, y1 );
                second = PVector( x1, y2 );
            }
            else
            {
                first = PVector( x1, y1 );
                second = PVector( x2, y2 );
            }

            if( ! first.between( begin, end )
                && ! second.between( begin, end ) )
            {
                // intersections are not between the end points
                //                  std::cout << "Coll outside of end points\n"
                //                            << "begin = " << begin << std::endl
                //                            << "end = " << end << std::endl
                //                            << "first = " << first << std::endl
                //                            << "second = " << second << std::endl;

                return false;
            }

            if ( ! first.between( begin, end ) )
            {
                inter = second;
                second = first;
            }
            else if ( ! second.between( begin, end ) )
            {
                inter = first;
            }
            else
            {
                if ( ( begin - first ).r() < ( begin - second ).r() )
                {
                    inter = first;
                }
                else
                {
                    inter = second;
                    second = first;
                }
            }

            if ( inter == begin
                 && ! second.between( begin, end ) )
            {
                //                  std::cout << "Fake collision\n"
                //                            << "begin = " << begin << std::endl
                //                            << "end = " << end << std::endl
                //                            << "first = " << inter << std::endl
                //                            << "second = " << second << std::endl;
                // fake collision.  Object is tagent to the circle and moving away
                return false;
            }
            return true;
        }
    }
    else
    {
        if ( intersect( begin - circle.center(), end - circle.center(),
                        CArea( PVector(), circle.radius() ), inter ) )
        {
            inter += circle.center();
            return true;
        }
        return false;
    }
}
}

namespace {
CArea
nearestPost( const PVector & pos,
             const Value & size )
{
    PVector nearest_gpost;

    if ( pos.y > 0 )
    {
        if ( pos.x > 0 )
        {
            nearest_gpost = PVector( PITCH_LENGTH*0.5
                                     - ServerParam::instance().goalPostRadius(),
                                     ServerParam::instance().goalWidth()*0.5
                                     + ServerParam::instance().goalPostRadius() );
        }
        else
        {
            nearest_gpost = PVector( -PITCH_LENGTH*0.5
                                     + ServerParam::instance().goalPostRadius(),
                                     ServerParam::instance().goalWidth()*0.5
                                     + ServerParam::instance().goalPostRadius() );
        }
    }
    else
    {
        if ( pos.x > 0 )
        {
            nearest_gpost = PVector( PITCH_LENGTH*0.5
                                     - ServerParam::instance().goalPostRadius(),
                                     -ServerParam::instance().goalWidth()*0.5
                                     - ServerParam::instance().goalPostRadius() );
        }
        else
        {
            nearest_gpost = PVector( -PITCH_LENGTH*0.5
                                     + ServerParam::instance().goalPostRadius(),
                                     -ServerParam::instance().goalWidth()*0.5
                                     - ServerParam::instance().goalPostRadius() );
        }
    }

    return CArea( nearest_gpost, ServerParam::instance().goalPostRadius() + size );
}
}

/*
 *===================================================================
 *Part: PObject
 *===================================================================
 */

TheNumber PObject::S_object_count = 0;

/* pfr 06/07/200 added short name support */
PObject::PObject( const  PObject::obj_type& object_type,
                  const std::string & name,
                  const std::string & short_name,
                  const std::string & close_name,
                  const std::string & short_close_name,
                  const PVector & p,
                  const Value & v )
    : M_object_type ( object_type ),
      M_id( S_object_count ),
      M_name( name ),
      M_short_name( short_name ),
      M_close_name( close_name ),
      M_short_close_name( short_close_name ),
      M_obj_ver( v ),
      M_size( 1.0 ),
      M_pos( p ),
      //      M_angle( 0.0 ),
      M_enable( true )
{
    ++S_object_count;
}

std::ostream &
PObject::print( std::ostream & o ) const
{
    o << "#Ob[" << this->id();
    if( ! name().empty() )
        o << ":" << name() << "";
    return o << ":pos=" << this->M_pos
             << ",size=" << this->M_size
        //<< ",angle=" << this->M_angle
             << "]";
}


/*
 *===================================================================
 *Part: MPObject
 *===================================================================
 */

/* pfr 06/07/200 added short name support */
MPObject::MPObject( Stadium * stadium,
                    const PObject::obj_type & object_type,
                    const std::string & name,
                    const std::string & short_name,
                    const std::string & close_name,
                    const std::string & short_close_name )
    : PObject( object_type,
               name, short_name,
               close_name, short_close_name ),
      M_stadium( stadium ),
      M_vel( 0.0,0.0 ),
      M_accel( 0.0,0.0 )
{
    assert( stadium );
    //M_weather = &( stadium->weather() );
}

std::ostream &
MPObject::print( std::ostream & o ) const
 {
     o << "#Ob[" << this->id();
     if( ! name().empty() )
         o << ":" << name() << "";
     return o << ":pos=" << this->M_pos
              << ",size=" << this->M_size
         //<< ",angle=" << Rad2IDegRound( this->M_angle )
              << ",vel=" << this->M_vel
              << ",acc=" << this->M_accel
              << ",decay=" << this->M_decay
              << ",randp=" << this->M_randp
              << "]";
 }

void
MPObject::moveTo( const PVector & pos,
                  //const Angle & angle,
                  const PVector & vel,
                  const PVector & accel )
{
    M_pos = pos;
    //M_angle = angle;
    M_vel = vel;
    M_accel = accel;
}

void
MPObject::setConstant( const Value & size,
                       const Value & decay,
                       const Value & randp,
                       const Value & weight,
                       const Value & max_speed,
                       const Value & max_accel )
{
    M_size = size;
    M_decay = decay;
    M_randp = randp;
    M_weight = weight;
    M_max_speed = max_speed;
    M_max_accel = max_accel;
}

PVector
MPObject::noise()
{
    Value maxrnd = M_randp * vel().r();
    return PVector( drand( -maxrnd, maxrnd ),
                    drand( -maxrnd, maxrnd ) );
}

PVector
MPObject::wind()
{
    const Weather & w = M_stadium->weather();

    //if ( ! M_weather
    //|| M_weather->wind_rand < EPS )
    if ( w.wind_rand < EPS )
    {
        return PVector( 0.0, 0.0 );
    }

//     return PVector( M_vel.r() * ( M_weather->wind_vector.x +
//                                   drand( - M_weather->wind_rand, M_weather->wind_rand) ) /
//                     (M_weight * WIND_WEIGHT), M_vel.r() * ( M_weather->wind_vector.y +
//                                                             drand( - M_weather->wind_rand, M_weather->wind_rand ) ) /
//                     (M_weight * WIND_WEIGHT));
    const double speed = M_vel.r();
    return PVector( speed * ( w.wind_vector.x +
                              drand( - w.wind_rand, + w.wind_rand) ) /
                    (M_weight * WIND_WEIGHT),
                    speed * ( w.wind_vector.y +
                              drand( - w.wind_rand, + w.wind_rand ) ) /
                    (M_weight * WIND_WEIGHT));
}

void
MPObject::_inc()
{
    if ( M_accel.x || M_accel.y )
	  {
         double max_a = maxAccel();
         double max_s = maxSpeed();

         Value tmp = M_accel.r();
         if ( tmp > max_a )
             M_accel *= (max_a / tmp);
         M_vel += M_accel;
         tmp = M_vel.r();
         if ( tmp > max_s )
             M_vel *= (max_s / tmp);
    }

    updateAngle();

    M_vel += noise();
    M_vel += wind();

    CArea post = nearestPost( pos(), M_size );

    //      std::cout << "pos = " << pos << endl;
    //      std::cout << "nearest post = " << post << endl;
    //      std::cout << "dist = " << (pos - post.center).r() << endl;
    while ( ( pos() - post.center() ).r() < post.radius() )
    {
        //          std::cout << "In post\n";
        // then the ball has overlapped the post.  Either it was moved
        // there or "pushed".  Either way, we just move the ball away
        // from the post
        PVector diff = pos() - post.center();
        if ( diff == PVector() )
        {
            //boost::uniform_real<> dir( -M_PI, +M_PI );
            diff = PVector::fromPolar( post.radius(),
                                       //dir( rcss::random::DefaultRNG::instance() ) );
                                       drand( -M_PI, +M_PI ) );
        }
        else
        {
            diff.normalize( post.radius() );
        }

        M_pos = post.center() + diff;

        while ( ( pos() - post.center() ).r() < post.radius() )
        {
            // noise keeps it inside the post, move it a bit further out
            diff.normalize( diff.r() * 1.01 );
            M_pos = post.center() + diff;
        }

        if ( M_vel.x != 0.0 || M_vel.y != 0.0 )
        {
            PVector pos2center = post.center() - pos();
            M_vel.rotate( -pos2center.th() );
            M_vel.x = - M_vel.x;
            M_vel.rotate( pos2center.th() );
        }

        post = nearestPost( pos(), M_size );
        //          std::cout << "pos = " << pos << endl;
        //          std::cout << "nearest post = " << post << endl;
        //          std::cout << "dist = " << (pos - post.center).r() << endl;
    }

    PVector new_pos = pos() + M_vel;
    CArea second_post = nearestPost( new_pos, M_size );
    PVector inter;
    bool second = false;

    //      std::cout << "vel = " << vel << endl;
    //      std::cout << "new_pos = " << new_pos << endl;

    while ( pos() != new_pos
            && ( ( intersect( pos(), new_pos, post, inter ) )
                 || ( post != second_post
                      && ( second = intersect( pos(), new_pos, second_post, inter ) )
                      )
                 )
            )
    {
        //          std::cout << "Collision:\n"
        //                    << pos << std::endl;

        // handle collision
        M_pos = inter;

        //          std::cout << pos << std::endl;

        PVector rem = new_pos - pos();
        PVector coll_2_circle;
        if ( second )
        {
            coll_2_circle = second_post.center() - pos();
        }
        else
        {
            coll_2_circle = post.center() - pos();
        }

        //          std::cout << "rem = " << rem << endl;

        rem.rotate( -coll_2_circle.th() );
        rem.x = -rem.x;
        rem.rotate( coll_2_circle.th() );

        new_pos = pos() + rem;
        //          std::cout << "rem = " << rem << endl;


        // setup post and second post for next loop
        post = nearestPost( pos(), M_size );
        second_post = nearestPost( new_pos, M_size );
        //         std::cout << "pos = " << pos << endl;
        //          std::cout << "new_pos = " << new_pos << endl;
        //          std::cout << "nearest post = " << post << endl;
        //          std::cout << "dist = " << (pos - post.center).r() << endl;

        // setup vel so it will decay normally.  The collisions are
        // elastic, so the maginitude does not change, but the heading
        // does
        M_vel = PVector::fromPolar( M_vel.r(), rem.th() );
        //          std::cout << "vel = " << vel << endl;

        second = false;
    }

    M_pos = new_pos;
    M_vel *= M_decay;
    M_accel *= Zero;
}

// void MPObject::collide(MPObject& obj)
// {
//     Value r = size + obj.size;
//     PVector dif = (pos - obj.pos);
//     Value d = pos.distance(obj.pos);
//     Angle th = fabs(dif.angle(vel));
//     Value l1 = d * cos(th);
//     Value h = d * sin(th);
//     Value cosp = h / r;
//     Value sinp = sqrt(1.0 - square(cosp));
//     Value l2 = r * sinp;
//     PVector dv = vel;

//     dv.normalize(-(l1 + l2));

//     pos += dv;
// }


void
MPObject::moveToCollPos()
{
    if ( this->col_count > 0 )
    {
        /*        cout << "oldpos = " << obj->pos << std::endl; */
        /*        cout << "colpos = " << obj->post_col_pos << std::endl; */
        /*        cout << "colcount = " << obj->col_count << std::endl; */
        this->post_col_pos /= this->col_count;
        this->M_pos = this->post_col_pos;
        /*        cout << "newpos = " << obj->pos << std::endl; */
    }

    this->post_col_pos = PVector( 0, 0 );
    this->col_count = 0;
}


Ball::Ball( Stadium * stadium )
    : MPObject( stadium,
                PObject::OT_BALL,
                BALL_NAME, BALL_NAME_SHORT,
                O_TYPE_BALL_NAME, O_TYPE_BALL_NAME_SHORT )
{

}

/*
 *===================================================================
 *Part: Player & Team
 *===================================================================
 */


Team::Team( Stadium *stad, const Side s )
    : M_stadium( stad ),
      M_side( s ),
      M_size( 0 ),
      M_point( 0 ),
      M_pen_taken( 0 ),
      M_pen_point( 0 ),
      M_pen_won( false ),
      M_subs_count( 0 ),
      M_olcoach( static_cast< OnlineCoach * >( 0 ) )
{
    for ( int i = 0; i < PlayerParam::instance().playerTypes(); ++i )
    {
        ptype_count[ i ] = 0;
    }

#ifdef NEW_QSTEP
    if ( side() == LEFT )
    {
        M_dist_qstep_team = ServerParam::instance().dist_qstep_l;
        M_land_qstep_team = ServerParam::instance().land_qstep_l;
        M_dir_qstep_team  = ServerParam::instance().dir_qstep_l;
    }
    else
    {
        dist_qstep_team = ServerParam::instance().dist_qstep_r;
        land_qstep_team = ServerParam::instance().land_qstep_r;
        dir_qstep_team  = ServerParam::instance().dir_qstep_r;
    }
#endif
    // pfr 8/14/00: for RC2000 evaluation
    if ( ServerParam::instance().teamActuatorNoise() )
    {
        if ( side() == LEFT )
        {
            M_prand_factor_team     = ServerParam::instance().playerRandFactorLeft();
            M_kick_rand_factor_team = ServerParam::instance().kickRandFactorLeft();
        }
        else
        {
            M_prand_factor_team     = ServerParam::instance().playerRandFactorRight();
            M_kick_rand_factor_team = ServerParam::instance().kickRandFactorRight();
        }
    }

    for ( int i = 0; i < MAX_PLAYER; ++i )
    {
        M_players[i] = static_cast< Player * >( 0 );
    }
}

Team::~Team()
{
    for ( GraphCont::iterator i = M_graphics.begin(); i != M_graphics.end(); ++i )
        delete i->second;

}

Player*
Team::newPlayer( Value version, int goalie_flag )
{
    if ( size() >= MAX_PLAYER )
    {
        if ( ServerParam::instance().verboseMode() )
            std::cerr << "Warning:Too many players." << std::endl;
        return NULL;
    }

    if ( goalie_flag )
    {
        for ( int i = 0; i < size(); ++i )
        {
            if ( M_players[i]->isGoalie() )
            {
                if ( ServerParam::instance().verboseMode() )
                    std::cerr << "Warning:Too many goalies." << std::endl;
                return NULL;
            }
        }
    }

    Player *p = M_players[size()];

    p->init( version, goalie_flag );

    if ( ! p->setSenders() )
    {
        std::cerr << "Error: Could not find serializer or sender for version "
                  << (int)( p->version() ) << std::endl;
        return NULL;
    }

    ++M_size;
    ++ptype_count[ 0 ];

    return p;
}

void
Team::assignPlayer( int n, Player * player )
{
    if ( n < 0 || MAX_PLAYER <= n )
    {
        std::cerr << "Error: Too many players are assigned." << std::endl;
        return;
    }
    M_players[n] = player;
}

void
Team::assignCoach( OnlineCoach * coach )
{
    M_olcoach = coach;
}

int
Team::ptypeCount( const int player_type ) const
{
    std::map< int, int >::const_iterator it = ptype_count.find( player_type );
    if ( it == ptype_count.end() )
    {
        return 0;
    }
    return it->second;
}


void
Team::substitute( const Player * player,
                  const int player_type )
{
    if ( ! player )
    {
        return;
    }

    std::map< int, int >::iterator old_it = ptype_count.find( player->playerTypeId() );
    std::map< int, int >::iterator new_it = ptype_count.find( player_type );

    if ( old_it == ptype_count.end()
         || new_it == ptype_count.end() )
    {
        return;
    }

    if ( old_it->second <= 0 )
    {
        return;
    }

    Player * candidate = NULL;
    for ( int i = 0; i < this->size(); ++i )
    {
        if ( player == M_players[i] )
        {
            candidate = M_players[i];
            break;
        }
    }

    if ( ! candidate )
    {
        std::cerr << __FILE__ << ":" << __LINE__
                  << " Warning: no such player." << std::endl;
        return;
    }

    old_it->second -= 1;

    candidate->substitute( player_type );

    if ( M_stadium->playmode() != PM_BeforeKickOff )
    {
        ++M_subs_count;
    }

    new_it->second += 1;
}


void
Team::addGraphic( unsigned int x,
                  unsigned int y,
                  std::auto_ptr< XPMHolder > holder )
{
    GraphKey key( x, y );
    GraphCont::iterator i = M_graphics.find( key );
    if( i != M_graphics.end() )
        delete i->second;

    M_graphics[ key ] = holder.release();
}
