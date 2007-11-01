// -*-c++-*-

/***************************************************************************
                                initsender.cc
                  Classes for sending init messages
                             -------------------
    begin                : 22-AUG-2002
    copyright            : (C) 2002 by The RoboCup Soccer Simulator
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

#include "initsender.h"

#include "coach.h"
#include "player.h"
#include "serializer.h"
#include "field.h"

namespace rcss
{

/*!
//===================================================================
//
//  CLASS: InitSender
//
//  DESC: Base class for the init protocol.
//
//===================================================================
*/

InitSender::InitSender( std::ostream& transport,
                        const boost::shared_ptr< InitSenderCommon >& common )
    : Sender( transport ), m_common_sender( common )
{}

InitSender::~InitSender()
{}





//----------------------


void
InitSenderCommonV7::sendServerParams()
{
    serializer().serializeServerParamBegin( transport() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().goalWidth() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().inertiaMoment() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().playerSize() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().playerDecay() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().playerRand() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().playerWeight() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().playerSpeedMax() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().playerAccelMax() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().staminaMax() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().staminaInc() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().recoverInit() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().recoverDecThr() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().recoverMin() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().recoverDec() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().effortInit() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().effortDecThr() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().effortMin() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().effortDec() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().effortIncThr() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().effortInc() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().kickRand() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().teamActuatorNoise() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().playerRandFactorLeft() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().playerRandFactorRight() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().kickRandFactorLeft() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().kickRandFactorRight() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().ballSize() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().ballDecay() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().ballRand() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().ballWeight() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().ballSpeedMax() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().ballAccelMax() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().dashPowerRate() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().kickPowerRate() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().kickableMargin() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().controlRadius() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().CtlRadiusWidth() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().maxPower() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().minPower() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().maxMoment() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().minMoment() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().maxNeckMoment() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().minNeckMoment() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().maxNeckAngle() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().minNeckAngle() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().visAngleDeg() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().visibleDistance() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().windDir() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().windForce() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().windAngle() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().windRand() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().kickableArea() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().catchAreaLength() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().catchAreaWidth() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().catchProb() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().goalieMaxMoves() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().cornerKickMargin() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().offsideActiveArea() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().windNone() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().windRandom() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().freeformCountMax() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().freeformMsgSize() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().clangWinSize() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().clangDefineWin() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().clangMetaWin() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().clangAdviceWin() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().clangInfoWin() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().clangMessDelay() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().clangMessPerCycle() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().halfTime() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().simStep() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().sendStep() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().recvStep() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().senseBodyStep() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().lcmStep() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().sayMsgSize() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().hearMax() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().hearInc() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().hearDecay() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().catchBanCycle() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().slowDownFactor() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().useOffside() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().kickOffOffside() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().offsideKickMargin() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().audioDist() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().distQStep() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().landQStep() );
#ifdef NEW_QSTEP
    serializer().serializeParam( transport(),
                                 ServerParam::instance().dirQStep() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().distQStepLeft() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().distQStepRight() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().landQStepLeft() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().landQStepRight() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().dirQStepLeft() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().dirQStepRight() );
#else
    serializer().serializeParam( transport(),
                                 -1 );
    serializer().serializeParam( transport(),
                                 -1 );
    serializer().serializeParam( transport(),
                                 -1 );
    serializer().serializeParam( transport(),
                                 -1 );
    serializer().serializeParam( transport(),
                                 -1 );
    serializer().serializeParam( transport(),
                                 -1 );
    serializer().serializeParam( transport(),
                                 -1 );
#endif
    serializer().serializeParam( transport(),
                                 ServerParam::instance().coachMode() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().coachWithRefereeMode() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().coachOldHear() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().coachVisualStep() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().startGoalLeft() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().startGoalRight() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().fullstateLeft() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().fullstateRight() );
    serializer().serializeParam( transport(),
                                 ServerParam::instance().dropTime() );
    serializer().serializeServerParamEnd( transport() );
    transport() << std::ends << std::flush;
}

void
InitSenderCommonV7::sendPlayerParams()
{
    serializer().serializePlayerParamBegin( transport() );
    serializer().serializeParam( transport(),
                                 PlayerParam::instance().playerTypes() );
    serializer().serializeParam( transport(),
                                 PlayerParam::instance().subsMax() );
    serializer().serializeParam( transport(),
                                 PlayerParam::instance().ptMax() );

    serializer().serializeParam( transport(),
                                 PlayerParam::instance().playerSpeedMaxDeltaMin() );
    serializer().serializeParam( transport(),
                                 PlayerParam::instance().playerSpeedMaxDeltaMax() );
    serializer().serializeParam( transport(),
                                 PlayerParam::instance().staminaIncMaxDeltaFactor() );

    serializer().serializeParam( transport(),
                                 PlayerParam::instance().playerDecayDeltaMin() );
    serializer().serializeParam( transport(),
                                 PlayerParam::instance().playerDecayDeltaMax() );
    serializer().serializeParam( transport(),
                                 PlayerParam::instance().inertiaMomentDeltaFactor() );

    serializer().serializeParam( transport(),
                                 PlayerParam::instance().dashPowerRateDeltaMin() );
    serializer().serializeParam( transport(),
                                 PlayerParam::instance().dashPowerRateDeltaMax() );
    serializer().serializeParam( transport(),
                                 PlayerParam::instance().playerSizeDeltaFactor() );

    serializer().serializeParam( transport(),
                                 PlayerParam::instance().kickableMarginDeltaMin() );
    serializer().serializeParam( transport(),
                                 PlayerParam::instance().kickableMarginDeltaMax() );
    serializer().serializeParam( transport(),
                                 PlayerParam::instance().kickRandDeltaFactor() );

    serializer().serializeParam( transport(),
                                 PlayerParam::instance().extraStaminaDeltaMin() );
    serializer().serializeParam( transport(),
                                 PlayerParam::instance().extraStaminaDeltaMax() );
    serializer().serializeParam( transport(),
                                 PlayerParam::instance().effortMaxDeltaFactor() );
    serializer().serializeParam( transport(),
                                 PlayerParam::instance().effortMinDeltaFactor() );
    serializer().serializePlayerParamEnd( transport() );
    transport() << std::ends << std::flush;
}

void
InitSenderCommonV7::sendPlayerTypes()
{
    for ( int i = 0; i < PlayerParam::instance().playerTypes(); ++i )
    {
        const HeteroPlayer * type = stadium().playerType( i );
        if ( type != NULL )
        {
            serializer().serializePlayerTypeBegin( transport() );
            serializer().serializeParam( transport(),
                                         i );
            serializer().serializeParam( transport(),
                                         type->playerSpeedMax() );
            serializer().serializeParam( transport(),
                                         type->staminaIncMax() );
            serializer().serializeParam( transport(),
                                         type->playerDecay() );
            serializer().serializeParam( transport(),
                                         type->inertiaMoment() );
            serializer().serializeParam( transport(),
                                         type->dashPowerRate() );
            serializer().serializeParam( transport(),
                                         type->playerSize() );
            serializer().serializeParam( transport(),
                                         type->kickableMargin() );
            serializer().serializeParam( transport(),
                                         type->kickRand() );
            serializer().serializeParam( transport(),
                                         type->extraStamina() );
            serializer().serializeParam( transport(),
                                         type->effortMax() );
            serializer().serializeParam( transport(),
                                         type->effortMin() );
            serializer().serializePlayerTypeEnd( transport() );
            transport() << std::ends << std::flush;
        }
    }
}

//-------------------------------------

void
InitSenderCommonV8::sendServerParams()
{
    //         LocalSerializer ser( serializer(), transport(), version() );

    serializer().serializeServerParamBegin( transport() );
    std::for_each( ServerParam::instance().verMap().begin(),
                   ServerParam::instance().verMap().end(),
                   std::bind1st( std::mem_fun( &rcss::InitSenderCommonV8::sendServerParam ),
                                 this ) );
    serializer().serializeServerParamEnd( transport() );
    transport() << std::ends << std::flush;
}

void
InitSenderCommonV8::doSendServerParam( ServerParam::VerMap::value_type param )
{
    if( param.second <= version() )
    {

        int ivalue;
        if( ServerParam::instance().getInt( param.first, ivalue ) )
        {
            serializer().serializeParam( transport(),
                                         param.first,
                                         ivalue );
            return;
        }

        bool bvalue;
        if( ServerParam::instance().getBool( param.first, bvalue ) )
        {
            serializer().serializeParam( transport(),
                                         param.first,
                                         bvalue );
            return;
        }

        double dvalue;
        if( ServerParam::instance().getDoub( param.first, dvalue ) )
        {
            serializer().serializeParam( transport(),
                                         param.first,
                                         dvalue );
            return;
        }

        std::string svalue;
        if( ServerParam::instance().getStr( param.first, svalue ) )
        {
            serializer().serializeParam( transport(),
                                         param.first,
                                         svalue );
            return;
        }
    }
}

void
InitSenderCommonV8::sendPlayerParams()
{
    serializer().serializePlayerParamBegin( transport() );
    std::for_each( PlayerParam::instance().verMap().begin(),
                   PlayerParam::instance().verMap().end(),
                   std::bind1st( std::mem_fun( &rcss::InitSenderCommonV8::sendPlayerParam ),
                                 this ) );
    serializer().serializePlayerParamEnd( transport() );
    transport() << std::ends << std::flush;
}


void
InitSenderCommonV8::doSendPlayerParam( PlayerParam::VerMap::value_type param )
{
    if( param.second <= version() )
    {
        int ivalue;
        if( PlayerParam::instance().getInt( param.first, ivalue ) )
        {
            serializer().serializeParam( transport(),
                                         param.first,
                                         ivalue );
            return;
        }

        bool bvalue;
        if( PlayerParam::instance().getBool( param.first, bvalue ) )
        {
            serializer().serializeParam( transport(),
                                         param.first,
                                         bvalue );
            return;
        }

        double dvalue;
        if( PlayerParam::instance().getDoub( param.first, dvalue ) )
        {
            serializer().serializeParam( transport(),
                                         param.first,
                                         dvalue );
            return;
        }

        std::string svalue;
        if( PlayerParam::instance().getStr( param.first, svalue ) )
        {
            serializer().serializeParam( transport(),
                                         param.first,
                                         svalue );
            return;
        }
    }
}

void
InitSenderCommonV8::sendPlayerTypes()
{
    for ( int i = 0; i < PlayerParam::instance().playerTypes(); ++i )
    {
        const HeteroPlayer * type = stadium().playerType( i );
        if ( type != NULL )
        {
            serializer().serializePlayerTypeBegin( transport() );
            serializer().serializeParam( transport(),
                                         "id",
                                         i );
            serializer().serializeParam( transport(),
                                         "player_speed_max",
                                         type->playerSpeedMax () );
            serializer().serializeParam( transport(),
                                         "stamina_inc_max",
                                         type->staminaIncMax () );
            serializer().serializeParam( transport(),
                                         "player_decay",
                                         type->playerDecay () );
            serializer().serializeParam( transport(),
                                         "inertia_moment",
                                         type->inertiaMoment () );
            serializer().serializeParam( transport(),
                                         "dash_power_rate",
                                         type->dashPowerRate () );
            serializer().serializeParam( transport(),
                                         "player_size",
                                         type->playerSize () );
            serializer().serializeParam( transport(),
                                         "kickable_margin",
                                         type->kickableMargin () );
            serializer().serializeParam( transport(),
                                         "kick_rand",
                                         type->kickRand () );
            serializer().serializeParam( transport(),
                                         "extra_stamina",
                                         type->extraStamina () );
            serializer().serializeParam( transport(),
                                         "effort_max",
                                         type->effortMax () );
            serializer().serializeParam( transport(),
                                         "effort_min",
                                         type->effortMin () );
            serializer().serializePlayerTypeEnd( transport() );
            transport() << std::ends << std::flush;
        }
    }
}

/*!
//===================================================================
//
//  CLASS: InitSenderPlayer
//
//  DESC: Base class for the init protocol for players.
//
//===================================================================
*/

InitSenderPlayer::Factory&
InitSenderPlayer::factory()
{ static Factory rval; return rval; }


InitSenderPlayer::InitSenderPlayer( const Params& params,
                                    const boost::shared_ptr< InitSenderCommon > common )
    : InitSender( params.m_transport, common ),
      M_serializer( params.m_ser ),
      M_self( params.m_self ),
      M_stadium( params.m_stadium )
{}

InitSenderPlayer::~InitSenderPlayer()
{}





/*!
//===================================================================
//
//  CLASS: InitSenderPlayerV1
//
//  DESC: version 1 of the init protocol.
//
//===================================================================
*/


InitSenderPlayerV1::InitSenderPlayerV1( const Params& params )
    : InitSenderPlayer( params,
                        boost::shared_ptr< rcss::InitSenderCommon >( new InitSenderCommonV1( params.m_transport,
                                                                                             params.m_ser,
                                                                                             params.m_stadium,
                                                                                             (unsigned int)params.m_self.version() ) ) )
{}

InitSenderPlayerV1::InitSenderPlayerV1( const Params& params,
                                        const boost::shared_ptr< InitSenderCommon > common )
    : InitSenderPlayer( params, common )
{}

InitSenderPlayerV1::~InitSenderPlayerV1()
{}

void
InitSenderPlayerV1::sendInit()
{
    serializer().serializeInit( transport(),
                                SideStr( self().team()->side() ),
                                self().unum(),
                                stadium().playmode() );
    transport() << std::ends << std::flush;
}

void
InitSenderPlayerV1::sendReconnect()
{
    //          std::cerr << "Sending reconnect" << std::endl;
    //          transport() << "test" << std::ends << std::flush;
    //          std::cerr << "side = " << self().team->side << std::endl;
    //          std::cerr << "mode = " << stadium().mode << std::endl;

    serializer().serializeReconnect( transport(),
                                     SideStr( self().team()->side() ),
                                     stadium().playmode() );
    transport() << std::ends << std::flush;
    //          std::cerr << "Sent" << std::endl;
}

void
InitSenderPlayerV1::sendScore()
{
    int our_score = self().team()->point();
    int opp_score = 0;
    if( self().team()->side() == LEFT
        && stadium().teamRight().enabled() )
    {
        opp_score = stadium().teamRight().point();
    }
    else if( self().team()->side() == RIGHT
             && stadium().teamLeft().enabled() )
    {
        opp_score = stadium().teamLeft().point();
    }
    serializer().serializeScore( transport(),
                                 stadium().time(),
                                 our_score,
                                 opp_score );
    transport() << std::ends << std::flush;
}

void
InitSenderPlayerV1::sendChangedPlayers()
{}

/*!
//===================================================================
//
//  CLASS: InitSenderPlayerV7
//
//  DESC: version 7 of the init protocol.
//
//===================================================================
*/


InitSenderPlayerV7::InitSenderPlayerV7( const Params& params )
    : InitSenderPlayerV1( params,
                          boost::shared_ptr< rcss::InitSenderCommon >( new InitSenderCommonV7( params.m_transport,
                                                                                               params.m_ser,
                                                                                               params.m_stadium,
                                                                                               (unsigned int)params.m_self.version() ) ) )
{}

InitSenderPlayerV7::InitSenderPlayerV7( const Params& params,
                                        const boost::shared_ptr< InitSenderCommon > common )
    : InitSenderPlayerV1( params, common )
{}

InitSenderPlayerV7::~InitSenderPlayerV7()
{}

void
InitSenderPlayerV7::sendChangedPlayers()
{
    const Stadium::PlayerCont::const_iterator end = stadium().players().end();
    for ( Stadium::PlayerCont::const_iterator p = stadium().players().begin();
          p != end;
          ++p )
    {
        if ( (*p)->playerTypeId() == 0 ) continue;

        if ( self().team() == (*p)->team() )
        {
            serializer().serializeChangePlayer( transport(),
                                                (*p)->unum(),
                                                (*p)->playerTypeId() );
        }
        else
        {
            serializer().serializeChangePlayer( transport(),
                                                (*p)->unum() );
        }
        transport() << std::ends << std::flush;
    }

}

/*!
//===================================================================
//
//  CLASS: InitSenderPlayerV8
//
//  DESC: version 8 of the init protocol.
//
//===================================================================
*/


InitSenderPlayerV8::InitSenderPlayerV8( const Params& params )
    : InitSenderPlayerV7( params,
                          boost::shared_ptr< rcss::InitSenderCommon >( new InitSenderCommonV8( params.m_transport,
                                                                                               params.m_ser,
                                                                                               params.m_stadium,
                                                                                               (unsigned int)params.m_self.version() ) ) )
{}

InitSenderPlayerV8::InitSenderPlayerV8( const Params& params,
                                        const boost::shared_ptr< InitSenderCommon > common )
    : InitSenderPlayerV7( params, common )
{}

InitSenderPlayerV8::~InitSenderPlayerV8()
{}


/*!
//===================================================================
//
//  CLASS: InitSenderOnlineCoach
//
//  DESC: Base class for the init protocol for Online Coaches.
//
//===================================================================
*/

InitSenderOnlineCoach::Factory &
InitSenderOnlineCoach::factory()
{
    static Factory rval;
    return rval;
}


InitSenderOnlineCoach::InitSenderOnlineCoach( const Params& params,
                                              const boost::shared_ptr< InitSenderCommon > common )
    : InitSender( params.m_transport, common ),
      M_serializer( params.m_ser ),
      M_self( params.m_self ),
      M_stadium( params.m_stadium )
{

}

InitSenderOnlineCoach::~InitSenderOnlineCoach()
{

}

void
InitSenderOnlineCoach::sendPlayerClangVer()
{
    for ( Stadium::PlayerCont::const_iterator i = stadium().remotePlayers().begin();
          i != stadium().remotePlayers().end();
          ++i )
    {
        if ( (*i)->clangMinVer() != 0
             || (*i)->clangMaxVer() != 0 )
        {
            sendPlayerClangVer( **i );
        }
    }
}

/*!
//===================================================================
//
//  CLASS: InitSenderOnlineCoachV1
//
//  DESC: version 1 of the init protocol.
//
//===================================================================
*/


InitSenderOnlineCoachV1::InitSenderOnlineCoachV1( const Params& params )
    : InitSenderOnlineCoach( params,
                             boost::shared_ptr< rcss::InitSenderCommon >( new InitSenderCommonV1( params.m_transport,
                                                                                                  params.m_ser,
                                                                                                  params.m_stadium,
                                                                                                  (unsigned int)params.m_self.version() ) ) )
{}

InitSenderOnlineCoachV1::InitSenderOnlineCoachV1( const Params& params,
                                                  const boost::shared_ptr< InitSenderCommon > common )
    : InitSenderOnlineCoach( params, common )
{}

InitSenderOnlineCoachV1::~InitSenderOnlineCoachV1()
{}

void
InitSenderOnlineCoachV1::sendInit()
{
    serializer().serializeInit( transport() );
    transport() << std::ends << std::flush;
}

void
InitSenderOnlineCoachV1::sendReconnect()
{}

void
InitSenderOnlineCoachV1::sendChangedPlayers()
{}

void
InitSenderOnlineCoachV1::sendScore()
{}

/*!
//===================================================================
//
//  CLASS: InitSenderOnlineCoachV6
//
//  DESC: version 6 of the init protocol.  Coaches now get to know
//        what side they are on.
//
//===================================================================
*/


InitSenderOnlineCoachV6::InitSenderOnlineCoachV6( const Params& params )
    : InitSenderOnlineCoachV1( params )
{}

InitSenderOnlineCoachV6::InitSenderOnlineCoachV6( const Params& params,
                                                  const boost::shared_ptr< InitSenderCommon > common )
    : InitSenderOnlineCoachV1( params, common )
{}

InitSenderOnlineCoachV6::~InitSenderOnlineCoachV6()
{}

void
InitSenderOnlineCoachV6::sendInit()
{
    serializer().serializeInit( transport(), self().side() );
    transport() << std::ends << std::flush;
}

/*!
//===================================================================
//
//  CLASS: InitSenderOnlineCoachV7
//
//  DESC: version 7 of the init protocol.  Coaches now get server
//        parameters, player parameters and player types.
//
//===================================================================
*/


InitSenderOnlineCoachV7::InitSenderOnlineCoachV7( const Params& params )
    : InitSenderOnlineCoachV6( params,
                               boost::shared_ptr< rcss::InitSenderCommon >( new InitSenderCommonV7( params.m_transport,
                                                                                                    params.m_ser,
                                                                                                    params.m_stadium,
                                                                                                    (unsigned int)params.m_self.version() ) ) )
{}

InitSenderOnlineCoachV7::InitSenderOnlineCoachV7( const Params& params,
                                                  const boost::shared_ptr< InitSenderCommon > common )
    : InitSenderOnlineCoachV6( params, common )
{}

InitSenderOnlineCoachV7::~InitSenderOnlineCoachV7()
{}

void
InitSenderOnlineCoachV7::sendChangedPlayers()
{
    const Stadium::PlayerCont::const_iterator end = stadium().players().end();
    for ( Stadium::PlayerCont::const_iterator p = stadium().players().begin();
          p != end;
          ++p )
    {
        if ( (*p)->playerTypeId() == 0 ) continue;

        if ( self().side() == (*p)->team()->side() )
        {
            serializer().serializeChangedPlayer( transport(),
                                                 (*p)->unum(),
                                                 (*p)->playerTypeId() );
        }
        else
        {
            serializer().serializeChangedPlayer( transport(),
                                                 (*p)->unum() );
        }
        transport() << std::ends << std::flush;
    }

}

/*!
//===================================================================
//
//  CLASS: InitSenderOnlineCoachV8
//
//  DESC: version 8 of the init protocol.  Coaches now get server
//        parameters, player parameters and player types.
//
//===================================================================
*/


InitSenderOnlineCoachV8::InitSenderOnlineCoachV8( const Params& params )
    : InitSenderOnlineCoachV7( params,
                               boost::shared_ptr< rcss::InitSenderCommon >( new InitSenderCommonV8( params.m_transport,
                                                                                                    params.m_ser,
                                                                                                    params.m_stadium,
                                                                                                    (unsigned int)params.m_self.version() ) ) )
{

}

InitSenderOnlineCoachV8::InitSenderOnlineCoachV8( const Params& params,
                                                  const boost::shared_ptr< InitSenderCommon > common )
    : InitSenderOnlineCoachV7( params, common )
{

}

InitSenderOnlineCoachV8::~InitSenderOnlineCoachV8()
{

}

void
InitSenderOnlineCoachV8::sendPlayerClangVer( const Player & player )
{
    serializer().serializePlayerClangVer( transport(),
                                          player.shortName(),
                                          player.clangMinVer(),
                                          player.clangMaxVer() );
    transport() << std::ends << std::flush;
}


/*!
//===================================================================
//
//  CLASS: InitSenderOfflineCoach
//
//  DESC: Base class for the init protocol for Offline Coaches.
//
//===================================================================
*/

InitSenderOfflineCoach::Factory&
InitSenderOfflineCoach::factory()
{ static Factory rval; return rval; }


InitSenderOfflineCoach::InitSenderOfflineCoach( const Params& params,
                                                const boost::shared_ptr< InitSenderCommon > common )
    : InitSender( params.m_transport, common ),
      M_serializer( params.m_ser ),
      M_self( params.m_self ),
      M_stadium( params.m_stadium )
{}

InitSenderOfflineCoach::~InitSenderOfflineCoach()
{}



/*!
//===================================================================
//
//  CLASS: InitSenderOfflineCoachV1
//
//  DESC: version 1 of the init protocol.
//
//===================================================================
*/


InitSenderOfflineCoachV1::InitSenderOfflineCoachV1( const Params& params )
    : InitSenderOfflineCoach( params,
                              boost::shared_ptr< rcss::InitSenderCommon >( new InitSenderCommonV1( params.m_transport,
                                                                                                   params.m_ser,
                                                                                                   params.m_stadium,
                                                                                                   (unsigned int)params.m_self.version() ) ) )
{}

InitSenderOfflineCoachV1::InitSenderOfflineCoachV1( const Params& params,
                                                    const boost::shared_ptr< InitSenderCommon > common )
    : InitSenderOfflineCoach( params, common )
{}

InitSenderOfflineCoachV1::~InitSenderOfflineCoachV1()
{}

void
InitSenderOfflineCoachV1::sendInit()
{
    serializer().serializeInit( transport() );
    transport() << std::ends << std::flush;
}

void
InitSenderOfflineCoachV1::sendReconnect()
{}

void
InitSenderOfflineCoachV1::sendChangedPlayers()
{}

void
InitSenderOfflineCoachV1::sendScore()
{}

/*!
//===================================================================
//
//  CLASS: InitSenderOfflineCoachV7
//
//  DESC: version 7 of the init protocol.
//
//===================================================================
*/


InitSenderOfflineCoachV7::InitSenderOfflineCoachV7( const Params& params )
    : InitSenderOfflineCoachV1( params,
                                boost::shared_ptr< rcss::InitSenderCommon >( new InitSenderCommonV7( params.m_transport,
                                                                                                     params.m_ser,
                                                                                                     params.m_stadium,
                                                                                                     (unsigned int)params.m_self.version() ) ) )
{}

InitSenderOfflineCoachV7::InitSenderOfflineCoachV7( const Params& params,
                                                    const boost::shared_ptr< InitSenderCommon > common )
    : InitSenderOfflineCoachV1( params, common )
{}

InitSenderOfflineCoachV7::~InitSenderOfflineCoachV7()
{}

/*!
//===================================================================
//
//  CLASS: InitSenderOfflineCoachV8
//
//  DESC: version 8 of the init protocol.
//
//===================================================================
*/


InitSenderOfflineCoachV8::InitSenderOfflineCoachV8( const Params& params )
    : InitSenderOfflineCoachV7( params,
                                boost::shared_ptr< rcss::InitSenderCommon >( new InitSenderCommonV8( params.m_transport,
                                                                                                     params.m_ser,
                                                                                                     params.m_stadium,
                                                                                                     (unsigned int)params.m_self.version() ) ) )
{}

InitSenderOfflineCoachV8::InitSenderOfflineCoachV8( const Params& params,
                                                    const boost::shared_ptr< InitSenderCommon > common )
    : InitSenderOfflineCoachV7( params, common )
{}

InitSenderOfflineCoachV8::~InitSenderOfflineCoachV8()
{}


/*!
//===================================================================
//
//  Register senders for different versions
//
//===================================================================
*/

namespace initsender
{
template< typename Sender >
InitSenderPlayer::Ptr
create( const InitSenderPlayer::Params& params )
{ return InitSenderPlayer::Ptr( new Sender( params ) ); }

template< typename Sender >
InitSenderOnlineCoach::Ptr
create( const InitSenderOnlineCoach::Params& params )
{ return InitSenderOnlineCoach::Ptr( new Sender( params ) ); }

template< typename Sender >
InitSenderOfflineCoach::Ptr
create( const InitSenderOfflineCoach::Params& params )
{ return InitSenderOfflineCoach::Ptr( new Sender( params ) ); }


lib::RegHolder vp1 = InitSenderPlayer::factory().autoReg( &create< InitSenderPlayerV1 >, 1 );
lib::RegHolder vp2 = InitSenderPlayer::factory().autoReg( &create< InitSenderPlayerV1 >, 2 );
lib::RegHolder vp3 = InitSenderPlayer::factory().autoReg( &create< InitSenderPlayerV1 >, 3 );
lib::RegHolder vp4 = InitSenderPlayer::factory().autoReg( &create< InitSenderPlayerV1 >, 4 );
lib::RegHolder vp5 = InitSenderPlayer::factory().autoReg( &create< InitSenderPlayerV1 >, 5 );
lib::RegHolder vp6 = InitSenderPlayer::factory().autoReg( &create< InitSenderPlayerV1 >, 6 );
lib::RegHolder vp7 = InitSenderPlayer::factory().autoReg( &create< InitSenderPlayerV7 >, 7 );
lib::RegHolder vp8 = InitSenderPlayer::factory().autoReg( &create< InitSenderPlayerV8 >, 8 );
lib::RegHolder vp9 = InitSenderPlayer::factory().autoReg( &create< InitSenderPlayerV8 >, 9 );
lib::RegHolder vp10 = InitSenderPlayer::factory().autoReg( &create< InitSenderPlayerV8 >, 10 );
lib::RegHolder vp11 = InitSenderPlayer::factory().autoReg( &create< InitSenderPlayerV8 >, 11 );
lib::RegHolder vp12 = InitSenderPlayer::factory().autoReg( &create< InitSenderPlayerV8 >, 12 );

lib::RegHolder voc1 = InitSenderOnlineCoach::factory().autoReg( &create< InitSenderOnlineCoachV1 >, 1 );
lib::RegHolder voc2 = InitSenderOnlineCoach::factory().autoReg( &create< InitSenderOnlineCoachV1 >, 2 );
lib::RegHolder voc3 = InitSenderOnlineCoach::factory().autoReg( &create< InitSenderOnlineCoachV1 >, 3 );
lib::RegHolder voc4 = InitSenderOnlineCoach::factory().autoReg( &create< InitSenderOnlineCoachV1 >, 4 );
lib::RegHolder voc5 = InitSenderOnlineCoach::factory().autoReg( &create< InitSenderOnlineCoachV1 >, 5 );
lib::RegHolder voc6 = InitSenderOnlineCoach::factory().autoReg( &create< InitSenderOnlineCoachV6 >, 6 );
lib::RegHolder voc7 = InitSenderOnlineCoach::factory().autoReg( &create< InitSenderOnlineCoachV7 >, 7 );
lib::RegHolder voc8 = InitSenderOnlineCoach::factory().autoReg( &create< InitSenderOnlineCoachV8 >, 8 );
lib::RegHolder voc9 = InitSenderOnlineCoach::factory().autoReg( &create< InitSenderOnlineCoachV8 >, 9 );
lib::RegHolder voc10 = InitSenderOnlineCoach::factory().autoReg( &create< InitSenderOnlineCoachV8 >, 10 );
lib::RegHolder voc11 = InitSenderOnlineCoach::factory().autoReg( &create< InitSenderOnlineCoachV8 >, 11 );
lib::RegHolder voc12 = InitSenderOnlineCoach::factory().autoReg( &create< InitSenderOnlineCoachV8 >, 12 );

lib::RegHolder vc1 = InitSenderOfflineCoach::factory().autoReg( &create< InitSenderOfflineCoachV1 >, 1 );
lib::RegHolder vc2 = InitSenderOfflineCoach::factory().autoReg( &create< InitSenderOfflineCoachV1 >, 2 );
lib::RegHolder vc3 = InitSenderOfflineCoach::factory().autoReg( &create< InitSenderOfflineCoachV1 >, 3 );
lib::RegHolder vc4 = InitSenderOfflineCoach::factory().autoReg( &create< InitSenderOfflineCoachV1 >, 4 );
lib::RegHolder vc5 = InitSenderOfflineCoach::factory().autoReg( &create< InitSenderOfflineCoachV1 >, 5 );
lib::RegHolder vc6 = InitSenderOfflineCoach::factory().autoReg( &create< InitSenderOfflineCoachV1 >, 6 );
lib::RegHolder vc7 = InitSenderOfflineCoach::factory().autoReg( &create< InitSenderOfflineCoachV7 >, 7 );
lib::RegHolder vc8 = InitSenderOfflineCoach::factory().autoReg( &create< InitSenderOfflineCoachV8 >, 8 );
lib::RegHolder vc9 = InitSenderOfflineCoach::factory().autoReg( &create< InitSenderOfflineCoachV8 >, 9 );
lib::RegHolder vc10 = InitSenderOfflineCoach::factory().autoReg( &create< InitSenderOfflineCoachV8 >, 10 );
lib::RegHolder vc11 = InitSenderOfflineCoach::factory().autoReg( &create< InitSenderOfflineCoachV8 >, 11 );
lib::RegHolder vc12 = InitSenderOfflineCoach::factory().autoReg( &create< InitSenderOfflineCoachV8 >, 12 );
}
}
