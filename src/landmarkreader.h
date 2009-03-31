/* -*- Mode: c++ -*- */

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

#ifndef RCSS_LANDMARKREADER_H
#define RCSS_LANDMARKREADER_H

#ifdef HAVE_LIBEXPAT
#include "xmlreader.h"
#endif // HAVE_LIBEXPAT

#include <iostream>
#include <string>
#include <stack>

class LandmarkReader;
class Field;

class LandmarkReader
#ifdef HAVE_LIBEXPAT
    : public XmlReader
#endif //HAVE_LIBEXPAT
{
public:
    LandmarkReader( Field & field,
                    const std::string & path_name = "" );
    ~LandmarkReader()
      { }

    std::ostream & print( std::ostream & o ) const;

private:
    struct rect_t {
        double startx;
        double starty;
        double endx;
        double endy;

        LandmarkReader::rect_t percentify( const LandmarkReader::rect_t & rect,
                                           const bool & dostartx,
                                           const bool & dostarty,
                                           const bool & doendx,
                                           const bool & doendy ) const
          {
              LandmarkReader::rect_t dest = *this;
              if ( dostartx )
              {
                  dest.startx *= 0.01;
                  dest.startx = ( rect.startx * ( 1 - dest.startx )
                                  + rect.endx * ( 1 + dest.startx ) ) * 0.5;
              }
              if ( dostarty )
              {
                  dest.starty *= 0.01;
                  dest.starty = ( rect.starty * ( 1 - dest.starty )
                                  + rect.endy * ( 1 + dest.starty ) ) * 0.5;
              }
              if ( doendx )
              {
                  dest.endx *= 0.01;
                  dest.endx = ( rect.startx * ( 1 - dest.endx )
                                + rect.endx * ( 1 + dest.endx ) ) * 0.5;
              }
              if ( doendy )
              {
                  dest.endy *= 0.01;
                  dest.endy = ( rect.starty * ( 1 - dest.endy )
                                + rect.endy * ( 1 + dest.endy ) ) * 0.5;
              }
              return dest;
          }
    };

    struct flag_t {
        double x;
        double y;
        std::string name;
        double ver;

        flag_t()
          { }

        flag_t( double xval,
                double yval,
                const std::string & flagname,
                double flagver )
            : x( xval ),
              y( yval ),
              name( flagname ),
              ver( flagver )
          {}

        std::ostream & toXml( std::ostream & o,
                              bool x_per = true,
                              bool y_per = true ) const
          {
              return o << "<flag x=\"" << x << ( x_per ? "%" : "" )
                       << "\" y=\"" << y << ( y_per ? "%" : "" )
                       << "\" name=\"" << name
                       << "\" ver=\"" << ver
                       << "\"/>";
          }

        LandmarkReader::flag_t percentify ( const LandmarkReader::rect_t & rect,
                                            const bool & dox,
                                            const bool & doy ) const
          {
              LandmarkReader::flag_t dest = *this;
              if ( dox )
              {
                  dest.x *= 0.01;
                  dest.x = ( rect.startx * ( 1 - dest.x )
                             + rect.endx * ( 1 + dest.x ) ) * 0.5;
              }
              if ( doy )
              {
                  dest.y *= 0.01;
                  dest.y = ( rect.starty * ( 1 - dest.y )
                             + rect.endy * ( 1 + dest.y ) ) * 0.5;
              }
              return dest;
          }
    };

    struct goal_mouth_t {
        char side;
        double starty;
        double endy;
        std::ostream & toXml ( std::ostream & o ) const
          {
              return o << "<goalMouth side=\"" << side
                       << "\" starty=\"" << starty
                       << "\" endy=\"" << endy
                       << "\">";
          }
        LandmarkReader::rect_t toRect( const LandmarkReader::rect_t & rect ) const
          {
              LandmarkReader::rect_t dest;
              if ( side == 'l' )
              {
                  dest.startx = dest.endx = rect.startx;
              }
              else
              {
                  dest.startx = dest.endx = rect.endx;
              }
              dest.starty = starty;
              dest.endy = endy;
              return dest;
          }
    };

    static const char FLAG_KW[];
    static const char GOAL_KW[];
    static const char MAINPITCH_KW[];
    static const char OUTERPITCH_KW[];
    static const char GOALMOUTH_KW[];
    static const char PENALTYAREA_KW[];

    static const char X_KW[];
    static const char Y_KW[];
    static const char STARTX_KW[];
    static const char STARTY_KW[];
    static const char ENDX_KW[];
    static const char ENDY_KW[];
    static const char NAME_KW[];
    static const char VER_KW[];
    static const char SIDE_KW[];

    static const char SIDE_L;
    static const char SIDE_R;


    static const LandmarkReader::flag_t DEF_FLAG;


    // standard landmarks
    static const LandmarkReader::rect_t DEF_PITCH;
    static const LandmarkReader::flag_t FLAG_C;
    static const LandmarkReader::flag_t FLAG_C_T;
    static const LandmarkReader::flag_t FLAG_C_B;
    static const LandmarkReader::flag_t FLAG_R_T;
    static const LandmarkReader::flag_t FLAG_R_B;
    static const LandmarkReader::flag_t FLAG_L_T;
    static const LandmarkReader::flag_t FLAG_L_B;
    static const LandmarkReader::goal_mouth_t GOAL_MOUTH_R;
    static const LandmarkReader::goal_mouth_t GOAL_MOUTH_L;
    static const LandmarkReader::flag_t FLAG_G_R_B;
    static const LandmarkReader::flag_t GOAL_R;
    static const LandmarkReader::flag_t FLAG_G_R_T;
    static const LandmarkReader::flag_t FLAG_G_L_B;
    static const LandmarkReader::flag_t GOAL_L;
    static const LandmarkReader::flag_t FLAG_G_L_T;
    static const LandmarkReader::rect_t PEN_R;
    static const LandmarkReader::flag_t FLAG_P_R_B;
    static const LandmarkReader::flag_t FLAG_P_R_C;
    static const LandmarkReader::flag_t FLAG_P_R_T;
    static const LandmarkReader::rect_t PEN_L;
    static const LandmarkReader::flag_t FLAG_P_L_B;
    static const LandmarkReader::flag_t FLAG_P_L_C;
    static const LandmarkReader::flag_t FLAG_P_L_T;
    static const LandmarkReader::rect_t OUTER_PITCH;
    static const LandmarkReader::flag_t FLAG_T_0;
    static const LandmarkReader::flag_t FLAG_T_R_10;
    static const LandmarkReader::flag_t FLAG_T_R_20;
    static const LandmarkReader::flag_t FLAG_T_R_30;
    static const LandmarkReader::flag_t FLAG_T_R_40;
    static const LandmarkReader::flag_t FLAG_T_R_50;
    static const LandmarkReader::flag_t FLAG_T_L_10;
    static const LandmarkReader::flag_t FLAG_T_L_20;
    static const LandmarkReader::flag_t FLAG_T_L_30;
    static const LandmarkReader::flag_t FLAG_T_L_40;
    static const LandmarkReader::flag_t FLAG_T_L_50;
    static const LandmarkReader::flag_t FLAG_B_0;
    static const LandmarkReader::flag_t FLAG_B_R_10;
    static const LandmarkReader::flag_t FLAG_B_R_20;
    static const LandmarkReader::flag_t FLAG_B_R_30;
    static const LandmarkReader::flag_t FLAG_B_R_40;
    static const LandmarkReader::flag_t FLAG_B_R_50;
    static const LandmarkReader::flag_t FLAG_B_L_10;
    static const LandmarkReader::flag_t FLAG_B_L_20;
    static const LandmarkReader::flag_t FLAG_B_L_30;
    static const LandmarkReader::flag_t FLAG_B_L_40;
    static const LandmarkReader::flag_t FLAG_B_L_50;
    static const LandmarkReader::flag_t FLAG_R_0;
    static const LandmarkReader::flag_t FLAG_R_T_10;
    static const LandmarkReader::flag_t FLAG_R_T_20;
    static const LandmarkReader::flag_t FLAG_R_T_30;
    static const LandmarkReader::flag_t FLAG_R_B_10;
    static const LandmarkReader::flag_t FLAG_R_B_20;
    static const LandmarkReader::flag_t FLAG_R_B_30;
    static const LandmarkReader::flag_t FLAG_L_0;
    static const LandmarkReader::flag_t FLAG_L_T_10;
    static const LandmarkReader::flag_t FLAG_L_T_20;
    static const LandmarkReader::flag_t FLAG_L_T_30;
    static const LandmarkReader::flag_t FLAG_L_B_10;
    static const LandmarkReader::flag_t FLAG_L_B_20;
    static const LandmarkReader::flag_t FLAG_L_B_30;


    void loadDefaults();

    void addFlag( const LandmarkReader::flag_t & flag,
                  const bool goal = false );

#ifdef HAVE_LIBEXPAT
    void start( const char * el,
                const char ** attr );
    void end( const char * el );

    bool processFlag( const char ** attr,
                      bool goal = false );
    bool processPitch( const char ** attr,
                       bool main = true );
    bool processGoalMouth ( const char ** attr );

#endif //HAVE_LIBEXPAT

    Field & M_field;

    std::stack < LandmarkReader::rect_t > rect_stack;
    LandmarkReader::rect_t pitch;
    double goal_width_l;
    double goal_width_r;
};


#endif
