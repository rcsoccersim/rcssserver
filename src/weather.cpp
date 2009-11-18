// -*-c++-*-

/***************************************************************************
                              weather.h
                       Class for weather effects control
                             -------------------
    begin                : 2009-11-18
    copyright            : (C) 2009 by The RoboCup Soccer Server
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "weather.h"

#include "random.h"
#include "serverparam.h"

#include <cstdio>

/*
 *===================================================================
 *Part: Weather
 *===================================================================
 */

Weather::Weather()
    : M_wind_vector( 0.0, 0.0 ),
      M_wind_rand( 0.0 )
{

}


Weather::~Weather()
{

}


void
Weather::init()
{
    if ( ServerParam::instance().windNone() )
    {
        M_wind_vector = PVector( 0.0, 0.0 );
        M_wind_rand = 0.0;
    }
    else if ( ServerParam::instance().windRandom() )
    {
        M_wind_vector = PVector::fromPolar( drand( 0, 100 ), drand( -M_PI, +M_PI ) );
        M_wind_rand = drand( 0.0, 0.5 );
    }
    else
    {
        M_wind_vector = PVector::fromPolar( ServerParam::instance().windForce(),
                                            Deg2Rad( ServerParam::instance().windDir() ) );
        M_wind_rand = ServerParam::instance().windRand();
    }

#if 1
    std::printf( "wind factor: rand: %f, vector: (%f, %f)\n",
                 M_wind_rand, M_wind_vector.x, M_wind_vector.y );
#endif

}


void
Weather::halfTime()
{
    M_wind_vector *= -1.0;
}
