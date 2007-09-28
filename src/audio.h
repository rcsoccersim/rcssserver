// -*-c++-*-

/***************************************************************************
                                   audio.h
                     Classes for building audio messages
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

#ifndef _AUDIO_H_
#define _AUDIO_H_

#include "sender.h"
#include "observer.h"
#include "param.h"
#include "types.h"
#include <string>
#include <map>
#include <list>
#include <rcssbase/lib/factory.hpp>


class Coach;
class OnlineCoach;
class Player;
class Stadium;
class RemoteClient;

namespace rcss
{
namespace clang
{
class Msg;
}

// Each Listener has an AudioSender for converting the message
// from machine format into a format the client can understand.  The
// AudioSender is in charge of gathering the relevant data
// required for the message (such as the timestamp, name of the
// sender, etc) including the addition of any noise (e.g. direction of
// sender rounded to int).  It then delegates the task of serializing
// this data to a generic serializer, which knows how to convert the
// supplied data into client format.
//
// The reason it is done this way is because it allows us to seperatly
// modify the data or the format.  If you which to change the data
// being sent to the client, (e.g. round to int rather than cast to
// int) then you change the AudioSender.  If you wish to change
// the format (such as quoting say messages), you need to change the
// generic Serializer that AudioSender delegates to.

class AudioSender
    : protected Sender
{
public:
    typedef std::auto_ptr< rcss::AudioSender > Ptr;

protected:
    const Stadium& M_stadium;

public:
    AudioSender( const Stadium& stadium,
                 std::ostream& transport )
        : Sender( transport ), M_stadium( stadium )
      {}

    virtual
    ~AudioSender() {}

    virtual
    void
    sendRefAudio( const char* ) {}

    virtual
    void
    sendCoachAudio( const Coach&, const char* ) {}

    virtual
    void
    sendCoachStdAudio( const clang::Msg& ) {}

    virtual
    void
    sendPlayerAudio( const Player&, const char* ) {}

    virtual
    void
    newCycle() {}

    virtual
    void
    focusOn( const Player& ) {}

    virtual
    void
    focusOff() {}

    virtual
    const Player*
    getFocusTarget() const
      { return NULL; }

    virtual
    unsigned int
    getFocusCount() const
      { return 0; }

    virtual
    void
    setEar( bool, Side, bool, bool )
      {}

    virtual
    void
    sendOKClang()
      {}

    virtual
    void
    sendErrorNoTeamName( const std::string& )
      {}
};




class Listener
    : protected BaseObserver< AudioSender >
{
public:
    Listener()
      {}

    Listener( AudioSender& sender )
        : BaseObserver< AudioSender >( sender )
      {}

    Listener( std::auto_ptr< AudioSender > sender )
        : BaseObserver< AudioSender >( sender )
      {}

    ~Listener()
      {}

    void
    setAudioSender( AudioSender& sender )
      {
          BaseObserver< AudioSender >::setSender( sender );
      }

    void
    setAudioSender( std::auto_ptr< AudioSender > sender )
      {
          BaseObserver< AudioSender >::setSender( sender );
      }

    void
    sendRefAudio( const char* msg );

    void
    sendCoachAudio( const Coach& coach, const char* msg );

    void
    sendCoachStdAudio( const clang::Msg& msg );

    void
    sendPlayerAudio( const Player& player, const char* msg );

    void
    newCycle();

    void
    focusOn( const Player& player );

    void
    focusOff();

    const Player*
    getFocusTarget() const
      { return sender().getFocusTarget(); }

    unsigned int
    getFocusCount() const
      { return sender().getFocusCount(); }

    class NewCycle
    {
    public:
        void
        operator()( Listener* listener )
          { listener->newCycle(); }
    };

    void
    setEar( bool on, Side side, bool complete, bool partial );

    void
    sendOKClang();

    void
    sendErrorNoTeamName( const std::string& team_name );
};


class SerializerPlayer;


class AudioSenderPlayer
    : public AudioSender
{
protected:
    Player& M_listener;
    const SerializerPlayer& M_serializer;

public:
    class Params
    {
    public:
        Params( const Stadium& stadium,
                std::ostream& transport,
                Player& listener,
                const SerializerPlayer& serializer )
            : m_stadium( stadium ),
              m_transport( transport ),
              m_listener( listener ),
              m_serializer( serializer )
          {}

        const Stadium& m_stadium;
        std::ostream& m_transport;
        Player& m_listener;
        const SerializerPlayer& m_serializer;
    };

    typedef Ptr(*Creator)( const Params& );
    typedef rcss::lib::Factory< Creator, int > Factory;

    static
    Factory&
    factory();


    AudioSenderPlayer( const Params& params )
        : AudioSender( params.m_stadium, params.m_transport ),
          M_listener( params.m_listener ),
          M_serializer( params.m_serializer )
      {}

    ~AudioSenderPlayer() {}

    virtual
    void
    sendPlayerAudio( const Player& player, const char* msg );

    virtual
    void
    sendSelfAudio( const char* ) {}

    virtual
    void
    sendNonSelfPlayerAudio( const Player&, const char* ) {}

    virtual
    void
    sendOKClang()
      {}

    virtual
    void
    sendErrorNoTeamName( const std::string& )
      {}

protected:

    // These predicate methods allow the send funtions to change the
    // data contstruction without having to rewrite the predicate.  It
    // also allows you to change the predicate without changing the data
    // construction.
    virtual
    bool
    generalPredicate() const;

    virtual
    bool
    coachStdPredicate( const clang::Msg& msg ) const;

    virtual
    bool
    nonSelfPlayerPredicate( const Player& player ) const;

    // The post method(s) allow any adjustmets to the listeners state to
    // be done, independant of the data contruction
    virtual
    bool
    postNonSelfPlayer( const Player& player );

};




class AudioSenderPlayerv1
    : public AudioSenderPlayer
{
public:
    AudioSenderPlayerv1( const Params& params )
        : AudioSenderPlayer( params )
      {}

    virtual
    ~AudioSenderPlayerv1()
      {}

    virtual
    void
    sendRefAudio( const char* msg );

    virtual
    void
    sendCoachAudio( const Coach& coach, const char* msg );

    virtual
    void
    sendCoachStdAudio( const clang::Msg& msg );

    virtual
    void
    sendSelfAudio( const char* msg );

    virtual
    void
    sendNonSelfPlayerAudio( const Player& player, const char* msg );

    virtual
    void
    sendOKClang();

    virtual
    void
    sendErrorNoTeamName( const std::string& team_name );

};




class AudioSenderPlayerv7
    : public AudioSenderPlayerv1
{
public:
    AudioSenderPlayerv7( const Params& params )
        : AudioSenderPlayerv1( params )
      {}

    virtual
    ~AudioSenderPlayerv7()
      {}

    virtual
    void
    sendCoachAudio( const Coach& coach, const char* msg );

    virtual
    void
    sendNonSelfPlayerAudio( const Player& player, const char* msg );
};


class AudioSenderPlayerv8
    : public AudioSenderPlayerv7
{
protected:
    // the containers and pairs below use char* instead of const
    // char* as the strings belong to the containers and pairs and
    // will be deleted after they are removed. By making the
    // non-const, a string needs to be copied to be inserted. I
    // know it's a bit of a dodgy safty check, but it's better
    // than nothing.  I'll try to make it cleaner later.  Idealy
    // we should store them once in a general store and then point
    // to that store.  Then we could just delete from that store
    // when we are done... More work for later.

    typedef const Player*                         player_key_t;
    typedef std::pair< player_key_t, char* >   player_key_value_t;
    typedef const Coach*                          coach_key_t;
    typedef std::pair< coach_key_t, char* >    coach_key_value_t;

    typedef std::multimap< player_key_t, char* >  player_msg_cont_t;
    typedef std::list< char* >                    self_msg_cont_t;
    typedef std::list< coach_key_value_t >               coach_msg_cont_t;

    class State
    {
    public:
        typedef AudioSenderPlayerv8::player_key_t               key_t;
        typedef AudioSenderPlayerv8::player_msg_cont_t          msg_cont_t;
        typedef AudioSenderPlayerv8::player_key_value_t         key_value_t;

        State() {}

        virtual
        ~State() {}

        virtual
        key_value_t
        getMsg( msg_cont_t& msgs ) = 0;

        virtual
        const Player*
        getFocusTarget() const
          { return NULL; }
    };

    class Unfocused
        : public State
    {
    public:
        Unfocused() {}

        virtual
        ~Unfocused() {}

        virtual
        key_value_t
        getMsg( msg_cont_t& msgs );
    };

    class Focused
        : public Unfocused
    {
    protected:
        player_key_t M_key;
    public:
        Focused()
            : M_key( NULL )
          {}

        virtual
        ~Focused() {}

        virtual
        void
        focusOn( const player_key_t& key )
          {
              M_key = key;
          }

        virtual
        key_value_t
        getMsg( msg_cont_t& msgs );

        virtual
        player_key_t
        getFocusTarget() const
          { return M_key; }
    };

    // since we might be switching back and forth between being focused
    // or unfocused, both states are created on the construction of
    // PlayerEar rather than creating them on demand and destroying them
    // when we are finished with them.
    Unfocused M_unfocused;
    Focused M_focused;

    State* M_state_p;
    unsigned int M_focus_count;

    player_msg_cont_t M_player_msgs;
    self_msg_cont_t   M_self_msgs;
    coach_msg_cont_t  M_coach_msgs;

    bool M_left_partial;
    bool M_left_complete;
    bool M_right_partial;
    bool M_right_complete;

public:
    AudioSenderPlayerv8( const Params& params )
        : AudioSenderPlayerv7( params ),
          M_state_p( &M_unfocused ),
          M_focus_count ( 0 ),
          M_left_partial( false ),
          M_left_complete( true ),
          M_right_partial( false ),
          M_right_complete( true )
      {}

    virtual
    ~AudioSenderPlayerv8()
      {
          for( player_msg_cont_t::iterator i = M_player_msgs.begin();
               i != M_player_msgs.end(); i++ )
          {
              free( i->second );
              i->second = NULL;
          }
          M_player_msgs.clear();

          for( self_msg_cont_t::iterator i = M_self_msgs.begin();
               i != M_self_msgs.end(); i++ )
          {
              free( *i );
              *i = NULL;
          }
          M_self_msgs.clear();

          for( coach_msg_cont_t::iterator i = M_coach_msgs.begin();
               i != M_coach_msgs.end(); i++ )
          {
              free( i->second );
              i->second = NULL;
          }
          M_coach_msgs.clear();
      }

    virtual
    void
    sendCoachAudio( const Coach& coach, const char* msg );

    virtual
    void
    sendSelfAudio( const char* msg );

    virtual
    void
    sendNonSelfPlayerAudio( const Player& player, const char* msg );

    virtual
    void
    newCycle();

    virtual
    void
    focusOn( const Player& player );

    virtual
    void
    focusOff();

    virtual
    const Player*
    getFocusTarget() const
      {
          return M_state_p->getFocusTarget();
      }

    virtual
    unsigned int
    getFocusCount() const
      { return M_focus_count; }

    virtual
    void
    setEar( bool on, Side side, bool complete, bool partial );

protected:
    virtual
    void
    sendCachedCoachAudio( const Coach& coach, const char* msg );

    virtual
    void
    sendCachedSelfAudio( const char* msg );

    virtual
    void
    sendCachedNonSelfPlayerAudio( const Player& player, const char* msg );

    virtual
    bool
    nonSelfPlayerPredicate( const Player& player ) const;

    virtual
    bool
    nonSelfPlayerFullPredicate( const Player& player ) const;

};


class SerializerCoach;


class AudioSenderCoach
    : public AudioSender
{
protected:
    Coach& M_listener;
    const SerializerCoach& M_serializer;
public:
    class Params
    {
    public:
        Params( const Stadium& stadium,
                std::ostream& transport,
                Coach& listener,
                const SerializerCoach& serializer )
            : m_stadium( stadium ),
              m_transport( transport ),
              m_listener( listener ),
              m_serializer( serializer )
          {}

        const Stadium& m_stadium;
        std::ostream& m_transport;
        Coach& m_listener;
        const SerializerCoach& m_serializer;
    };

public:
    typedef Ptr(*Creator)( const Params& );
    typedef rcss::lib::Factory< Creator, int > Factory;

    static
    Factory&
    factory();


    AudioSenderCoach( const Params& params )
        : AudioSender( params.m_stadium, params.m_transport ),
          M_listener( params.m_listener ),
          M_serializer( params.m_serializer )
      {}

    ~AudioSenderCoach() {}

protected:
    virtual
    bool
    generalPredicate() const;
};




class AudioSenderCoachv1
    : public AudioSenderCoach
{
public:
    AudioSenderCoachv1( const Params& params )
        : AudioSenderCoach( params )
      {}

    virtual
    ~AudioSenderCoachv1()
      {}

    virtual
    void
    sendRefAudio( const char* msg );

    virtual
    void
    sendCoachAudio( const Coach& coach, const char* msg );

    virtual
    void
    sendCoachStdAudio( const clang::Msg& msg );

    virtual
    void
    sendPlayerAudio( const Player& player, const char* msg );
};




class AudioSenderCoachv7
    : public AudioSenderCoachv1
{
public:
    AudioSenderCoachv7( const Params& params )
        : AudioSenderCoachv1( params )
      {}

    virtual
    ~AudioSenderCoachv7()
      {}

    virtual
    void
    sendPlayerAudio( const Player& player, const char* msg );
};

class SerializerOnlineCoach;

class AudioSenderOnlineCoach
    : public AudioSender
{
protected:
    OnlineCoach& M_listener;
    const SerializerOnlineCoach& M_serializer;
public:
    class Params
    {
    public:
        Params( const Stadium& stadium,
                std::ostream& transport,
                OnlineCoach& listener,
                const SerializerOnlineCoach& serializer )
            : m_stadium( stadium ),
              m_transport( transport ),
              m_listener( listener ),
              m_serializer( serializer )
          {}

        const Stadium& m_stadium;
        std::ostream& m_transport;
        OnlineCoach& m_listener;
        const SerializerOnlineCoach& m_serializer;
    };

public:
    typedef Ptr(*Creator)( const rcss::AudioSenderOnlineCoach::Params& );
    typedef rcss::lib::Factory< Creator, int > Factory;

    static
    Factory&
    factory();

    AudioSenderOnlineCoach( const Params& params )
        : AudioSender( params.m_stadium, params.m_transport ),
          M_listener( params.m_listener ),
          M_serializer( params.m_serializer )
      {}

    ~AudioSenderOnlineCoach() {}

protected:
    virtual
    bool
    generalPredicate() const;

    virtual
    bool
    coachPredicate( const Coach& coach ) const;

};

class AudioSenderOnlineCoachv1
    : public AudioSenderOnlineCoach
{
public:
    AudioSenderOnlineCoachv1( const Params& params )
        : AudioSenderOnlineCoach( params )
      {}

    virtual
    ~AudioSenderOnlineCoachv1()
      {}

    virtual
    void
    sendRefAudio( const char* msg );

    virtual
    void
    sendCoachAudio( const Coach& coach, const char* msg );

    virtual
    void
    sendPlayerAudio( const Player& player, const char* msg );
};

class AudioSenderOnlineCoachv7
    : public AudioSenderOnlineCoachv1
{
public:
    AudioSenderOnlineCoachv7( const Params& params )
        : AudioSenderOnlineCoachv1( params )
      {}

    virtual
    ~AudioSenderOnlineCoachv7()
      {}

    virtual
    void
    sendPlayerAudio( const Player& player, const char* msg );
};
}

#endif // _AUDIO_H_
