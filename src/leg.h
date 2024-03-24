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
public:
    enum CommandType {
        MOVE,
        DASH,
        TURN,
        KICK,
        TACKLE,
        NONE
    };

private:
    const Player & M_player;

    CommandType M_command_type;

    double M_kick_power;
    double M_kick_dir;

    double M_dash_power;
    double M_dash_dir;

    Leg() = delete;
    Leg( const Leg & ) = delete;
    const Leg & operator=( const Leg & ) = delete;

public:
    explicit
    Leg( const Player & player );

    void resetCommand();

    void move();

    void kick( const double power,
               const double dir );

    void turn();

    void dash( const double power,
               const double dir );

    void tackle();


    bool commandDone() const
      {
          return M_command_type != NONE;
      }

    CommandType commandType() const
      {
          return M_command_type;
      }

    double kickPower() const
      {
          return M_kick_power;
      }

    double kickDir() const
      {
          return M_kick_dir;
      }

    double dashPower() const
      {
          return M_dash_power;
      }

    double dashDir() const
      {
          return M_dash_dir;
      }

    PVector calcDashAccel( const double consumed_stamina ) const;
};

#endif
