// -*-c++-*-

/***************************************************************************
                                   arm.h
  Classes for an arm actuator that can be used to point at field locations
                             -------------------
    begin                : 15-JAN-2002
    copyright            : (C) 2001 by The RoboCup Soccer Server
                           Maintenance Group.
    email                : sserver-admin@lists.sourceforge.net
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LGPL as published by the Free Software  *
 *   Foundation; either version 3 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#ifndef ARM_H
#define ARM_H

#include "vector.h"
#include "utility.h"

class Arm {
protected:
    rcss::geom::Vector2D M_dest;

    unsigned int M_timer;
    unsigned int M_min_point_duration;
    unsigned int M_max_point_duration;

    unsigned int M_count;

public:

    Arm( const unsigned int min_point_duration,
         const unsigned int max_point_duration )
        : M_timer ( max_point_duration ),
          M_min_point_duration ( min_point_duration ),
          M_max_point_duration ( max_point_duration ),
          M_count ( 0 )
      { }

    ~Arm()
      { }

    const
    rcss::geom::Vector2D & dest() const
      {
          return M_dest;
      }

    bool isMovable() const
      {
          return M_timer >= M_min_point_duration;
      }

    bool isPointing() const
      {
          return M_timer < M_max_point_duration;
      }

    void incAge()
      {
          ++M_timer;
      }

    bool pointTo( const rcss::geom::Vector2D & origin,
                  const double & orientation,
                  const rcss::geom::Vector2D & rel_dest )
      {
          if ( isMovable() )
          {
              M_dest = rel_dest;
              M_dest.rotate( orientation );
              M_dest += origin;
              M_timer = 0;
              M_count++;
              return true;
          }
          return false;
      }

    bool stopPointing()
      {
          if ( isMovable() )
          {
              M_dest.null();
              M_timer = M_max_point_duration;
              M_count++;
              return true;
          }
          return false;
      }

    bool getRelDest( const rcss::geom::Vector2D & origin,
                     const double & orientation,
                     rcss::geom::Vector2D & rel_dest ) const
      {
          if ( isPointing() )
          {
              rel_dest = M_dest;
              rel_dest -= origin;
              rel_dest.rotate ( -orientation );
              return true;
          }
          return false;
      }

    bool getRelDir( const rcss::geom::Vector2D & origin,
                    const double & orientation,
                    double & rel_dir ) const
      {
          rcss::geom::Vector2D rel_dest;
          if ( getRelDest( origin, orientation, rel_dest ) )
          {
              rel_dir = rel_dest.getHead ();
              return true;
          }
          return false;
      }

    unsigned int getCyclesTillExpiry() const
      {
          return ( isPointing () ? M_max_point_duration - M_timer : 0 );
      }

    unsigned int getCyclesTillMovable() const
      {
          return ( isMovable () ? 0 : M_min_point_duration - M_timer );
      }

    unsigned int getCounter() const
      {
          return M_count;
      }

    class State {
    protected:
        unsigned int M_cycles_till_movable;
        unsigned int M_cycles_till_expires;
        double M_dist;
        double M_head;
        unsigned int M_count;

    public:
        State ( const rcss::geom::Vector2D & origin,
                const double & orientation,
                const Arm & arm )
             : M_cycles_till_movable( arm.getCyclesTillMovable() )
             , M_cycles_till_expires( arm.getCyclesTillExpiry() )
             , M_dist( 0 )
             , M_head( 0 )
             , M_count( arm.getCounter() )
          {
              rcss::geom::Vector2D target;
              arm.getRelDest( origin, orientation, target );
              M_dist = rint( target.getMag() * 10.0 ) * 0.1;
              M_head = rint( Rad2Deg( target.getHead() ) * 10.0 ) * 0.1;
          }

        ~State()
          { }

        unsigned int cyclesTillMovable () const { return M_cycles_till_movable; }
        unsigned int cyclesTillExpires () const { return M_cycles_till_expires; }
        double dist () const { return M_dist; }
        double head () const { return M_head; }
        unsigned int count () const { return M_count; }
    };

    State getState( const rcss::geom::Vector2D & origin,
                    const double & orientation ) const
      {
          return State( origin, orientation, *this );
      }

};


#endif
