/* -*- Mode: C++ -*- */

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

#ifndef HETEROPLAYER_H
#define HETEROPLAYER_H

#include "types.h"

#include <iostream>

class HeteroPlayer {
public:
    HeteroPlayer();
    HeteroPlayer( int );
    ~HeteroPlayer();

    const double & playerSpeedMax() const { return player_speed_max; }
    const double & staminaIncMax() const { return stamina_inc_max; }
    const double & playerDecay() const { return player_decay; }
    const double & inertiaMoment() const { return inertia_moment; }
    const double & dashPowerRate() const { return dash_power_rate; }
    const double & playerSize() const { return player_size; }
    const double & kickableMargin() const { return kickable_margin; }
    const double & kickRand() const { return kick_rand; }
    const double & extraStamina() const { return extra_stamina; }
    const double & effortMax() const { return effort_max; }
    const double & effortMin() const { return effort_min; }

    std::ostream & print( std::ostream & o ) const;

    player_type_t convertToStruct ( int id ) const;

private:

    double delta( const double & min,
                  const double & max );

    double player_speed_max;
    double stamina_inc_max;
    double player_decay;
    double inertia_moment;
    double dash_power_rate;
    double player_size;
    double kickable_margin;
    double kick_rand;
    double extra_stamina;
    double effort_max;
    double effort_min;

};

inline
std::ostream &
operator<<( std::ostream & o,
            const HeteroPlayer & hp )
{
    return hp.print( o );
}

#endif
