/* -*- Mode: C++ -*- */

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

#ifndef _HETROPLAYER_H_
#define _HETROPLAYER_H_

#include "types.h"
#include <cstdlib>
#include <sys/time.h>
#include "serverparam.h"
#include "playerparam.h"

#include <iostream>

class HetroPlayer
{
public:
    HetroPlayer();
    HetroPlayer( int );
    ~HetroPlayer();

    const Value & playerSpeedMax() const { return player_speed_max; }
    const Value & staminaIncMax() const { return stamina_inc_max; }
    const Value & playerDecay() const { return player_decay; }
    const Value & inertiaMoment() const { return inertia_moment; }
    const Value & dashPowerRate() const { return dash_power_rate; }
    const Value & playerSize() const { return player_size; }
    const Value & kickableMargin() const { return kickable_margin; }
    const Value & kickRand() const { return kick_rand; }
    const Value & extraStamina() const { return extra_stamina; }
    const Value & effortMax() const { return effort_max; }
    const Value & effortMin() const { return effort_min; }

    std::ostream & print( std::ostream & o ) const;

    player_type_t convertToStruct ( int id );

private:

    Value delta( const Value & min,
                 const Value & max );

    Value player_speed_max;
    Value stamina_inc_max;
    Value player_decay;
    Value inertia_moment;
    Value dash_power_rate;
    Value player_size;
    Value kickable_margin;
    Value kick_rand;
    Value extra_stamina;
    Value effort_max;
    Value effort_min;

};

inline
std::ostream &
operator<<( std::ostream & o,
            const HetroPlayer & hp )
{
    return hp.print( o );
}

#endif
