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

    double playerSpeedMax() const { return M_player_speed_max; }
    double staminaIncMax() const { return M_stamina_inc_max; }
    double playerDecay() const { return M_player_decay; }
    double inertiaMoment() const { return M_inertia_moment; }
    double dashPowerRate() const { return M_dash_power_rate; }
    double playerSize() const { return M_player_size; }
    double kickableMargin() const { return M_kickable_margin; }
    double kickRand() const { return M_kick_rand; }
    double extraStamina() const { return M_extra_stamina; }
    double effortMax() const { return M_effort_max; }
    double effortMin() const { return M_effort_min; }

    double kickPowerRate() const { return M_kick_power_rate; }
    double foulDetectProbability() const { return M_foul_detect_probability; }
    double catchAreaLengthStretch() const { return M_catchable_area_l_stretch; }

    std::ostream & print( std::ostream & o ) const;

    player_type_t convertToStruct( int id ) const;

    void serializeParams( std::ostream & o,
                          const unsigned int version,
                          const int id ) const;
private:

    double delta( const double & min,
                  const double & max );
    void setDefault();

    double M_player_speed_max;
    double M_stamina_inc_max;
    double M_player_decay;
    double M_inertia_moment;
    double M_dash_power_rate;
    double M_player_size;
    double M_kickable_margin;
    double M_kick_rand;
    double M_extra_stamina;
    double M_effort_max;
    double M_effort_min;

    // v14
    double M_kick_power_rate;
    double M_foul_detect_probability;
    double M_catchable_area_l_stretch;

};

inline
std::ostream &
operator<<( std::ostream & o,
            const HeteroPlayer & hp )
{
    return hp.print( o );
}

#endif
