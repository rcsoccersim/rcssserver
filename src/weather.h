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

#ifndef RCSSSERVER_WEATHER_H
#define RCSSSERVER_WEATHER_H

#include "object.h"

class Weather {
private:

    PVector	M_wind_vector;
    double M_wind_rand;

    const Weather & operator=( const Weather & );
public:
    Weather();
    ~Weather();

    void init();
    void halfTime();

    const PVector & windVector() const { return M_wind_vector; }
    const double & windRand() const { return M_wind_rand; }
};

#endif
