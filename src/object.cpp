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
 Copyright (C) 2000,2001 RoboCup Soccer Server Maintainance Group.
 Patrick Riley, Tom Howard, Daniel Polani, Itsuki Noda,
 Mikhail Prokopenko, Jan Wendler
 Copyright (C) 2002- RoboCup Soccer Simulator Maintainance Group.

 This file is a part of SoccerServer.

 This code is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 3 of the License, or (at your option) any later version.

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
#include <config.h>
#endif

#include "object.h"

#include "stadium.h"
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

bool
PVector::between( const PVector & begin,
                  const PVector & end ) const
{
    if ( begin.x > end.x )
    {
        return between( end, begin );
    }

    if ( begin.x <= x && x <= end.x )
    {
        if ( begin.y < end.y )
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
    return PVector( std::min( std::max( p.x, left() ), right() ),
                    ( std::fabs( p.y - top() ) < std::fabs( p.y - bottom() )
                      ? top()
                      : bottom() ) );
}

PVector
RArea::nearestVEdge( const PVector & p ) const
{
    return PVector( ( std::fabs( p.x - left() ) < std::fabs( p.x - right() )
                      ? left()
                      : right() ),
                    std::min( std::max( p.y, top() ), bottom() ) );
}

PVector
RArea::nearestEdge( const PVector & p ) const
{
    if ( std::min( std::fabs( p.x - left() ), std::fabs( p.x - right() ) )
         < std::min( std::fabs( p.y - top() ), std::fabs( p.y - bottom() ) ) )
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
    return PVector( drand( left(), right() ),
                    drand( bottom(), top() ) );
}

std::ostream &
RArea::print( std::ostream& o ) const
{
    return o << "#A[h:" << left() << "~" << right()
             << ",v:" << top() << "~" << bottom() << "]";
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
    {
        return false;
    }

    if ( ( begin - end ).r() < ( begin - circle.center() ).r() - circle.radius() )
    {
        // object wont get within circles range
        return false;
    }

    if ( circle.center() == PVector() )
    {
        double dx = end.x - begin.x;
        double dy = end.y - begin.y;
        //          std::cout << dx << endl;
        //          std::cout << dy << endl;
        double dr = std::sqrt( dx*dx + dy*dy );
        //          std::cout << dr << endl;
        double D = begin.x * end.y - end.x * begin.y;
        double descrim = circle.radius() * circle.radius() * dr*dr - D*D;
        //          std::cout << descrim << endl;
        if ( descrim <= 0.0 )
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

            if ( ! first.between( begin, end )
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
             const double & size )
{
    PVector nearest_gpost;

    if ( pos.y > 0 )
    {
        if ( pos.x > 0 )
        {
            nearest_gpost = PVector( ServerParam::PITCH_LENGTH*0.5
                                     - ServerParam::instance().goalPostRadius(),
                                     ServerParam::instance().goalWidth()*0.5
                                     + ServerParam::instance().goalPostRadius() );
        }
        else
        {
            nearest_gpost = PVector( - ServerParam::PITCH_LENGTH*0.5
                                     + ServerParam::instance().goalPostRadius(),
                                     ServerParam::instance().goalWidth()*0.5
                                     + ServerParam::instance().goalPostRadius() );
        }
    }
    else
    {
        if ( pos.x > 0 )
        {
            nearest_gpost = PVector( ServerParam::PITCH_LENGTH*0.5
                                     - ServerParam::instance().goalPostRadius(),
                                     - ServerParam::instance().goalWidth()*0.5
                                     - ServerParam::instance().goalPostRadius() );
        }
        else
        {
            nearest_gpost = PVector( - ServerParam::PITCH_LENGTH*0.5
                                     + ServerParam::instance().goalPostRadius(),
                                     - ServerParam::instance().goalWidth()*0.5
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

int PObject::S_object_count = 0;

/* pfr 06/07/200 added short name support */
PObject::PObject( const std::string & name,
                  const std::string & short_name,
                  const std::string & close_name,
                  const std::string & short_close_name,
                  const PVector & p,
                  const double & v )
    : M_id( S_object_count ),
      M_name( name ),
      M_short_name( short_name ),
      M_close_name( close_name ),
      M_short_close_name( short_close_name ),
      M_object_version( v ),
      M_size( 1.0 ),
      M_pos( p ),
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
MPObject::MPObject( Stadium & stadium,
                    const std::string & name,
                    const std::string & short_name,
                    const std::string & close_name,
                    const std::string & short_close_name )
    : PObject( name, short_name,
               close_name, short_close_name )
    , M_stadium( stadium )
    , M_vel( 0.0,0.0 )
    , M_accel( 0.0,0.0 )
{
    //assert( stadium );
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
                  const PVector & vel,
                  const PVector & accel )
{
    M_pos = pos;
    M_vel = vel;
    M_accel = accel;
}

void
MPObject::setConstant( const double & size,
                       const double & decay,
                       const double & randp,
                       const double & weight,
                       const double & max_speed,
                       const double & max_accel )
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
    double maxrnd = M_randp * vel().r();
    return PVector::fromPolar( drand( 0.0, maxrnd ),
                               drand( -M_PI, M_PI ) );
    //return PVector( drand( -maxrnd, maxrnd ),
    //                drand( -maxrnd, maxrnd ) );
}

PVector
MPObject::wind()
{
    const Weather & w = M_stadium.weather();

    if ( w.windRand() < EPS )
    {
        return PVector( 0.0, 0.0 );
    }

    const double speed = M_vel.r();
    return PVector( speed * ( w.windVector().x +
                              drand( - w.windRand(), + w.windRand() ) ) /
                    ( M_weight * ServerParam::instance().windWeight() ),
                    speed * ( w.windVector().y +
                              drand( - w.windRand(), + w.windRand() ) ) /
                    ( M_weight * ServerParam::instance().windWeight() ));
}

void
MPObject::_inc()
{
    if ( M_accel.x || M_accel.y )
    {
        double max_a = maxAccel();
        double max_s = maxSpeed();

        double tmp = M_accel.r();
        if ( tmp > max_a )
        {
            M_accel *= ( max_a / tmp );
        }

        M_vel += M_accel;
        tmp = M_vel.r();
        if ( tmp > max_s )
        {
            M_vel *= ( max_s / tmp );
        }
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
            diff = PVector::fromPolar( post.radius(),
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
        //         std::cout << M_stadium.time() << ": Colliding with post\n"
        //                   << "  pos = " << pos() << '\n'
        //                   << "  vel = " << vel() << '\n'
        //                   << "  nearest post = " << post.center() << '\n'
        //                   << "  dist = " << ( pos() - post.center() ).r() << std::endl;

        collidedWithPost();
    }

    PVector new_pos = pos() + M_vel;
    CArea second_post = nearestPost( new_pos, M_size );
    PVector inter;
    bool second = false;

    //      std::cout << "vel = " << vel << endl;
    //      std::cout << "new_pos = " << new_pos << endl;
    //int loop_count = 0;
    while ( pos() != new_pos
            && ( ( intersect( pos(), new_pos, post, inter ) )
                 || ( post != second_post
                      && ( second = intersect( pos(), new_pos, second_post, inter ) )
                      )
                 )
            )
    {
        //         ++loop_count;
        //         std::cout << M_stadium.time() <<": Collision: " << loop_count << "\n"
        //                   << "  pos=" << pos() << '\n';

        // handle collision
        M_pos = inter;

        //         std::cout << "  intersect=" << pos() << '\n';

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

        // 2008-05-22 akiyama
        // fixed endless-loop bug.
        // If this small vector is not added to M_pos, intersect() may still
        // return pos() as the intersect point.
        M_pos += PVector::fromPolar( EPS, coll_2_circle.th() + 180.0 );

        //         std::cout << "  rem = " << rem << '\n';

        rem.rotate( -coll_2_circle.th() );
        rem.x = -rem.x;
        rem.rotate( coll_2_circle.th() );

        new_pos = pos() + rem;
        //         std::cout << "  rem = " << rem << '\n';

        // setup post and second post for next loop
        post = nearestPost( pos(), M_size );
        second_post = nearestPost( new_pos, M_size );

        //         std::cout << "  pos = " << pos() << '\n'
        //                   << "  new_pos = " << new_pos << '\n'
        //                   << "  nearest post = " << post.center() << '\n'
        //                   << "  dist = " << ( pos() - post.center() ).r() << '\n';

        // setup vel so it will decay normally.  The collisions are
        // elastic, so the maginitude does not change, but the heading
        // does
        M_vel = PVector::fromPolar( M_vel.r(), rem.th() );
        //         std::cout << "  vel = " << vel() << std::endl;

        second = false;

        collidedWithPost();
    }

    M_pos = new_pos;
    M_vel *= M_decay;
    M_accel *= 0.0;
}

// void MPObject::collide(MPObject& obj)
// {
//     double r = size + obj.size;
//     PVector dif = (pos - obj.pos);
//     double d = pos.distance(obj.pos);
//     Angle th = fabs(dif.angle(vel));
//     double l1 = d * cos(th);
//     double h = d * sin(th);
//     double cosp = h / r;
//     double sinp = sqrt(1.0 - square(cosp));
//     double l2 = r * sinp;
//     PVector dv = vel;

//     dv.normalize(-(l1 + l2));

//     pos += dv;
// }


void
MPObject::moveToCollisionPos()
{
    if ( M_collision_count > 0 )
    {
        /*        cout << "oldpos = " << obj->pos << std::endl; */
        /*        cout << "colpos = " << obj->post_col_pos << std::endl; */
        /*        cout << "colcount = " << obj->col_count << std::endl; */
        M_post_collision_pos /= M_collision_count;
        M_pos = M_post_collision_pos;
        /*        cout << "newpos = " << obj->pos << std::endl; */
    }

    M_post_collision_pos.assign( 0.0, 0.0 );
    M_collision_count = 0;
}


Ball::Ball( Stadium & stadium )
    : MPObject( stadium,
                BALL_NAME, BALL_NAME_SHORT,
                O_TYPE_BALL_NAME, O_TYPE_BALL_NAME_SHORT )
{

}
