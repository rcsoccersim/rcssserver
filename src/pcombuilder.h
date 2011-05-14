// -*-c++-*-

/***************************************************************************
                               pcombuilder.h
                Interface for building a player command message
                             -------------------
    begin                : 16-FEB-2002
    copyright            : (C) 2002 by The RoboCup Soccer Server
                           Maintenance Group.
    email                : sserver-admin@lists.sourceforge.net
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LGPL as published by the Free Software  *
 *   Foundation; either version 3 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#ifndef RCSSSERVER_PCOMBUILDER_H
#define RCSSSERVER_PCOMBUILDER_H

#include <string>
#include <iosfwd>
#include <exception>

// The pcom::Builder is called from within the parser to construct the
// player command message.  It is not the job of the builder to make sure the
// grammar meets to player command grammar rules, that's the job of the parser,
// however the Builder should do some error detection to make sure
// that if for some reason the methods are called out of context, the
// builder doesn't cause the whole program to crash.
// By using a builder, complexity is removed from the parser itself,
// making it easier to focus on the grammar without distraction.  This
// also allows the behaviour of the parser to be easily modified or
// extended by simple modifying or extending the builder.


namespace rcss {
namespace pcom {

enum VIEW_WIDTH {
    NARROW,
    NORMAL,
    WIDE
};

enum VIEW_QUALITY {
    LOW,
    HIGH
};

enum TEAM {
    LEFT_SIDE,
    RIGHT_SIDE,
    OUR,
    OPP,
    UNKNOWN_TEAM
};

enum EAR_MODE {
    PARTIAL,
    COMPLETE,
    UNKNOWN_EAR_MODE
};


class Builder {
public:
    Builder()
      { }

    virtual
    ~Builder()
      { }

    virtual void dash( double power ) = 0;
    virtual void dash( double power, double dir ) = 0;
    virtual void turn( double moment ) = 0;
    virtual void turn_neck( double moment ) = 0;
    virtual void kick( double power, double dir ) = 0;
    virtual void long_kick( double power, double dir ) = 0;
    virtual void goalieCatch( double dir ) = 0;
    virtual void say( std::string message ) = 0;
    virtual void sense_body() = 0;
    virtual void score() = 0;
    virtual void move( double x, double y ) = 0;
    virtual void change_view( VIEW_WIDTH viewWidth, VIEW_QUALITY viewQuality ) = 0;
    virtual void change_view( VIEW_WIDTH viewWidth ) = 0;
    virtual void compression( int level ) = 0;
    virtual void bye() = 0;
    virtual void done() = 0;
    virtual void pointto( bool on, double dist, double head ) = 0;
    virtual void attentionto( bool on, TEAM team_side, std::string team_name, int at_unum ) = 0;
    virtual void tackle( double power_or_dir ) = 0;
    virtual void tackle( double power_or_dir, bool foul ) = 0;
    virtual void clang( int min, int max) = 0;
    virtual void ear( bool on, TEAM team_side, std::string team_name, EAR_MODE mode ) = 0;
    virtual void synch_see() = 0;
};


class BuilderErr
    : public std::exception {
protected:
    std::string M_msg;
public:
    BuilderErr( const char * file,
                const int & line,
                const char * msg ) throw();

    ~BuilderErr() throw()
      { }

    const char * what() const throw()
      {
          return M_msg.c_str();
      }
};

}
}

#endif
