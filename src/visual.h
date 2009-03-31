// -*-c++-*-

/***************************************************************************
                                   visual.h
                     Classes for building visual messages
                             -------------------
    begin                : 5-AUG-2002
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

#ifndef RCSS_VISUAL_H
#define RCSS_VISUAL_H

#include "sender.h"

namespace rcss {

/*!
//===================================================================
//
//  CLASS: VisualSendor
//
//  DESC: Base class for the visual protocol.
//
//===================================================================
*/

class VisualSender
    : protected Sender {
protected:
    VisualSender( std::ostream & transport )
        :  Sender( transport )
      { }

public:
    virtual
    ~VisualSender()
      { }

    virtual
    void sendVisual() = 0;
};


}

#endif
