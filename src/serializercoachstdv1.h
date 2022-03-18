// -*-c++-*-

/***************************************************************************
                            serializercoachstdv1.h
               Class for serializing data to std v1 offline coaches
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
 *   Foundation; either version 3 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#ifndef SERIALIZERCOACHSTDV1_H
#define SERIALIZERCOACHSTDV1_H

#include "serializer.h"

namespace rcss {

class SerializerCoachStdv1
    : public SerializerCoach {
protected:
    explicit
    SerializerCoachStdv1( const SerializerCommon::Ptr common );

public:
    virtual
    ~SerializerCoachStdv1() override;

    static
    const
    Ptr create();

    virtual
    void serializeRefereeAudio( std::ostream & strm,
                                const int time,
                                const char * msg ) const override;

    virtual
    void serializeCoachAudio( std::ostream & strm,
                              const int time,
                              const std::string & name,
                              const char * msg ) const override;

    virtual
    void serializeCoachStdAudio( std::ostream & strm,
                                 const int time,
                                 const std::string & name,
                                 const rcss::clang::Msg & msg ) const override;

    virtual
    void serializePlayerAudio( std::ostream & strm,
                               const int time,
                               const std::string& name,
                               const char* msg ) const override;

    virtual
    void serializeInit( std::ostream & ) const override;

    virtual
    void serializeVisualBegin( std::ostream & strm,
                               const int time ) const override;

    virtual
    void serializeVisualEnd( std::ostream & strm ) const override;

    virtual
    void serializeLookBegin( std::ostream & strm,
                             const int time ) const override;

    virtual
    void serializeLookEnd( std::ostream & strm ) const override;

    virtual
    void serializeVisualObject( std::ostream & strm ,
                                const std::string & name,
                                const PVector & pos ) const override;

    virtual
    void serializeVisualObject( std::ostream & strm,
                                const std::string & name,
                                const PVector & pos,
                                const PVector & vel ) const override;

    virtual
    void serializeVisualObject( std::ostream & strm,
                                const std::string & name,
                                const PVector & pos,
                                const PVector & vel,
                                const int body,
                                const int neck ) const override;

    virtual
    void serializeVisualPlayer( std::ostream & strm,
                                const Player & player,
                                const std::string & name,
                                const PVector & pos,
                                const PVector & vel,
                                const int body,
                                const int neck ) const override;

    virtual
    void serializeVisualPlayer( std::ostream & strm,
                                const Player & player,
                                const std::string & name,
                                const PVector & pos,
                                const PVector & vel,
                                const int body,
                                const int neck,
                                const int point_dir ) const override;


    virtual
    void serializeOKEye( std::ostream & strm,
                         const bool on ) const override;
};

}

#endif
