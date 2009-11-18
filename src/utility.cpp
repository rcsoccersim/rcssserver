// -*-c++-*-

/*
 *Header:
 *File: utility.C
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "utility.h"

#include <iostream>
#include <cstdlib>
#include <ctime>

#include <sys/types.h>
#ifdef HAVE_PWD_H
#include <pwd.h>
#endif

//#include "random.h"
/*
 *===================================================================
 *Part:      Random number utility
 *===================================================================
 */
//unsigned int randomize(void)
//{
//	static boost::uniform_int< unsigned int > rand_int( 0, UINT_MAX );
//	return rand_int( rcss::random::DefaultRNG::instance() );
//}


/*
 *===================================================================
 *Part:      Angle
 *===================================================================
 */
/* double normalize_angle(double ang) */
/* { */
/*     while (1) { */
/*       if ( ang < -M_PI ) */
/*           ang += 2 * M_PI; */
/*       else if ( ang > M_PI ) */
/*           ang -= 2 * M_PI; */
/*       else */
/*           break; */
/*   } */
/*   return ang; */
/* } */


/*
 *==================================================================
 *Part:     Lowest Common Multiple
 *==================================================================
 */
// int
// lcm( int a,
//      int b )
// {
//     int tmp = 0, idx = 0, larger = std::max( a, b );
//     do
//     {
//         ++idx;
//         tmp = larger * idx;
//     }
//     while ( tmp % a != 0 || tmp % b != 0 );
//     return tmp;
// }

/*
 *==================================================================
 *Part:     Tilde Expand
 *==================================================================
 */
// This will expand ~ and ~user at the beginning of the path specified
// in path_name.  No attempt is made to check or or canonize the path.
//
// Throws when an OS API gives us something unusable like a NULL pointer.

#if defined(_WIN32) || defined(__WIN32__) || defined (WIN32) || defined(__CYGWIN__)
#  ifndef __CYGWIN__
#    define RCSS_WIN
#  endif
#  ifndef _WIN32_IE
#    define _WIN32_IE 0x0400
#  endif
#  ifndef _WIN32_WINDOWS
#    define _WIN32_WINDOWS 0x0490
#  endif
//#  include <windows.h>
#  include <shlobj.h>
//#  include <stdio.h>
#endif

std::string
tildeExpand( const std::string & path_name )
{
#ifdef __CYGWIN__

    std::string new_path = path_name;
    std::replace( new_path.begin(), new_path.end(), '\\', '/' );

    // There must be a ~ at the start of the path for a valid
    // expansioin.
    if ( new_path.length() == 0
         || new_path[0] != '~' )
    {
        return new_path;
    }

    if ( new_path.length() == 1
         || new_path[1] == '/' )
    {
        char szpath[MAX_PATH];
        if ( SHGetSpecialFolderPath( NULL, szpath, CSIDL_PERSONAL, TRUE ) )
        {
            std::string userdir = szpath;
            std::replace( userdir.begin(), userdir.end(), '\\', '/' );
            return userdir + new_path.substr( 1 );
        }
    }

    return new_path;

#elif defined(RCSS_WIN)

    // There must be a ~ at the start of the path for a valid
    // expansioin.
    if ( path_name.length() == 0
         || path_name[0] != '~' )
    {
        return path_name;
    }

    if ( path_name.length() == 1
         || path_name[1] == '\\' )
    {
        char szpath[MAX_PATH];
        if ( SHGetSpecialFolderPath( NULL, szpath, CSIDL_PERSONAL, TRUE ) )
        {
            return szpath + path_name.substr( 1 );
        }
    }

    return path_name;

#else

    // There must be a ~ at the start of the path for a valid
    // expansioin.
    if ( path_name.length() == 0
         || path_name[0] != '~' )
    {
        return path_name;
    }

    std::string new_path;  // Used to store the new ~ expanded path.
    std::string username;  // Used to store user name of interest.

    if ( path_name.length() == 1
         || path_name[1] == '/' )
    {
        // Get the current user.
        const char * err = std::getenv( "USER" );
        if ( err == NULL )
        {
            // On Windows USERNAME is used instead
            err = std::getenv( "USERNAME" );
            if ( err == NULL )
            {
                return path_name;
            }
        }

        username = err;
        // if succeeded, remove the tilde
        new_path = path_name.substr( 1 );
    }
    else
    {
        // Fish out the user name from path_name and remove it
        // from new_path.
        std::string::size_type user_end = path_name.find( '/' );
        if ( user_end == std::string::npos )
        {
            // No / so whole path must be the username.
            user_end = path_name.length();
        }
        username = path_name.substr( 1, user_end - 1 );
        new_path = path_name.substr( user_end );
    }

    // Get the passwd file entry for the user and place their home
    // directory path at the start of newPath.
    struct passwd * pwd_entry = getpwnam( username.c_str() );
    if ( pwd_entry == NULL )
    {
        return path_name;
    }

    new_path.insert( 0, pwd_entry->pw_dir );

    return new_path;

#endif
}
