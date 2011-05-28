// -*-c++-*-

/***************************************************************************
                               clangbuilder.h
                      Interface for building a clang message
                             -------------------
    begin                : 16-FEB-2002
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

#ifndef RCSSSERVER_CLANGBUILDER_H
#define RCSSSERVER_CLANGBUILDER_H


#include <string>
#include <exception>

#include "clangutil.h"


// The clang::Builder is called from within the parser to construct the
// CLang message.  It is not the job of the builder to make sure the
// grammar meets to Clang grammar rules, that's the job of the parser,
// however the Builder should do some error detection to make sure
// that if for some reason the methods are called out of context, the
// builder doesn't cause the whole program to crash.
// By using a builder, complexity is removed from the parser itself,
// making it easier to focus on the grammar without distraction.  This
// also allows the behaviour of the parser to be easily modified or
// extended by simple modifying or extending the builder.


namespace rcss {

namespace util {
class ArithOp;
class CompOp;
}

namespace clang {

class Builder {
public:
    Builder()
      { }

    virtual
    ~Builder()
      { }

    inline
    void setVer( const unsigned int & ver )
      {
          setVer( ver, ver );
      }

    virtual
    void setVer( const unsigned int & min,
                 const unsigned int & max ) = 0;


    virtual
    void setStrVarSize( const unsigned int len ) = 0;

    virtual
    unsigned int strVarSize() const = 0;

    virtual
    void setFreeformMsgSize( const unsigned int len ) = 0;

    virtual
    unsigned int freeformMsgSize() const = 0;

    virtual
    void setTime( const int & time ) = 0;

    virtual
    void setSide( const int & side ) = 0;

    virtual
    void setTimeRecv( const int & time ) = 0;

    virtual
    void buildMetaMsg() = 0;

    virtual
    void buildMetaTokenVer( const double & ver ) = 0;

    virtual
    void buildDefineMsg() = 0;

    virtual
    void buildDefineCond( const std::string & name ) = 0;

    virtual
    void buildDefineDir( const std::string & name ) = 0;

    virtual
    void buildDefineReg( const std::string & name ) = 0;

    virtual
    void buildDefineAct( const std::string & name ) = 0;

    virtual
    void buildFreeformMsg( const std::string & str ) = 0;

    virtual
    void buildUnsuppMsg() = 0;

    virtual
    void buildInfoMsg() = 0;

    virtual
    void buildAdviceMsg() = 0;

    virtual
    void buildTokenRule( const int & ttl ) = 0;

    virtual
    void buildTokenClear() = 0;

    virtual
    void buildActPos() = 0;

    virtual
    void buildActHome() = 0;

    virtual
    void buildActBallToReg() = 0;

    virtual
    void buildActBallToPlayer() = 0;

    virtual
    void buildActMark() = 0;

    virtual
    void buildActMarkLinePlayer() = 0;

    virtual
    void buildActMarkLineReg() = 0;

    virtual
    void buildActOffsideLine() = 0;

    virtual
    void buildActHetType( const int & type ) = 0;

    virtual
    void buildActNamed( const std::string & name ) = 0;

    virtual
    void buildActPassReg() = 0;

    virtual
    void buildActPassUNum() = 0;

    virtual
    void buildActDribble() = 0;

    virtual
    void buildActClear() = 0;

    virtual
    void buildActShoot() = 0;

    virtual
    void buildActHold() = 0;

    virtual
    void buildActIntercept() = 0;

    virtual
    void buildActTackle() = 0;

    virtual
    void buildDirComm( const bool & do_dont,
                       const bool & our_side ) = 0;

    virtual
    void buildDirNamed( const std::string & name ) = 0;

    virtual
    void buildCondTrue() = 0;

    virtual
    void buildCondFalse() = 0;

    virtual
    void buildCondPlayerPos( const bool & our_side,
                             const int & min,
                             const int & max ) = 0;

    virtual
    void buildCondBallPos() = 0;

    virtual
    void buildCondBallOwner( const bool & our_side ) = 0;

    virtual
    void buildCondPlayMode( const PlayMode & play_mode ) = 0;

    virtual
    void buildCondAnd() = 0;

    virtual
    void buildCondOr() = 0;

    virtual
    void buildCondNot() = 0;

    virtual
    void buildCondNamed( const std::string & name ) = 0;

    virtual
    void buildCondTime( const int & time,
                        const util::CompOp & comp ) = 0;

    virtual
    void buildCondOppGoal( const int & goals,
                           const util::CompOp & comp ) = 0;

    virtual
    void buildCondOurGoal( const int & goals,
                           const util::CompOp & comp ) = 0;

    virtual
    void buildCondGoalDiff( const int & goals,
                            const util::CompOp & comp ) = 0;

    virtual
    void buildCondUNum( const rcss::clang::UNum & unum ) = 0;

    virtual
    void buildAddToCondList() = 0;

    virtual
    void buildCreateCondList() = 0;

    virtual
    void buildRegNull() = 0;

    virtual
    void buildRegQuad() = 0;

    virtual
    void buildRegArc( const double & start_rad,
                      const double & end_rad,
                      const double & start_ang,
                      const double & span_ang ) = 0;

    virtual
    void buildRegUnion() = 0;

    virtual
    void buildRegNamed( const std::string & name ) = 0;

    virtual
    void buildRegPoint() = 0;

    virtual
    void buildRegTri() = 0;

    virtual
    void buildRegRec() = 0;

    virtual
    void buildPointSimple( const double & x,
                           const double & y ) = 0;

    virtual
    void buildPointRel( const double & x,
                        const double & y ) = 0;

    virtual
    void buildPointBall() = 0;

    virtual
    void buildPointPlayer( const bool & our_side,
                           const UNum & unum ) = 0;

    virtual
    void buildPointArith( const rcss::util::ArithOp & arith_op ) = 0;

    virtual
    void buildUNum( const UNum & unum ) = 0;

    virtual
    void buildUNumSet() = 0;

    virtual
    void buildBallMoveToken( const BallMoveToken & bmt ) = 0;

    virtual
    void buildRuleMsg() = 0;

    virtual
    void buildActivateAllRules( const bool & on ) = 0;

    virtual
    void buildActivateRules( const bool & on ) = 0;

    virtual
    void buildRuleID( const std::string & id ) = 0;

    virtual
    void buildRuleIDList() = 0;

    virtual
    void buildRuleIDListALL() = 0;

    virtual
    void buildDelMsg() = 0;

    virtual
    void buildDefineModelRule( const std::string & id ) = 0;

    virtual
    void buildDefineDirectiveRule( const std::string & id ) = 0;

    virtual
    void buildSimpleRule() = 0;

    virtual
    void buildNestedRule() = 0;

    virtual
    void buildIDRule() = 0;

};


class BuilderErr
    : public std::exception {
protected:
    std::string M_msg;
public:
    BuilderErr( const char * file,
                const int & line,
                const char * msg ) throw();

    ~BuilderErr() throw()
      { }

    const char * what() const throw()
      {
          return M_msg.c_str();
      }

};

}
}

#endif
