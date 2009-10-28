// -*-c++-*-

/***************************************************************************
                         serializeronlinecoachstdv14.h
               Class for serializing data to std v14 online coaches
                             -------------------
    begin                : 2009-10-26
    copyright            : (C) 2009 by The RoboCup Soccer Server
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

#ifndef SERIALIZERONLINECOACHSTDV14_H
#define SERIALIZERONLINECOACHSTDV14_H

#include "serializeronlinecoachstdv13.h"

namespace rcss {

class SerializerOnlineCoachStdv14
    : public SerializerOnlineCoachStdv13 {
protected:
    SerializerOnlineCoachStdv14( const SerializerCommon::Ptr common,
                                 const SerializerCoach::Ptr coach );

public:
    virtual
    ~SerializerOnlineCoachStdv14();

    static
    const
    SerializerOnlineCoach::Ptr create();

};

}

#endif
