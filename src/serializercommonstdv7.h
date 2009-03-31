// -*-c++-*-

/***************************************************************************
                            serializercommonstdv7.h
                  Class for serializing data to std v7 clients
                             -------------------
    begin                : 24-JAN-2003
    copyright            : (C) 2003 by The RoboCup Soccer Server
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

#ifndef SERIALIZERCOMMONSTDV7_H
#define SERIALIZERCOMMONSTDV7_H

#include "serializercommonstdv1.h"

namespace rcss {


class SerializerCommonStdv7
    : public SerializerCommonStdv1 {
protected:
    SerializerCommonStdv7();

public:
    virtual
    ~SerializerCommonStdv7();

    static
    const
    Ptr create();

    virtual
    void serializeServerParamBegin( std::ostream & strm ) const;

    virtual
    void serializeServerParamEnd( std::ostream & strm ) const;

    virtual
    void serializePlayerParamBegin( std::ostream & strm ) const;

    virtual
    void serializePlayerParamEnd( std::ostream & strm ) const;

    virtual
    void serializePlayerTypeBegin( std::ostream & strm ) const;

    virtual
    void serializePlayerTypeEnd( std::ostream & strm ) const;

    virtual
    void serializeParam( std::ostream & strm,
                         const int param ) const;

    virtual
    void serializeParam( std::ostream & strm,
                         const unsigned int param ) const;

    virtual
    void serializeParam( std::ostream & strm,
                         const bool param ) const;

    virtual
    void serializeParam( std::ostream & strm,
                         const double & param ) const;

    virtual
    void serializeParam( std::ostream & strm,
                         const std::string & param ) const;
};

}

#endif
