// -*-c++-*-
/*
 *Header:
 *File: field.h (for C++)
 *Author: Noda Itsuki
 *Date: 1996/02/16
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


#ifndef RCSSSERVER_FIELD_H
#define RCSSSERVER_FIELD_H

#include "timeable.h"

#include "object.h"
#include "weather.h"
#include "logger.h"

#include <rcssbase/gzip/gzfstream.hpp>
#include <rcssbase/net/udpsocket.hpp>

#include <string>
#include <vector>
#include <list>
#include <memory>
#include <cstdio>

class HeteroPlayer;
class XPMHolder;

class Monitor;
class Player;
class Coach;
class OnlineCoach;
class Team;

class Referee;
class ResultSaver;

struct timeval;

namespace rcss {
class Listener;
namespace clang {
class Msg;
}
}

class Field {
private:
    const Field & operator=( const Field & );

    std::vector< const PObject * > M_goals;
    std::vector< PObject * > M_landmarks;
public:
    const PObject line_l;
    const PObject line_r;
    const PObject line_t;
    const PObject line_b;

    Field();
    ~Field();

    const
    std::vector< const PObject * > & goals() const
      {
          return M_goals;
      }

    const
    std::vector< PObject * > & landmarks() const
      {
          return M_landmarks;
      }

    void addLandmark( PObject * new_obj );
};

#endif
