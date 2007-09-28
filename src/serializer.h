// -*-c++-*-

/***************************************************************************
                                 serializer.h
                    Classes for serializing data to clients
                             -------------------
    begin                : 27-JAN-2002
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

#ifndef _SERIALIZER_H_
#define _SERIALIZER_H_

#include <string>
#include <iostream>
#include <map>
#include "types.h"
#include <rcssbase/lib/factory.hpp>

class PVector;

namespace rcss
{
namespace clang
{
class Msg;
}


class SerializerCommon
{
protected:
    SerializerCommon();

    virtual
    ~SerializerCommon();
public:
    typedef const SerializerCommon& (*Creator)();
    typedef rcss::lib::Factory< Creator, int > Factory;

    static
    Factory & factory();

    virtual
    void serializeServerParamBegin( std::ostream & ) const
      {}

    virtual
    void serializeServerParamEnd( std::ostream & ) const
      {}

    virtual
    void serializePlayerParamBegin( std::ostream & ) const
      {}

    virtual
    void serializePlayerParamEnd( std::ostream & ) const
      {}

    virtual
    void serializePlayerTypeBegin( std::ostream & ) const
      {}

    virtual
    void serializePlayerTypeEnd( std::ostream & ) const
      {}

    virtual
    void serializeParam( std::ostream &,
                         const int ) const
      {}

    virtual
    void serializeParam( std::ostream &,
                         const unsigned int ) const
      {}

    virtual
    void serializeParam( std::ostream &,
                         const bool ) const
      {}

    virtual
    void serializeParam( std::ostream &,
                         const double & ) const
      {}

    virtual
    void serializeParam( std::ostream &,
                         const std::string & ) const
      {}

    virtual
    void serializeParam( std::ostream &,
                         const std::string &,
                         const int ) const
      {}

    virtual
    void serializeParam( std::ostream &,
                         const std::string &,
                         const bool ) const
      {}

    virtual
    void serializeParam( std::ostream &,
                         const std::string &,
                         const double & ) const
      {}

    virtual
    void serializeParam( std::ostream &,
                         const std::string &,
                         const std::string & ) const
      {}
};


class Serializer
{
public:
    Serializer( const SerializerCommon& common )
        : m_common( common )
      {}


protected:
    const SerializerCommon&
    commonSerializer() const
      { return m_common; }

public:

    void serializeServerParamBegin( std::ostream & strm ) const
      {
          commonSerializer().serializeServerParamBegin( strm );
      }

    void serializeServerParamEnd( std::ostream & strm ) const
      {
          commonSerializer().serializeServerParamEnd( strm );
      }

    void serializePlayerParamBegin( std::ostream & strm ) const
      {
          commonSerializer().serializePlayerParamBegin( strm );
      }

    void serializePlayerParamEnd( std::ostream & strm ) const
      {
          commonSerializer().serializePlayerParamEnd( strm );
      }

    void serializePlayerTypeBegin( std::ostream & strm ) const
      {
          commonSerializer().serializePlayerTypeBegin( strm );
      }

    void serializePlayerTypeEnd( std::ostream & strm ) const
      {
          commonSerializer().serializePlayerTypeEnd( strm );
      }

    void serializeParam( std::ostream & strm,
                         const int value ) const
      {
          commonSerializer().serializeParam( strm, value );
      }

    void serializeParam( std::ostream & strm,
                         const unsigned int value ) const
      {
          commonSerializer().serializeParam( strm, value );
      }

    void serializeParam( std::ostream & strm,
                         const bool value ) const
      {
          commonSerializer().serializeParam( strm, value );
      }

    void serializeParam( std::ostream & strm,
                         const double & value ) const
      { commonSerializer().serializeParam( strm, value ); }

    void serializeParam( std::ostream & strm,
                         const std::string & value ) const
      {
          commonSerializer().serializeParam( strm, value );
      }

    void serializeParam( std::ostream & strm,
                         const std::string & name,
                         const int value ) const
      {
          commonSerializer().serializeParam( strm, name, value );
      }

    void serializeParam( std::ostream & strm,
                         const std::string & name,
                         const bool value ) const
      {
          commonSerializer().serializeParam( strm, name, value );
      }

    void serializeParam( std::ostream & strm,
                         const std::string & name,
                         const double & value ) const
      {
          commonSerializer().serializeParam( strm, name, value );
      }

    void serializeParam( std::ostream & strm,
                         const std::string & name,
                         const std::string & value ) const
      {
          commonSerializer().serializeParam( strm, name, value );
      }

private:
    const SerializerCommon& m_common;
};


class SerializerPlayer
    : public Serializer
{
public:
    typedef const SerializerPlayer* (*Creator)();
    typedef rcss::lib::Factory< Creator, int > Factory;

    static
    Factory&
    factory();

protected:
    SerializerPlayer( const SerializerCommon& common );

    virtual
    ~SerializerPlayer();
public:

    virtual
    void serializeRefAudio( std::ostream & strm,
                            const int& time,
                            const char* msg ) const = 0;
    virtual
    void serializeCoachAudio( std::ostream & strm,
                              const int& time,
                              const std::string & name,
                              const char* msg ) const = 0;
    virtual
    void serializeCoachStdAudio( std::ostream & strm,
                                 const int& time,
                                 const std::string & name,
                                 const clang::Msg& msg ) const = 0;

    virtual
    void serializeSelfAudio( std::ostream & strm,
                             const int& time,
                             const char* msg ) const = 0;

    virtual
    void serializePlayerAudio( std::ostream & strm,
                               const int& time,
                               const double& dir,
                               const char* msg ) const = 0;

    virtual
    void serializeAllyAudioFull( std::ostream &,
                                 const int,
                                 const double &,
                                 const int,
                                 const char* ) const
      {}

    virtual
    void serializeOppAudioFull( std::ostream &,
                                const int,
                                const double &,
                                const char* ) const
      {}

    virtual
    void serializeAllyAudioShort( std::ostream &,
                                  const int,
                                  const int ) const
      {}

    virtual
    void serializeOppAudioShort( std::ostream &,
                                 const int ) const
      {}


    virtual
    void serializeVisualBegin( std::ostream &,
                               const int ) const
      {}

    virtual
    void serializeVisualEnd( std::ostream & ) const
      {}

//     virtual
//     void serializeVisualObject( std::ostream &,
//                                 const std::string &,
//                                 const int ) const
//       { }

//     virtual
//     void serializeVisualObject( std::ostream &,
//                                 const std::string &,
//                                 const double &,
//                                 const int ) const
//       { }

//     virtual
//     void serializeVisualObject( std::ostream &,
//                                 const std::string &,
//                                 const double &,
//                                 const int,
//                                 const double &,
//                                 const double & ) const
//       { }

//     virtual
//     void serializeVisualObject( std::ostream &,
//                                 const std::string &,
//                                 const double &,
//                                 const int,
//                                 const double &,
//                                 const double &,
//                                 const double & ) const
//       { }

//     virtual
//     void serializeVisualObject( std::ostream &,
//                                 const std::string &,
//                                 const double &,
//                                 const int,
//                                 const double &,
//                                 const double &,
//                                 const double &,
//                                 const double & ) const
//       { }

//     virtual
//     void serializeVisualObject( std::ostream &,
//                                 const std::string &,
//                                 const double &,
//                                 const int,
//                                 const bool ) const
//       { }

//     virtual
//     void serializeVisualObject( std::ostream &,
//                                 const std::string &,
//                                 const double &,
//                                 const int,
//                                 const int,
//                                 const bool ) const
//       { }

//     virtual
//     void serializeVisualObject( std::ostream &,
//                                 const std::string &,
//                                 const double &,
//                                 const int,
//                                 const double &,
//                                 const double &,
//                                 const int,
//                                 const int,
//                                 const bool ) const
//       { }

//     virtual
//     void serializeVisualObject( std::ostream &,
//                                 const std::string &,
//                                 const double &,
//                                 const int,
//                                 const double &,
//                                 const double &,
//                                 const int,
//                                 const int,
//                                 const int,
//                                 const bool ) const
//       { }


    void serializeVisualObject( std::ostream & strm,
                                const std::string & name,
                                const int dir ) const
      {
          strm << " (" << name << " " << dir << ")";
      }

    void serializeVisualObject( std::ostream & strm,
                                const std::string & name,
                                const double & dist,
                                const int dir ) const
      {
          strm << " (" << name << " " << dist << " " << dir << ")";
      }

    void serializeVisualObject( std::ostream & strm,
                                const std::string & name,
                                const double & dist,
                                const int dir,
                                const double & dist_chg,
                                const double & dir_chg ) const
      {
          strm << " (" << name << " " << dist << " " << dir
               << " " << dist_chg << " " << dir_chg
               << ")";
      }

    void serializeVisualObject( std::ostream & strm,
                                const std::string & name,
                                const double & dist,
                                const int dir,
                                const double & dist_chg,
                                const double & dir_chg,
                                const double & body_dir ) const
      {
          strm << " (" << name << " " << dist << " " << dir
               << " " << dist_chg << " " << dir_chg
               << " " << body_dir
               << ")";
      }

    void serializeVisualObject( std::ostream & strm,
                                const std::string & name,
                                const double & dist,
                                const int dir,
                                const double & dist_chg,
                                const double & dir_chg,
                                const double & body_dir,
                                const double & head_dir ) const
      {
          strm << " (" << name << " " << dist << " " << dir
               << " " << dist_chg << " " << dir_chg
               << " " << body_dir << " " << head_dir
               << ")";
      }

    void serializeVisualObject( std::ostream & strm,
                                const std::string & name,
                                const double & dist,
                                const int dir,
                                const bool tackling ) const
      {
          strm << " (" << name << " " << dist << " " << dir;
          if( tackling )
              strm << " t";
          strm << ")";
      }

    void serializeVisualObject( std::ostream & strm,
                                const std::string & name,
                                const double & dist,
                                const int dir,
                                const int point_dir,
                                const bool tackling ) const
      {
          strm << " (" << name << " " << dist << " " << dir
               << " " << point_dir;
          if( tackling )
              strm << " t";
          strm << ")";
      }

    void serializeVisualObject( std::ostream & strm,
                                const std::string & name,
                                const double & dist,
                                const int dir,
                                const double & dist_chg,
                                const double & dir_chg,
                                const int body_dir,
                                const int head_dir,
                                const bool tackling ) const
      {
          strm << " (" << name << " " << dist << " " << dir
               << " " << dist_chg << " " << dir_chg
               << " " << body_dir << " " << head_dir;
          if( tackling )
              strm << " t";
          strm << ")";
      }

    void serializeVisualObject( std::ostream & strm,
                                const std::string & name,
                                const double & dist,
                                const int dir,
                                const double & dist_chg,
                                const double & dir_chg,
                                const int body_dir,
                                const int head_dir,
                                const int point_dir,
                                const bool tackling ) const
      {
          strm << " (" << name << " " << dist << " " << dir
               << " " << dist_chg << " " << dir_chg
               << " " << body_dir << " " << head_dir
               << " " << point_dir;
          if( tackling )
              strm << " t";
          strm << ")";
      }

    virtual
    void
    serializeBodyBegin( std::ostream &,
                        const int ) const
      {}

    virtual
    void
    serializeBodyEnd( std::ostream & ) const
      {}

    virtual
    void
    serializeBodyViewMode( std::ostream &,
                           const char*,
                           const char* ) const
      {}

    virtual
    void
    serializeBodyStamina( std::ostream &,
                          const double &,
                          const double & ) const
      {}

    virtual
    void
    serializeBodyVelocity( std::ostream &,
                           const double & ) const
      {}

    virtual
    void
    serializeBodyVelocity( std::ostream &,
                           const double &,
                           const int ) const
      {}

    virtual
    void
    serializeBodyCounts( std::ostream &,
                         const int,
                         const int,
                         const int,
                         const int ) const
      {}

    virtual
    void
    serializeBodyCounts( std::ostream &,
                         const int,
                         const int,
                         const int ) const
      {}

    virtual
    void
    serializeNeckAngle( std::ostream &,
                        const int ) const
      {}

    virtual
    void
    serializeNeckCount( std::ostream &,
                        const int ) const
      {}

    virtual
    void
    serializeArm( std::ostream &,
                  const int,
                  const int,
                  const double &,
                  const int,
                  const int ) const
      {}

    virtual
    void
    serializeFocus( std::ostream &,
                    const char*,
                    const int ) const
      {}

    virtual
    void
    serializeFocus( std::ostream &,
                    const char*,
                    const int,
                    const int ) const
      {}


    virtual
    void
    serializeTackle( std::ostream &,
                     const int,
                     const int ) const
      {}


    virtual
    void
    serializeFSBegin( std::ostream &,
                      const int ) const
      {}

    virtual
    void
    serializeFSEnd( std::ostream & ) const
      {}

    virtual
    void
    serializeFSPlayMode( std::ostream &,
                         const char* ) const
      {}

    virtual
    void
    serializeFSViewMode( std::ostream &,
                         const char*,
                         const char* ) const
      {}

    virtual
    void
    serializeFSCounts( std::ostream &,
                       const int,
                       const int,
                       const int,
                       const int,
                       const int,
                       const int,
                       const int,
                       const int ) const
      {}

    virtual
    void
    serializeFSScore( std::ostream &,
                      const int,
                      const int ) const
      {}

    virtual
    void
    serializeFSBall( std::ostream &,
                     const double &,
                     const double &,
                     const double &,
                     const double & ) const
      {}

    virtual
    void
    serializeFSPlayerBegin( std::ostream &,
                            const char,
                            const int,
                            const bool,
                            const int,
                            const double &,
                            const double &,
                            const double &,
                            const double &,
                            const double &,
                            const double & ) const
      {}

    virtual
    void
    serializeFSPlayerArm( std::ostream &,
                          const double &,
                          const double & ) const
      {}

    virtual
    void
    serializeFSPlayerEnd( std::ostream &,
                          const double &,
                          const double &,
                          const double & ) const
      {}

    virtual
    void
    serializeInit( std::ostream &,
                   const char*,
                   const int,
                   const PlayMode& ) const
      {}

    virtual
    void
    serializeReconnect( std::ostream &,
                        const char*,
                        const PlayMode& ) const
      {}

    virtual
    void
    serializeChangePlayer( std::ostream &,
                           const int ) const
      {}

    virtual
    void
    serializeChangePlayer( std::ostream &,
                           const int,
                           const int ) const
      {}

    virtual
    void
    serializeOKClang( std::ostream &,
                      const int,
                      const int ) const
      {}

    virtual
    void
    serializeErrorNoTeamName( std::ostream &,
                              const std::string & ) const
      {}

    virtual
    void
    serializeScore( std::ostream &,
                    const int,
                    const int,
                    const int ) const
      {}

};



class SerializerCoach
    : public Serializer
{
public:
    typedef const rcss::SerializerCoach* (*Creator)();
    typedef rcss::lib::Factory< Creator, int > Factory;

    static
    Factory&
    factory();

protected:
    SerializerCoach( const SerializerCommon& common );

    virtual
    ~SerializerCoach();
public:

    virtual
    void
    serializeRefAudio( std::ostream &,
                       const int&,
                       const char* ) const = 0;

    virtual
    void
    serializeCoachAudio( std::ostream &,
                         const int&,
                         const std::string &,
                         const char* ) const = 0;

    virtual
    void
    serializeCoachStdAudio( std::ostream &,
                            const int&,
                            const std::string &,
                            const clang::Msg& ) const = 0;

    virtual
    void
    serializePlayerAudio( std::ostream &,
                          const int&,
                          const std::string &,
                          const char* ) const = 0;


    virtual
    void
    serializeInit( std::ostream & strm ) const = 0;


    virtual
    void serializeVisualBegin( std::ostream &,
                               const int ) const
      { }

    virtual
    void serializeVisualEnd( std::ostream & ) const
      { }

    virtual
    void serializeLookBegin( std::ostream &,
                             const int ) const
      { }

    virtual
    void serializeLookEnd( std::ostream & ) const
      { }

    virtual
    void serializeVisualObject( std::ostream &,
                                const std::string &,
                                const PVector & ) const
      { }

    virtual
    void serializeVisualObject( std::ostream &,
                                const std::string &,
                                const PVector &,
                                const PVector & ) const
      { }

    virtual
    void serializeVisualObject( std::ostream &,
                                const std::string &,
                                const PVector &,
                                const PVector &,
                                const int,
                                const int ) const
      { }

    virtual
    void serializeVisualObject( std::ostream &,
                                const std::string &,
                                const PVector &,
                                const PVector &,
                                const int,
                                const int,
                                const bool ) const
      { }

    virtual
    void serializeVisualObject( std::ostream &,
                                const std::string &,
                                const PVector &,
                                const PVector &,
                                const int,
                                const int,
                                const int,
                                const bool ) const
      { }

    virtual
    void serializeOKEye( std::ostream &,
                         const bool ) const
      { }

};



class SerializerOnlineCoach
    : public Serializer
{
public:
    typedef const rcss::SerializerOnlineCoach* (*Creator)();
    typedef rcss::lib::Factory< Creator, int > Factory;

private:

    const SerializerCoach & M_coach;

public:

    static
    Factory&
    factory();

protected:

    SerializerOnlineCoach( const SerializerCommon & common,
                           const SerializerCoach & cosch);

    virtual
    ~SerializerOnlineCoach();


public:
    const
    SerializerCoach & coachSerializer() const
      {
          return M_coach;
      }

    virtual
    void
    serializeRefAudio( std::ostream &,
                       const int&,
                       const std::string &,
                       const char* ) const = 0;

    virtual
    void
    serializePlayerAudio( std::ostream &,
                          const int&,
                          const std::string &,
                          const char* ) const = 0;

    virtual
    void
    serializePlayerClangVer( std::ostream &,
                             const std::string &,
                             const unsigned int&,
                             const unsigned int& ) const = 0;

    virtual
    void
    serializeInit( std::ostream &,
                   const int side = 0 ) const = 0;

    virtual
    void
    serializeChangedPlayer( std::ostream &,
                            const int unum,
                            const int type = -1 ) const = 0;


    void serializeVisualBegin( std::ostream & strm,
                               const int time ) const
      {
          coachSerializer().serializeVisualBegin( strm, time );
      }

    void serializeVisualEnd( std::ostream & strm ) const
      {
          coachSerializer().serializeVisualEnd( strm );
      }

    void serializeLookBegin( std::ostream & strm,
                             const int time ) const
      {
          coachSerializer().serializeLookBegin( strm, time );
      }

    void serializeLookEnd( std::ostream & strm ) const
      {
          coachSerializer().serializeLookEnd( strm );
      }

    void serializeVisualObject( std::ostream & strm,
                                const std::string & name,
                                const PVector & pos ) const
      {
          coachSerializer().serializeVisualObject( strm, name, pos );
      }

    void serializeVisualObject( std::ostream & strm,
                                const std::string & name,
                                const PVector & pos,
                                const PVector & vel ) const
      {
          coachSerializer().serializeVisualObject( strm, name, pos, vel );
      }

    void serializeVisualObject( std::ostream & strm,
                                const std::string & name,
                                const PVector & pos,
                                const PVector & vel,
                                const int body,
                                const int neck ) const
      {
          coachSerializer().serializeVisualObject( strm,
                                                   name,
                                                   pos, vel,
                                                   body, neck );
      }

    void serializeVisualObject( std::ostream & strm,
                                const std::string & name,
                                const PVector & pos,
                                const PVector & vel,
                                const int body,
                                const int neck,
                                const bool tackling ) const
      {
          coachSerializer().serializeVisualObject( strm,
                                                   name,
                                                   pos, vel,
                                                   body, neck,
                                                   tackling );
      }

    void serializeVisualObject( std::ostream & strm,
                                const std::string & name,
                                const PVector & pos,
                                const PVector & vel,
                                const int body,
                                const int neck,
                                const int point_dir,
                                const bool tackling ) const
      {
          coachSerializer().serializeVisualObject( strm,
                                                   name,
                                                   pos, vel,
                                                   body, neck, point_dir,
                                                   tackling );
      }

};

}

#endif // _SERIALIZER_H_
