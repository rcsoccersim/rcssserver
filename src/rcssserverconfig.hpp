/***************************************************************************
               rcssbaseconfig.hpp  -  Defines RCSSLIB_API
                             -------------------
    begin                : 2005-07-08
    copyright            : (C) 2005 by The RoboCup Soccer Server 
                           Maintenance Group.
    email                : sserver-admin@lists.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LGPL as published by the Free Software  *
 *   Foundation; either version 2 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#ifndef RCSSSERVER_CONFIG_HPP
#define RCSSSERVER_CONFIG_HPP

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(CYGWIN)
  #ifdef RCSSSERVER_EXPORTS
    #define RCSSSERVER_API __declspec(dllexport)
    #define RCSSSERVER_EXTERN
  #else
    #define RCSSSERVER_API __declspec(dllimport)
    #define RCSSSERVER_EXTERN extern
  #endif
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
  #define RCSSSERVER_API
  #define RCSSSERVER_EXTERN extern
#else
  #define RCSSSERVER_API
  #define RCSSSERVER_EXTERN
#endif

#endif
