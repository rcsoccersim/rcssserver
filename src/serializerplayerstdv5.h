// -*-c++-*-

/***************************************************************************
                            serializerplayerstdv5.h
                  Class for serializing data to std v5 players
                             -------------------
    begin                : 2023-01-27
    copyright            : (C) 2023 by The RoboCup Soccer Server
                           Maintenance Group.
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LGPL as published by the Free Software  *
 *   Foundation; either version 3 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/


#ifndef SERIALIZERPLAYERSTDV5_H
#define SERIALIZERPLAYERSTDV5_H

#include "serializerplayerstdv1.h"

namespace rcss {

class SerializerPlayerStdv5
    : public SerializerPlayerStdv1 {
protected:
    explicit
    SerializerPlayerStdv5( const SerializerCommon::Ptr common );

public:
    virtual
    ~SerializerPlayerStdv5() override;

    static
    const
    Ptr create();

    virtual
    void serializeBodyCounts( std::ostream &,
                              const Player & ) const override;

};

}

#endif
