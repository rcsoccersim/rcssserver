// -*-c++-*-

/***************************************************************************
                                   audio.cc
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
 *   Foundation; either version 3 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "audio.h"

#include "object.h"
#include "serializer.h"
#include "clangmsg.h"
#include "clangunsuppmsg.h"
#include "coach.h"
#include "stadium.h"
#include "player.h"
#include "random.h"
#include "utility.h"

#include <rcssbase/factory.hpp>

namespace rcss {

void
Listener::sendRefereeAudio( const char * msg )
{
    sender().sendRefereeAudio( msg );
}

void
Listener::sendCoachAudio( const Coach & coach,
                          const char * msg )
{
    sender().sendCoachAudio( coach, msg );
}

void
Listener::sendCoachStdAudio( const rcss::clang::Msg & msg )
{
    sender().sendCoachStdAudio( msg );
}

void
Listener::sendPlayerAudio( const Player & player,
                           const char * msg )
{
    sender().sendPlayerAudio( player, msg );
}

void
Listener::newCycle()
{
    sender().newCycle();
}

void
Listener::focusOn( const Player & player )
{
    sender().focusOn( player );
}

void
Listener::focusOff()
{
    sender().focusOff();
}

void
Listener::setEar( bool on,
                  Side side,
                  bool complete,
                  bool partial )
{
    sender().setEar( on, side, complete, partial );
}

void
Listener::sendOKClang()
{
    sender().sendOKClang();
}

void
Listener::sendErrorNoTeamName( const std::string & team_name )
{
    sender().sendErrorNoTeamName( team_name );
}


AudioSenderPlayer::FactoryHolder &
AudioSenderPlayer::factory()
{
    static FactoryHolder rval;
    return rval;
}


AudioSenderPlayer::AudioSenderPlayer( const Params & params )
    : AudioSender( params.M_stadium,
                   params.M_transport ),
      M_listener( params.M_listener ),
      M_serializer( params.M_serializer )
{

}

AudioSenderPlayer::~AudioSenderPlayer()
{
    //std::cerr << "delete AudioSenderPlayer" << std::endl;
}


void
AudioSenderPlayer::sendPlayerAudio( const Player & player,
                                    const char * msg )
{
    if ( &player == &M_listener )
    {
        sendSelfAudio( msg );
    }
    else
    {
        sendNonSelfPlayerAudio( player, msg );
    }
}

bool
AudioSenderPlayer::generalPredicate() const
{
    return listener().isEnabled();
}

bool
AudioSenderPlayer::coachStdPredicate( const rcss::clang::Msg & msg ) const
{
    return ( generalPredicate()
             && listener().side() == msg.getSide() );
}


bool
AudioSenderPlayer::nonSelfPlayerPredicate( const Player & player ) const
{
    // we could do this as one big return statement, but then it becomes
    // a nightmare to understand
    if( generalPredicate() )
    {
        if ( listener().canHearFullFrom( player )
             && player.pos().distance2( listener().pos() )
             <= std::pow( ServerParam::instance().audioCutDist(), 2 ) )
        {
            return true;
        }
//         if( player.pos().distance( listener().pos() )
//             <= ServerParam::instance().audio_dist )
//         {
//             if ( listener().team() == player.team() )
//             {
//                 return ( listener().hear_capacity_from_teammate
//                          >= (int)ServerParam::instance().hearDecay() );
//             }
//             else
//             {
//                 return ( listener().hear_capacity_from_opponent
//                          >= (int)ServerParam::instance().hearDecay() );
//            }
//        }
    }
    return false;
}

bool
AudioSenderPlayer::postNonSelfPlayer( const Player & player )
{
    listener().decrementHearCapacity( player );
//     if ( listener().team() == player.team() )
//         listener().hear_capacity_from_teammate
//             -= ServerParam::instance().hearDecay();
//     else
//         listener().hear_capacity_from_opponent
//             -= ServerParam::instance().hearDecay();
    return true;
}


void
AudioSenderPlayerv1::sendRefereeAudio( const char * msg )
{
    if ( generalPredicate () )
    {
        serializer().serializeRefereeAudio( transport(), M_stadium.time(), msg );
        transport() << std::ends << std::flush;
    }
}


void
AudioSenderPlayerv1::sendCoachAudio( const Coach & coach,
                                     const char * msg )
{
    if ( generalPredicate () )
    {
        std::string name;
        switch( coach.side() ) {
        case NEUTRAL:
            name = REFEREE_NAME;
            break;

        case LEFT:
            name = OLCOACH_NAME_L;
            break;

        case RIGHT:
            name = OLCOACH_NAME_R;
            break;

        default:
            // don't know what we have here so don't send anything
            return;
        }
        serializer().serializeCoachAudio( transport(),
                                          M_stadium.time(),
                                          name,
                                          msg );
        transport() << std::ends << std::flush;
    }
}


void
AudioSenderPlayerv1::sendCoachStdAudio( const rcss::clang::Msg & msg )
{
    if ( coachStdPredicate( msg ) )
    {
        std::string name;
        switch( msg.getSide() ) {
        case LEFT:
            name = OLCOACH_NAME_L;
            break;

        case RIGHT:
            name = OLCOACH_NAME_R;
            break;

        default:
            // don't know what we have here so don't send anything
            return;
        }

        if ( msg.isSupported( listener().clangMinVer(),
                              listener().clangMaxVer() ) )
        {
            serializer().serializeCoachStdAudio( transport(),
                                                 msg.getTimeSend(),
                                                 name,
                                                 msg );
        }
        else
        {
            serializer().serializeCoachStdAudio( transport(),
                                                 msg.getTimeSend(),
                                                 name,
                                                 rcss::clang::UnsuppMsg() );
        }

        transport() << std::ends << std::flush;
    }
}

void
AudioSenderPlayerv1::sendSelfAudio( const char * msg )
{
    if ( generalPredicate() )
    {
        serializer().serializeSelfAudio( transport(), M_stadium.time(), msg );
        transport() << std::ends << std::flush;
    }
}

void
AudioSenderPlayerv1::sendNonSelfPlayerAudio( const Player & player,
                                             const char * msg )
{
    if ( nonSelfPlayerPredicate( player ) )
    {
        int dir = Rad2IDeg( listener().angleFromBody( player ) );
        serializer().serializePlayerAudio( transport(),
                                           M_stadium.time(),
                                           dir,
                                           msg );
        transport() << std::ends << std::flush;
        postNonSelfPlayer( player );
    }
}

void
AudioSenderPlayerv1::sendOKClang()
{
    serializer().serializeOKClang( transport(),
                                   listener().clangMinVer(),
                                   listener().clangMaxVer() );
    transport() << std::ends << std::flush;
}

void
AudioSenderPlayerv1::sendErrorNoTeamName( const std::string& team_name )
{
    serializer().serializeErrorNoTeamName( transport(), team_name );
    transport() << std::ends << std::flush;
}


void
AudioSenderPlayerv7::sendCoachAudio( const Coach & coach,
                                     const char * msg )
{
    if ( generalPredicate() )
    {
        std::string name;
        switch ( coach.side() ) {
        case NEUTRAL:
            name = COACH_NAME;
            break;

        case LEFT:
            name = OLCOACH_NAME_L;
            break;

        case RIGHT:
            name = OLCOACH_NAME_R;
            break;

        default:
            // don't know what we have hear so don't send anything
            return;
        }
        serializer().serializeCoachAudio( transport(),
                                          M_stadium.time(),
                                          name,
                                          msg );
        transport() << std::ends << std::flush;
    }
}

void
AudioSenderPlayerv7::sendNonSelfPlayerAudio( const Player & player,
                                             const char * msg )
{
    if ( nonSelfPlayerPredicate( player ) )
    {
        int dir = Rad2IDegRound( listener().angleFromBody( player ) );
        serializer().serializePlayerAudio( transport(),
                                           M_stadium.time(),
                                           dir,
                                           msg );
        transport() << std::ends << std::flush;
        postNonSelfPlayer( player );
    }
}


AudioSenderPlayerv8::~AudioSenderPlayerv8()
{
    for ( player_msg_cont_t::iterator i = M_player_msgs.begin();
          i != M_player_msgs.end();
          ++i )
    {
        free( i->second );
        i->second = NULL;
    }
    M_player_msgs.clear();

    for ( self_msg_cont_t::iterator i = M_self_msgs.begin();
          i != M_self_msgs.end();
         ++i )
    {
        free( *i );
        *i = NULL;
    }
    M_self_msgs.clear();

    for ( coach_msg_cont_t::iterator i = M_coach_msgs.begin();
          i != M_coach_msgs.end();
          ++i )
    {
        free( i->second );
        i->second = NULL;
    }
    M_coach_msgs.clear();
}

bool
AudioSenderPlayerv8::nonSelfPlayerPredicate( const Player & player ) const
{
    return ( generalPredicate()
             && ( player.pos().distance2( listener().pos() )
                  <= std::pow( ServerParam::instance().audioCutDist(), 2 ) ) );
}

bool
AudioSenderPlayerv8::nonSelfPlayerFullPredicate( const Player & player ) const
{
    return listener().canHearFullFrom( player );
//     if ( listener().team() == player.team() )
//     {
//         return ( listener().hear_capacity_from_teammate
//                  >= (int)ServerParam::instance().hearDecay() );
//     }
//     else
//     {
//         return ( listener().hear_capacity_from_opponent
//                  >= (int)ServerParam::instance().hearDecay() );
//     }
}


void
AudioSenderPlayerv8::sendCoachAudio( const Coach & coach,
                                     const char * msg )
{
    char * msg_copy = strdup( msg );
    if ( msg_copy == NULL )
    {
        std::cerr << "Error:  could not alocate memory to cache coach audio message\n";
    }
    else
    {
        M_coach_msgs.push_back( coach_key_value_t( &coach,
                                                   msg_copy ) );
    }
}


void
AudioSenderPlayerv8::sendSelfAudio( const char * msg )
{
    char * msg_copy = strdup( msg );
    if ( msg_copy == NULL )
    {
        std::cerr << "Error:  could not alocate memory to cache player's own audio message\n";
    }
    else
    {
        M_self_msgs.push_back( msg_copy );
    }
}


void
AudioSenderPlayerv8::sendNonSelfPlayerAudio( const Player & player,
                                             const char * msg )
{
    if ( ! nonSelfPlayerPredicate( player ) )
    {
        return;
    }

    if ( ! M_left_complete
         && ! M_left_partial
         && player.side() == LEFT )
    {
        // if ear off all, messages are never cached
        return;
    }

    if ( ! M_right_complete
         && ! M_right_partial
         && player.side() == RIGHT )
    {
        // if ear off all, messages are never cached
        return;
    }

    char * msg_copy = strdup( msg );
    if ( msg_copy == NULL )
    {
        std::cerr << "Error:  could not alocate memory to cache player audio message\n";
    }
    else
    {
        M_player_msgs.insert( player_key_value_t( &player,
                                                  msg_copy ) );
    }
}


void
AudioSenderPlayerv8::newCycle()
{
    for ( coach_msg_cont_t::iterator i = M_coach_msgs.begin();
          i != M_coach_msgs.end();
          ++i )
    {
        sendCachedCoachAudio( *(i->first), i->second );
        free( i->second );
    }
    M_coach_msgs.clear();

    for ( self_msg_cont_t::iterator i = M_self_msgs.begin();
          i != M_self_msgs.end();
          ++i )
    {
        sendCachedSelfAudio( *i );
        free( *i );
    }
    M_self_msgs.clear();

    while ( ! M_player_msgs.empty() )
    {
        State::key_value_t data = M_state_p->getMsg( M_player_msgs );
        sendCachedNonSelfPlayerAudio( *(data.first), data.second );
        free( data.second );
    }
}

void
AudioSenderPlayerv8::focusOn( const Player & player )
{
    M_focused.focusOn ( &player );
    M_state_p = &M_focused;
    M_focus_count++;
}

void
AudioSenderPlayerv8::focusOff()
{
    M_state_p = &M_unfocused;
    M_focus_count++;
}


void
AudioSenderPlayerv8::sendCachedCoachAudio( const Coach & coach,
                                           const char * msg )
{
    AudioSenderPlayerv7::sendCoachAudio( coach, msg );
}

void
AudioSenderPlayerv8::sendCachedSelfAudio( const char * msg )
{
    AudioSenderPlayerv7::sendSelfAudio( msg );
}

void
AudioSenderPlayerv8::sendCachedNonSelfPlayerAudio( const Player & player,
                                                   const char * msg )
{
    //if ( nonSelfPlayerPredicate( player ) )
    {
        if ( nonSelfPlayerFullPredicate( player ) )
        {
            if ( ( player.side() == LEFT && M_left_complete )
                 || ( player.side() == RIGHT && M_right_complete ) )
            {
                int dir = Rad2IDeg( listener().angleFromBody( player ) );
                if ( listener().team() == player.team() )
                {
                    serializer().serializeAllyAudioFull( transport(),
                                                         M_stadium.time(),
                                                         dir,
                                                         player.unum(),
                                                         msg );
                }
                else
                {
                    serializer().serializeOppAudioFull( transport(),
                                                        M_stadium.time(),
                                                        dir,
                                                        msg );
                }
                transport() << std::ends << std::flush;
                postNonSelfPlayer( player );
            }
        }
        else
        {
            if ( ( player.side() == LEFT && M_left_partial )
                 || ( player.side() == RIGHT && M_right_partial ) )
            {
                if ( listener().team() == player.team() )
                {
                    serializer().serializeAllyAudioShort( transport(),
                                                          M_stadium.time(),
                                                          player.unum() );
                }
                else
                {
                    serializer().serializeOppAudioShort( transport(),
                                                         M_stadium.time() );
                }
                transport() << std::ends << std::flush;
            }
        }
    }
}

AudioSenderPlayerv8::Unfocused::key_value_t
AudioSenderPlayerv8::Unfocused::getMsg( msg_cont_t & msgs )
{
    if ( msgs.size() == 1 )
    {
        key_value_t rval = *(msgs.begin());
        msgs.clear();
        return rval;
    }
    else
    {
        //boost::uniform_smallint<> rand_int( 0, msgs.size() - 1 );
        //int idx = rand_int( rcss::random::DefaultRNG::instance() );
        int idx = irand( msgs.size() );
        msg_cont_t::iterator iter = msgs.begin();
        for( int i = 0; i < idx; ++i )
            ++iter;

        key_value_t rval = *iter;
        msgs.erase( iter );
        return rval;
    }
}

AudioSenderPlayerv8::Focused::key_value_t
AudioSenderPlayerv8::Focused::getMsg( msg_cont_t & msgs )
{
#if 1
     std::pair< msg_cont_t::iterator, msg_cont_t::iterator > iters;
     //iters = equal_range ( msgs.begin (), msgs.end (), M_key, msgs.value_comp () );
     //can't get equal_range to work with a mulit_map.  Do it manually instead.
     iters.first = msgs.find( M_key );
     iters.second = msgs.end();

     for ( msg_cont_t::iterator i = iters.first;
           i != iters.second; //msgs.end();
           ++i )
     {
         if ( i->first != M_key )
         {
             iters.second = i;
             break;
         }
     }
#else
    if ( M_key == NULL )
        return Unfocused::getMsg( msgs );

    std::pair< msg_cont_t::iterator, msg_cont_t::iterator > iters
        = msgs.equal_range( M_key );
#endif
    if ( iters.first == iters.second )
        // then there was no message from the player we are focused
        // on, so just get a message at random
        return Unfocused::getMsg( msgs );

    int count = 0;
    for ( msg_cont_t::iterator i = iters.first;
          i != iters.second;
          ++i )
    {
        ++count;
    }

    if ( count == 1 )
    {
        // then there was only one message from the player we are
        // focused on, so just return that.
        key_value_t rval = *(iters.first);
        msgs.erase( iters.first );
        return rval;
    }
    // otherwise return one of those messages at random
    //boost::uniform_smallint<> rand_int( 0, count - 1 );
    //int idx = rand_int( rcss::random::DefaultRNG::instance() );
    int idx = irand( count );
    msg_cont_t::iterator iter = iters.first;
    for( int i = 0; i < idx; i++ )
        iter++;

    msgs.erase( iter );
    return *iter;
}


void
AudioSenderPlayerv8::setEar( bool on,
                             Side side,
                             bool complete,
                             bool partial )
{
    if ( side != RIGHT )
    {
        if ( complete )
        {
            M_left_complete = on;
            //std::cout << "Set left complete to: " << on << std::endl;
        }
        if ( partial )
        {
            M_left_partial = on;
            //std::cout << "Set left partial to: " << on << std::endl;
        }
    }
    if ( side != LEFT )
    {
        if ( complete )
        {
            M_right_complete = on;
            //std::cout << "Set right complete to: " << on << std::endl;
        }
        if ( partial )
        {
            M_right_partial = on;
            //std::cout << "Set right partial to: " << on << std::endl;
        }
    }
}

AudioSenderCoach::FactoryHolder &
AudioSenderCoach::factory()
{
    static FactoryHolder rval;
    return rval;
}


AudioSenderCoach::AudioSenderCoach( const Params & params )
    : AudioSender( params.M_stadium,
                   params.M_transport ),
      M_listener( params.M_listener ),
      M_serializer( params.M_serializer )
{

}

AudioSenderCoach::~AudioSenderCoach()
{
    //std::cerr << "delete AudioSenderCoach" << std::endl;
}


bool
AudioSenderCoach::generalPredicate() const
{
    return ( ( ServerParam::instance().coachMode()
               || ServerParam::instance().coachWithRefereeMode() )
             && listener().isEarOn() );
}



void
AudioSenderCoachv1::sendRefereeAudio( const char * msg )
{
    if ( generalPredicate() )
    {
        serializer().serializeRefereeAudio( transport(), M_stadium.time(), msg );
        transport() << std::ends << std::flush;
    }
}

void
AudioSenderCoachv1::sendCoachAudio( const Coach & coach,
                                    const char * msg )
{
    if ( generalPredicate() )
    {
        std::string name;
        switch( coach.side() ) {
        case NEUTRAL:
            name = REFEREE_NAME;
            break;

        case LEFT:
            name = OLCOACH_NAME_L;
            break;

        case RIGHT:
            name = OLCOACH_NAME_R;
            break;

        default:
            // don't know what we have hear so don't send anything
            return;
        }
        serializer().serializeCoachAudio( transport(),
                                          M_stadium.time(),
                                          name,
                                          msg );
        transport() << std::ends << std::flush;
    }
}

void
AudioSenderCoachv1::sendCoachStdAudio( const rcss::clang::Msg & msg )
{
    if ( generalPredicate() )
    {
        std::string name;
        switch ( msg.getSide() ) {
        case LEFT:
            name = OLCOACH_NAME_L;
            break;

        case RIGHT:
            name = OLCOACH_NAME_R;
            break;

        default:
            // don't know what we have hear so don't send anything
            return;
        }
        serializer().serializeCoachStdAudio( transport(),
                                             M_stadium.time(),
                                             name,
                                             msg );
        transport() << std::ends << std::flush;
    }
}

void
AudioSenderCoachv1::sendPlayerAudio( const Player & player,
                                     const char * msg )
{
    if ( generalPredicate() )
    {
        serializer().serializePlayerAudio( transport(),
                                           M_stadium.time(),
                                           player.name(),
                                           msg );
        transport() << std::ends << std::flush;
    }
}

void
AudioSenderCoachv7::sendPlayerAudio( const Player & player,
                                     const char * msg )
{
    if ( generalPredicate() )
    {
        serializer().serializePlayerAudio( transport(),
                                           M_stadium.time(),
                                           player.shortName(),
                                           msg );
        transport() << std::ends << std::flush;
    }
}


AudioSenderOnlineCoach::FactoryHolder &
AudioSenderOnlineCoach::factory()
{
    static FactoryHolder rval;
    return rval;
}


AudioSenderOnlineCoach::AudioSenderOnlineCoach( const Params & params )
    : AudioSender( params.M_stadium,
                   params.M_transport ),
      M_listener( params.M_listener ),
      M_serializer( params.M_serializer )
{

}

AudioSenderOnlineCoach::~AudioSenderOnlineCoach()
{
    //std::cerr << "delete AudioSenderOnlineCoach" << std::endl;
}


bool
AudioSenderOnlineCoach::generalPredicate() const
{
    return listener().assigned();
}


bool
AudioSenderOnlineCoach::coachPredicate( const Coach & coach ) const
{
    return generalPredicate() && coach.side() == NEUTRAL;
}


void
AudioSenderOnlineCoachv1::sendRefereeAudio( const char * msg )
{
    if ( generalPredicate() )
    {
        serializer().serializeRefereeAudio( transport(),
                                            M_stadium.time(),
                                            REFEREE_NAME,
                                            msg );
        transport() << std::ends << std::flush;
    }
}

void
AudioSenderOnlineCoachv1::sendCoachAudio( const Coach & coach,
                                          const char * msg )
{
    if ( coachPredicate( coach ) )
    {
        serializer().serializeRefereeAudio ( transport(),
                                             M_stadium.time(),
                                             REFEREE_NAME,
                                             msg );
        transport() << std::ends << std::flush;
    }
}

void
AudioSenderOnlineCoachv1::sendPlayerAudio( const Player & player,
                                           const char * msg )
{
    if ( generalPredicate() )
    {
        serializer().serializePlayerAudio( transport(),
                                           M_stadium.time(),
                                           player.name(),
                                           msg );
        transport() << std::ends << std::flush;
    }
}

void
AudioSenderOnlineCoachv7::sendPlayerAudio( const Player & player,
                                           const char * msg )
{
    if ( generalPredicate() )
    {
        serializer().serializePlayerAudio( transport(),
                                           M_stadium.time(),
                                           player.shortName(),
                                           msg );
        transport() << std::ends << std::flush;
    }
}

namespace audio {

template< typename Sender >
AudioSender::Ptr
create( const AudioSenderPlayer::Params & params )
{
    return AudioSender::Ptr( new Sender( params ) );
}

RegHolder vp1 = AudioSenderPlayer::factory().autoReg( &create< AudioSenderPlayerv1 >, 1 );
RegHolder vp2 = AudioSenderPlayer::factory().autoReg( &create< AudioSenderPlayerv1 >, 2 );
RegHolder vp3 = AudioSenderPlayer::factory().autoReg( &create< AudioSenderPlayerv1 >, 3 );
RegHolder vp4 = AudioSenderPlayer::factory().autoReg( &create< AudioSenderPlayerv1 >, 4 );
RegHolder vp5 = AudioSenderPlayer::factory().autoReg( &create< AudioSenderPlayerv1 >, 5 );
RegHolder vp6 = AudioSenderPlayer::factory().autoReg( &create< AudioSenderPlayerv1 >, 6 );
RegHolder vp7 = AudioSenderPlayer::factory().autoReg( &create< AudioSenderPlayerv7 >, 7 );
RegHolder vp8 = AudioSenderPlayer::factory().autoReg( &create< AudioSenderPlayerv8 >, 8 );
RegHolder vp9 = AudioSenderPlayer::factory().autoReg( &create< AudioSenderPlayerv8 >, 9 );
RegHolder vp10 = AudioSenderPlayer::factory().autoReg( &create< AudioSenderPlayerv8 >, 10 );
RegHolder vp11 = AudioSenderPlayer::factory().autoReg( &create< AudioSenderPlayerv8 >, 11 );
RegHolder vp12 = AudioSenderPlayer::factory().autoReg( &create< AudioSenderPlayerv8 >, 12 );
RegHolder vp13 = AudioSenderPlayer::factory().autoReg( &create< AudioSenderPlayerv8 >, 13 );
RegHolder vp14 = AudioSenderPlayer::factory().autoReg( &create< AudioSenderPlayerv8 >, 14 );
RegHolder vp15 = AudioSenderPlayer::factory().autoReg( &create< AudioSenderPlayerv8 >, 15 );
//RegHolder vp16 = AudioSenderPlayer::factory().autoReg( &create< AudioSenderPlayerv8 >, 16 );

template< typename Sender >
AudioSender::Ptr
create( const AudioSenderCoach::Params & params )
{
    return AudioSender::Ptr( new Sender( params ) );
}

RegHolder vc1 = AudioSenderCoach::factory().autoReg( &create< AudioSenderCoachv1 >, 1 );
RegHolder vc2 = AudioSenderCoach::factory().autoReg( &create< AudioSenderCoachv1 >, 2 );
RegHolder vc3 = AudioSenderCoach::factory().autoReg( &create< AudioSenderCoachv1 >, 3 );
RegHolder vc4 = AudioSenderCoach::factory().autoReg( &create< AudioSenderCoachv1 >, 4 );
RegHolder vc5 = AudioSenderCoach::factory().autoReg( &create< AudioSenderCoachv1 >, 5 );
RegHolder vc6 = AudioSenderCoach::factory().autoReg( &create< AudioSenderCoachv1 >, 6 );
RegHolder vc7 = AudioSenderCoach::factory().autoReg( &create< AudioSenderCoachv7 >, 7 );
RegHolder vc8 = AudioSenderCoach::factory().autoReg( &create< AudioSenderCoachv7 >, 8 );
RegHolder vc9 = AudioSenderCoach::factory().autoReg( &create< AudioSenderCoachv7 >, 9 );
RegHolder vc10 = AudioSenderCoach::factory().autoReg( &create< AudioSenderCoachv7 >, 10 );
RegHolder vc11 = AudioSenderCoach::factory().autoReg( &create< AudioSenderCoachv7 >, 11 );
RegHolder vc12 = AudioSenderCoach::factory().autoReg( &create< AudioSenderCoachv7 >, 12 );
RegHolder vc13 = AudioSenderCoach::factory().autoReg( &create< AudioSenderCoachv7 >, 13 );
RegHolder vc14 = AudioSenderCoach::factory().autoReg( &create< AudioSenderCoachv7 >, 14 );
RegHolder vc15 = AudioSenderCoach::factory().autoReg( &create< AudioSenderCoachv7 >, 15 );
//RegHolder vc16 = AudioSenderCoach::factory().autoReg( &create< AudioSenderCoachv7 >, 16 );

template< typename Sender >
AudioSender::Ptr
create( const AudioSenderOnlineCoach::Params & params )
{
    return AudioSender::Ptr( new Sender( params ) );
}

RegHolder voc1 = AudioSenderOnlineCoach::factory().autoReg( &create< AudioSenderOnlineCoachv1 >, 1 );
RegHolder voc2 = AudioSenderOnlineCoach::factory().autoReg( &create< AudioSenderOnlineCoachv1 >, 2 );
RegHolder voc3 = AudioSenderOnlineCoach::factory().autoReg( &create< AudioSenderOnlineCoachv1 >, 3 );
RegHolder voc4 = AudioSenderOnlineCoach::factory().autoReg( &create< AudioSenderOnlineCoachv1 >, 4 );
RegHolder voc5 = AudioSenderOnlineCoach::factory().autoReg( &create< AudioSenderOnlineCoachv1 >, 5 );
RegHolder voc6 = AudioSenderOnlineCoach::factory().autoReg( &create< AudioSenderOnlineCoachv1 >, 6 );
RegHolder voc7 = AudioSenderOnlineCoach::factory().autoReg( &create< AudioSenderOnlineCoachv7 >, 7 );
RegHolder voc8 = AudioSenderOnlineCoach::factory().autoReg( &create< AudioSenderOnlineCoachv7 >, 8 );
RegHolder voc9 = AudioSenderOnlineCoach::factory().autoReg( &create< AudioSenderOnlineCoachv7 >, 9 );
RegHolder voc10 = AudioSenderOnlineCoach::factory().autoReg( &create< AudioSenderOnlineCoachv7 >, 10 );
RegHolder voc11 = AudioSenderOnlineCoach::factory().autoReg( &create< AudioSenderOnlineCoachv7 >, 11 );
RegHolder voc12 = AudioSenderOnlineCoach::factory().autoReg( &create< AudioSenderOnlineCoachv7 >, 12 );
RegHolder voc13 = AudioSenderOnlineCoach::factory().autoReg( &create< AudioSenderOnlineCoachv7 >, 13 );
RegHolder voc14 = AudioSenderOnlineCoach::factory().autoReg( &create< AudioSenderOnlineCoachv7 >, 14 );
RegHolder voc15 = AudioSenderOnlineCoach::factory().autoReg( &create< AudioSenderOnlineCoachv7 >, 15 );
//RegHolder voc16 = AudioSenderOnlineCoach::factory().autoReg( &create< AudioSenderOnlineCoachv7 >, 16 );
}
}
