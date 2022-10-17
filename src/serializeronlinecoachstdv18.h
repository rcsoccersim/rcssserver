// -*-c++-*-

/***************************************************************************
                         serializeronlinecoachstdv18.h
               Class for serializing data to std v18 online coaches
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

#ifndef SERIALIZERONLINECOACHSTDV18_H
#define SERIALIZERONLINECOACHSTDV18_H

#include "serializeronlinecoachstdv14.h"

namespace rcss {

class SerializerOnlineCoachStdv18
        : public SerializerOnlineCoachStdv14 {
protected:
    SerializerOnlineCoachStdv18( const SerializerCommon::Ptr common,
                                 const SerializerCoach::Ptr coach );

public:
    virtual
    ~SerializerOnlineCoachStdv18() override;

    static
    const Ptr create();

};
}

#endif
