/* -*-c++-*- */
/*
 *Header:
 *File: utility.h (for C++ & cc)
 *Author: Noda Itsuki
 *Date: 1995/02/24
 *EndHeader:
 */

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

#ifndef RCSS_UTILITY_H
#define RCSS_UTILITY_H

#include <cmath>
#include <cstdlib>
#include <string>
#include <algorithm> // min, max

/*
 *ModifyHistory
 *
 *EndModifyHistory
 */

#define EPS 1.0e-10
//#define RCSS_INFINITE 1.0e10
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

const double TWO_PI = 2 * M_PI;

//#define RAD2DEG	(180.0/M_PI)
//#define DEG2RAD (M_PI/180.0)
const double RAD2DEG = (180.0/M_PI);
const double DEG2RAD = (M_PI/180.0);

//#define Rad2Deg(a) 	((double)(a) * RAD2DEG)
inline
double
Rad2Deg( const double & a )
{
    return a * RAD2DEG;
}

//#define Rad2IDeg(a)	((int)(Rad2Deg(a)))
inline
int
Rad2IDeg( const double & a )
{
    return static_cast< int >( a * RAD2DEG );
}

//#define Rad2IDegRound(a)	((int)rint(Rad2Deg(a)))
inline
int
Rad2IDegRound( const double & a )
{
    return static_cast< int >( rint( Rad2Deg( a ) ) );
}

//#define Deg2Rad(a)	((double)(a) * DEG2RAD)
inline
double
Deg2Rad( const double & a )
{
    return a * DEG2RAD;
}

//#define Quantize(v,q)	((rint((v)/(q)))*(q))
inline
double
Quantize( const double & v,
          const double & q )
{
    return ( q < EPS
             ? v
             : rint( v / q ) * q );
}

inline
double
normalize_angle( double ang )
{
    if ( std::fabs( ang ) > TWO_PI )
    {
        ang = std::fmod( ang, TWO_PI );
    }

    if ( ang < -M_PI ) ang += TWO_PI;
    if ( ang > M_PI ) ang -= TWO_PI;

    return ang;
}

namespace rcss {

template < typename T >
const T &
bound( const T & low,
       const T & value,
       const T & high )
{
    return std::min( std::max( low, value ), high );
}

}

// #ifdef __cplusplus
// extern "C" {
// #endif

// extern int lcm(int a, int b);

// #ifdef __cplusplus
// }
// #endif


//#define nstonl(x) htonl((Int32)(((double)(Int16)ntohs(x) / SHOWINFO_SCALE) * SHOWINFO_SCALE2))
//#define nltons(x) htons((Int16)(((double)(Int32)ntohl(x) / SHOWINFO_SCALE2) * SHOWINFO_SCALE))


std::string
tildeExpand( const std::string & path_name );

#endif
