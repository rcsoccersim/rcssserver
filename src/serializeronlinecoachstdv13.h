// -*-c++-*-

/***************************************************************************
                         serializeronlinecoachstdv13.h
               Class for serializing data to std v13 online coaches
                             -------------------
    begin                : 2008-11-03
    copyright            : (C) 2008 by The RoboCup Soccer Server
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

#ifndef SERIALIZERONLINECOACHSTDV13_H
#define SERIALIZERONLINECOACHSTDV13_H

#include "serializeronlinecoachstdv8.h"

namespace rcss {

class SerializerOnlineCoachStdv13
    : public SerializerOnlineCoachStdv8 {
protected:
    SerializerOnlineCoachStdv13( const SerializerCommon::Ptr common,
                                 const SerializerCoach::Ptr coach );

public:
    virtual
    ~SerializerOnlineCoachStdv13();

    static
    const
    SerializerOnlineCoach::Ptr create();

};

}

#endif
