// -*-c++-*-

/***************************************************************************
                                sender.h
                           Base for sender classes
                             -------------------
    begin                : 2002-10-07
    copyright            : (C) 2002 by The RoboCup Soccer Simulator
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


#ifndef RCSSSENDER_H
#define RCSSSENDER_H

#include <iosfwd>


namespace rcss
{

/*!
//===================================================================
//
//  CLASS: Sender
//
//  DESC: Base sender.
//
//===================================================================
*/

class Sender
{
private:
    std::ostream& M_transport;

protected:
    std::ostream&
    transport()
      { return M_transport; }

public:
    Sender( std::ostream& transport )
        : M_transport( transport )
      {}
};
}

#endif
