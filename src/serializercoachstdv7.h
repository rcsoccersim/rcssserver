// -*-c++-*-

/***************************************************************************
                            serializercoachstdv7.h
               Class for serializing data to std v7 offline coaches
                             -------------------
    begin                : 27-MAY-2002
    copyright            : (C) 2002 by The RoboCup Soccer Server
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

#ifndef SERIALIZERCOACHSTDV7_H
#define SERIALIZERCOACHSTDV7_H

#include "serializercoachstdv1.h"

namespace rcss {

class SerializerCoachStdv7
    : public SerializerCoachStdv1 {
protected:
    SerializerCoachStdv7( const SerializerCommon & common );

public:
    virtual
    ~SerializerCoachStdv7();

    static
    const
    SerializerCoachStdv7 * instance();

    virtual
    void serializeCoachAudio( std::ostream & strm,
                              const int time,
                              const std::string & name,
                              const char * msg ) const;

    virtual
    void serializeCoachStdAudio( std::ostream & strm,
                                 const int time,
                                 const std::string & name,
                                 const rcss::clang::Msg & msg ) const;

    virtual
    void serializePlayerAudio( std::ostream & strm,
                               const int time,
                               const std::string & name,
                               const char * msg ) const;

    virtual
    void serializeVisualBegin( std::ostream & strm,
                               const int time ) const;

    virtual
    void serializeVisualObject( std::ostream & strm,
                                const std::string & name,
                                const PVector & pos,
                                const PVector & vel,
                                const int body,
                                const int neck ) const;


    virtual
    void serializeVisualPlayer( std::ostream & strm,
                                const Player & player,
                                const std::string & name,
                                const PVector & pos,
                                const PVector & vel,
                                const int body,
                                const int neck ) const;

    virtual
    void serializeVisualPlayer( std::ostream & strm,
                                const Player & player,
                                const std::string & name,
                                const PVector & pos,
                                const PVector & vel,
                                const int body,
                                const int neck,
                                const int point_dir ) const;

};

}

#endif
