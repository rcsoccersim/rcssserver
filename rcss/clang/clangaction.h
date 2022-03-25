// -*-c++-*-

/***************************************************************************
                                clangaction.h
                       Abstract base class for CLang Actions
                             -------------------
    begin                : 28-MAY-2002
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

#ifndef RCSS_CLANGACTION_H
#define RCSS_CLANGACTION_H

#include <rcss/clang/region.h>
#include <rcss/clang/clangutil.h>

#include <memory>

namespace rcss {
namespace clang {

class ActPos;
class ActHome;
class ActBallToReg;
class ActBallToPlayer;
class ActMark;
class ActMarkLinePlayer;
class ActMarkLineReg;
class ActOffsidesLine;
class ActHetType;
class ActNamed;
class ActPassReg;
class ActPassUNum;
class ActDribble;
class ActClear;
class ActShoot;
class ActHold;
class ActIntercept;
class ActTackle;

class Action {
public:
    //       typedef util::Visitor20< ActPos*,
    //         ActHome*,
    //         ActBallToReg*,
    //         ActBallToPlayer*,
    //         ActMark*,
    //         ActMarkLinePlayer*,
    //         ActMarkLineReg*,
    //         ActOffsidesLine*,
    //         ActHetType*,
    //         ActNamed*,
    //         ActPassReg*,
    //         ActPassUNum*,
    //         ActDribble*,
    //         ActClear*,
    //         ActShoot*,
    //         ActHold*,
    //         ActIntercept*,
    //         ActTackle* > Visitor;

    //       typedef util::Visitor20< const ActPos*,
    //         const ActHome*,
    //         const ActBallToReg*,
    //         const ActBallToPlayer*,
    //         const ActMark*,
    //         const ActMarkLinePlayer*,
    //         const ActMarkLineReg*,
    //         const ActOffsidesLine*,
    //         const ActHetType*,
    //         const ActNamed*,
    //         const ActPassReg*,
    //         const ActPassUNum*,
    //         const ActDribble*,
    //         const ActClear*,
    //         const ActShoot*,
    //         const ActHold*,
    //         const ActIntercept*,
    //         const ActTackle* > ConstVisitor;

    //       class TypeExtractor
    //         : public util::TypeExtractor20< ActPos*,
    //         ActHome*,
    //         ActBallToReg*,
    //         ActBallToPlayer*,
    //         ActMark*,
    //         ActMarkLinePlayer*,
    //         ActMarkLineReg*,
    //         ActOffsidesLine*,
    //         ActHetType*,
    //         ActNamed*,
    //         ActPassReg*,
    //         ActPassUNum*,
    //         ActDribble*,
    //         ActClear*,
    //         ActShoot*,
    //         ActHold*,
    //         ActIntercept*,
    //         ActTackle* >
    //       {
    //       protected:
    //           virtual
    //           ~TypeExtractor()
    //           {}

    //         virtual
    //         void
    //         visit( ActPos* act );

    //         virtual
    //         void
    //         visit( ActHome* act );

    //         virtual
    //         void
    //         visit( ActBallToReg* act );

    //         virtual
    //         void
    //         visit( ActBallToPlayer* act );

    //         virtual
    //         void
    //         visit( ActMark* act );

    //         virtual
    //         void
    //         visit( ActMarkLinePlayer* act );

    //         virtual
    //         void
    //         visit( ActMarkLineReg* act );

    //         virtual
    //         void
    //         visit( ActOffsidesLine* act );

    //         virtual
    //         void
    //         visit( ActHetType* act );

    //         virtual
    //         void
    //         visit( ActNamed* act );

    //         virtual
    //         void
    //         visit( ActPassReg* act );

    //         virtual
    //         void
    //         visit( ActPassUNum* act );

    //         virtual
    //         void
    //         visit( ActDribble* act );

    //         virtual
    //         void
    //         visit( ActClear* act );

    //         virtual
    //         void
    //         visit( ActShoot* act );

    //         virtual
    //         void
    //         visit( ActHold* act );

    //         virtual
    //         void
    //         visit( ActIntercept* act );

    //         virtual
    //         void
    //         visit( ActTackle* act );
    //       };

    //       class ConstTypeExtractor
    //         : public util::TypeExtractor20< const ActPos*,
    //         const ActHome*,
    //         const ActBallToReg*,
    //         const ActBallToPlayer*,
    //         const ActMark*,
    //         const ActMarkLinePlayer*,
    //         const ActMarkLineReg*,
    //         const ActOffsidesLine*,
    //         const ActHetType*,
    //         const ActNamed*,
    //         const ActPassReg*,
    //         const ActPassUNum*,
    //         const ActDribble*,
    //         const ActClear*,
    //         const ActShoot*,
    //         const ActHold*,
    //         const ActIntercept*,
    //         const ActTackle* >
    //       {
    //       protected:
    //           virtual
    //           ~ConstTypeExtractor()
    //           {}

    //         virtual
    //         void
    //         visit( const ActPos* act );

    //         virtual
    //         void
    //         visit( const ActHome* act );

    //         virtual
    //         void
    //         visit( const ActBallToReg* act );

    //         virtual
    //         void
    //         visit( const ActBallToPlayer* act );

    //         virtual
    //         void
    //         visit( const ActMark* act );

    //         virtual
    //         void
    //         visit( const ActMarkLinePlayer* act );

    //         virtual
    //         void
    //         visit( const ActMarkLineReg* act );

    //         virtual
    //         void
    //         visit( const ActOffsidesLine* act );

    //         virtual
    //         void
    //         visit( const ActHetType* act );

    //         virtual
    //         void
    //         visit( const ActNamed* act );

    //         virtual
    //         void
    //         visit( const ActPassReg* act );

    //         virtual
    //         void
    //         visit( const ActPassUNum* act );

    //         virtual
    //         void
    //         visit( const ActDribble* act );

    //         virtual
    //         void
    //         visit( const ActClear* act );

    //         virtual
    //         void
    //         visit( const ActShoot* act );

    //         virtual
    //         void
    //         visit( const ActHold* act );

    //         virtual
    //         void
    //         visit( const ActIntercept* act );

    //         virtual
    //         void
    //         visit( const ActTackle* act );
    //       };

    virtual
    ~Action();

    virtual
    std::shared_ptr< Action > deepCopy() const = 0;

    //       virtual
    //       void
    //       accept( Visitor& v ) = 0;

    //       virtual
    //       void
    //       accept( ConstVisitor& v ) const = 0;

    virtual
    std::ostream & print( std::ostream & out ) const = 0;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const = 0;
}; // end Action


class ActPos
    : public Action {
public:
    ActPos();
    ActPos( std::shared_ptr< Region > r );
    ~ActPos() override;

    virtual
    std::shared_ptr< Action > deepCopy() const override;

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

    const Region * getRegion() const;

private:
	std::shared_ptr< Region > m_reg;
};


class ActHome
    : public Action {
public:
    ActHome();
    ActHome( std::shared_ptr< Region > r );
    ~ActHome() override;

    virtual
    std::shared_ptr< Action > deepCopy() const override;

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

    const Region * getRegion() const;

private:
	std::shared_ptr< Region > m_reg;
};


class ActBallToReg
    : public Action {

public:
    ActBallToReg();
    ActBallToReg( std::shared_ptr< Region > reg,
                  const BallMove & bmtset );
    ~ActBallToReg() override;

    virtual
    std::shared_ptr< Action > deepCopy() const override;

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

    const Region * getRegion() const;
    BallMove getBallMove() const;
    void addBMT( BallMoveToken t );
    void removeBMT( BallMoveToken t );
    void clearBMTSet();

private:
	std::shared_ptr< Region > m_reg;
	BallMove m_bmtset;
};

class ActBallToPlayer
    : public Action {
public:
    ActBallToPlayer();
    ActBallToPlayer( const UNumSet & players );
    ~ActBallToPlayer() override;

    virtual
    std::shared_ptr< Action > deepCopy() const override;

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

    UNumSet & getPlayers();
    const UNumSet & getPlayers() const;
    void setPlayers( const UNumSet & players );
    void addPlayer( const UNum & u );
    void clearPlayers();

private:
    UNumSet M_players;
};


class ActMark
    : public Action {
public:
    ActMark();
    ActMark( const UNumSet & players );
    ~ActMark() override;

    virtual
    std::shared_ptr< Action > deepCopy() const override;

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

    UNumSet & getPlayers();
    const UNumSet & getPlayers() const;
    void setPlayers( const UNumSet & players );
    void addPlayer( const UNum & i );
    void clearPlayers();

private:
    UNumSet M_players;
};


class ActMarkLinePlayer
    : public Action {
public:
    ActMarkLinePlayer();
    ActMarkLinePlayer( const UNumSet & players );
    ~ActMarkLinePlayer() override;

    virtual
    std::shared_ptr< Action > deepCopy() const override;

    virtual
    std::ostream & print( std::ostream & out ) const override;
    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

    UNumSet & getPlayers();
    const UNumSet & getPlayers() const;
    void setPlayers( const UNumSet & players );
    void addPlayer( const UNum & i );
    void clearPlayers();

private:
    UNumSet M_players;
};


class ActMarkLineReg
    : public Action {
public:

    ActMarkLineReg();
    ActMarkLineReg( std::shared_ptr< Region > reg );
    ~ActMarkLineReg() override;

    virtual
    std::shared_ptr< Action > deepCopy() const override;

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

    const Region * getRegion() const;
    std::shared_ptr< Region > detachRegion();

private:
	std::shared_ptr< Region > m_reg;
};


class ActOffsidesLine
    : public Action {
public:

    ActOffsidesLine();
    ActOffsidesLine( std::shared_ptr< Region > r );
    ~ActOffsidesLine() override;

	virtual
	std::shared_ptr< Action > deepCopy() const override;

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

    const Region * getRegion() const;
    std::shared_ptr< Region > detachRegion();

private:
	std::shared_ptr< Region > m_reg;
};


class ActHetType
    : public Action {
public:

    ActHetType();
    ActHetType( const int & player_type );
    ~ActHetType() override;

    virtual
    std::shared_ptr< Action > deepCopy() const override;

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

    /* class specific stuff */
    int getPlayerType() const;
    void setPlayerType( const int player_type );

private:
    int M_player_type;
};


class ActNamed
    : public Action {
public:
    ActNamed( const std::string & name = "" );
    ~ActNamed() override;

    virtual
    std::shared_ptr< Action > deepCopy() const override;

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

    /* class specific */
    std::string & getName();
    const std::string & getName() const;
    void setName( const std::string & name );

private:
    std::string M_name;
};


class ActPassReg
    : public Action {
public:
    ActPassReg();
    ActPassReg( std::shared_ptr< Region > reg );
    ~ActPassReg() override;

    virtual
    std::shared_ptr< Action > deepCopy() const override;

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

    const Region * getRegion() const;
    std::shared_ptr< Region > detachRegion();

private:
	std::shared_ptr< Region > m_reg;
};


class ActPassUNum
    : public Action {
public:
    ActPassUNum();
    ActPassUNum( const UNumSet & players );
    ~ActPassUNum() override;

    virtual
    std::shared_ptr< Action > deepCopy() const override;

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

    /* class specific stuff */
    UNumSet & getPlayers();
    const UNumSet & getPlayers() const;
    void add( const UNum & unum );
    void clearPlayers();

private:
	UNumSet m_players;
};


class ActDribble
    : public Action {
public:
    ActDribble();
    ActDribble( std::shared_ptr< Region > reg );
    ~ActDribble() override;

    virtual
    std::shared_ptr< Action > deepCopy() const override;

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

    const Region * getRegion() const;
    std::shared_ptr< Region > detachRegion();

private:
	std::shared_ptr< Region > m_reg;
};


class ActClear
    : public Action {
public:
    ActClear();
    ActClear( std::shared_ptr< Region > reg );
    ~ActClear() override;

    virtual
    std::shared_ptr< Action > deepCopy() const override;

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

    const Region * getRegion() const;
    std::shared_ptr< Region > detachRegion();

private:
	std::shared_ptr< Region > m_reg;
};


class ActShoot
    : public Action {
public:
    ActShoot();
    ~ActShoot() override;

    virtual
    std::shared_ptr< Action > deepCopy() const override;

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

};

class ActHold
    : public Action {
public:
    ActHold();
    ~ActHold() override;

    virtual
    std::shared_ptr< Action > deepCopy() const override;

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;
};


class ActIntercept
    : public Action {
public:
    ActIntercept();
    ~ActIntercept() override;

    virtual
    std::shared_ptr< Action > deepCopy() const override;

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;
};


class ActTackle
    : public Action {
public:
    ActTackle();
    ActTackle( const UNumSet & players );
    ~ActTackle() override;

    virtual
    std::shared_ptr< Action > deepCopy() const override;

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

    /* class specific stuff */
    UNumSet & getPlayers();
    const UNumSet & getPlayers() const;
    void setPlayers( const UNumSet & players );
    void addPlayer( const UNum & i );
    void clearPlayers();

private:
    UNumSet M_players;
};

std::ostream &
operator<<( std::ostream & os,
            const Action & a );

}
}

#endif
