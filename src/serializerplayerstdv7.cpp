// -*-c++-*-

/***************************************************************************
                            serializerplayerstdv7.cc
                  Class for serializing data to std v7 players
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "serializerplayerstdv7.h"

#include "clangmsg.h"

namespace rcss {

SerializerPlayerStdv7::SerializerPlayerStdv7( const SerializerCommon::Ptr common )
    : SerializerPlayerStdv1( common )
{

}

SerializerPlayerStdv7::~SerializerPlayerStdv7()
{

}

void
SerializerPlayerStdv7::serializeCoachAudio( std::ostream & strm,
                                            const int time,
                                            const std::string & name,
                                            const char * msg ) const
{
    strm << "(hear " << time << ' '
         << name << " \"" << msg << "\")";
}

void
SerializerPlayerStdv7::serializeCoachStdAudio( std::ostream & strm,
                                               const int time,
                                               const std::string & name,
                                               const rcss::clang::Msg & msg ) const
{
    strm << "(hear " << time << ' '
         << name << ' ' << msg << ')';
}

void
SerializerPlayerStdv7::serializeSelfAudio( std::ostream & strm,
                                           const int time,
                                           const char * msg ) const
{
    strm << "(hear " << time << " self \""
         << msg << "\")";
}

void
SerializerPlayerStdv7::serializePlayerAudio( std::ostream & strm,
                                             const int time,
                                             const double & dir,
                                             const char * msg ) const
{
    strm << "(hear " << time << ' '
         << dir << " \"" << msg << "\")";
}

void
SerializerPlayerStdv7::serializeServerParamBegin( std::ostream & strm ) const
{
    strm << "(server_param";
}

void
SerializerPlayerStdv7::serializeServerParamEnd( std::ostream & strm ) const
{
    strm << ')';
}

void
SerializerPlayerStdv7::serializePlayerParamBegin( std::ostream& strm ) const
{
    strm << "(player_param";
}

void
SerializerPlayerStdv7::serializePlayerParamEnd( std::ostream & strm ) const
{
    strm << ")";
}

void
SerializerPlayerStdv7::serializePlayerTypeBegin( std::ostream & strm ) const
{
    strm << "(player_type";
}

void
SerializerPlayerStdv7::serializePlayerTypeEnd( std::ostream & strm ) const
{
    strm << ')';
}

void
SerializerPlayerStdv7::serializeParam( std::ostream & strm,
                                       const int param ) const
{
    strm << ' ' << param;
}

void
SerializerPlayerStdv7::serializeParam( std::ostream & strm,
                                       const unsigned int param ) const
{
    strm << ' ' << param;
}

void
SerializerPlayerStdv7::serializeParam( std::ostream & strm,
                                       const bool param ) const
{
    strm << ' ' << param;
}

void
SerializerPlayerStdv7::serializeParam( std::ostream & strm,
                                       const double & param ) const
{
    strm << ' ' << param;
}

void
SerializerPlayerStdv7::serializeParam( std::ostream & strm,
                                       const std::string & param ) const
{
    strm << ' ' << param;
}

void
SerializerPlayerStdv7::serializeChangePlayer( std::ostream & strm,
                                              const int unum ) const
{
    strm << "(change_player_type "
         << unum << ')';
}

void
SerializerPlayerStdv7::serializeChangePlayer( std::ostream & strm,
                                              const int unum,
                                              const int type ) const
{
    strm << "(change_player_type "
         << unum << ' ' << type << ')';
}

const
SerializerPlayer::Ptr
SerializerPlayerStdv7::create()
{
    SerializerCommon::Creator cre;
    if ( ! SerializerCommon::factory().getCreator( cre, 7 ) )
    {
        return SerializerPlayer::Ptr();
    }

    SerializerPlayer::Ptr ptr( new SerializerPlayerStdv7( cre() ) );
    return ptr;
}


namespace {
RegHolder v7 = SerializerPlayer::factory().autoReg( &SerializerPlayerStdv7::create, 7 );
}

}
