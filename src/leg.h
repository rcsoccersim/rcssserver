// -*-c++-*-


/***************************************************************************
    leg.h
    -------------------
    begin                : 23-OCT-2023
    copyright            : (C) 2023 by The RoboCup Soccer Simulator
                           Maintenance Group.
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LGPL as published by the Free Software  *
 *   Foundation; either version 3 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#ifndef RCSSSERVER_LEG_H
#define RCSSSERVER_LEG_H

#include "object.h"

class Player;

/*!
  \todo inherits an abstract actuator class
*/
class Leg {
private:
    const Player & M_player;

    bool M_command_done;

    PVector M_accel;
    double M_consumed_stamina;

    Leg() = delete;
    Leg( const Leg & ) = delete;
    const Leg & operator=( const Leg & ) = delete;

public:
    explicit
    Leg( const Player & player );

    void resetCommand();

    bool commandDone() const
      {
          return M_command_done;
      }

    void dash( const double power,
               const double dir );

    const PVector & accel() const
      {
          return M_accel;
      }

    double consumedStamina() const
      {
          return M_consumed_stamina;
      }
};

#endif
