/* -*- Mode: C++ -*-
 *Header:
 *File: field.C
 *Author: Noda Itsuki
 *Date: 1996/02/14
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

#include "field.h"

#include "serverparam.h"

/*
 *===================================================================
 *Part: Field
 *===================================================================
 */
Field::Field()
    : line_l( LINE_L_NAME, LINE_L_NAME_SHORT, "(Line)", "(L)",
              PVector( - ServerParam::PITCH_LENGTH / 2.0, 0.0 ) ),
      line_r( LINE_R_NAME, LINE_R_NAME_SHORT, "(Line)", "(L)",
              PVector( + ServerParam::PITCH_LENGTH / 2.0, 0.0 ) ),
      line_t( LINE_T_NAME, LINE_T_NAME_SHORT, "(Line)", "(L)",
              PVector( - ServerParam::PITCH_WIDTH / 2.0, 0.0 ) ),
      line_b( LINE_B_NAME, LINE_B_NAME_SHORT, "(Line)", "(L)",
              PVector( + ServerParam::PITCH_WIDTH / 2.0, 0.0 ) )

{

}

Field::~Field()
{
    M_goals.clear();

    for ( std::vector< PObject * >::iterator i = M_landmarks.begin();
          i != M_landmarks.end();
          ++i )
    {
        delete *i;
        *i = static_cast< PObject * >( 0 );
    }
    M_landmarks.clear();
}

void
Field::addLandmark( PObject * new_obj )
{
    if ( new_obj )
    {
        if ( new_obj->closeName() == O_TYPE_GOAL_NAME )
        {
            M_goals.push_back( new_obj );
        }
        M_landmarks.push_back( new_obj );
    }
}
