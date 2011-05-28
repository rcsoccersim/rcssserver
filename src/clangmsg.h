// -*-c++-*-

/***************************************************************************
                                 clangmsg.h
                    Abstract base class for CLang messages
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

#ifndef CLANGMSG_H
#define CLANGMSG_H

#include <memory>
#include <iostream>

namespace rcss {
namespace clang {

/********* Msg *******************/
class MetaMsg;
class FreeformMsg;
class InfoMsg;
class AdviceMsg;
class DefineMsg;
class UnsuppMsg;
class RuleMsg;
class DelMsg;

class Msg {

public:

    enum Types { META, FREEFORM, INFO, ADVICE, DEFINE, DEL, RULE, UNSUP };

protected:
    Msg();
    Msg( const int & time_recv );

public:
    virtual
    ~Msg();

    virtual
    Types getType() const = 0;

    virtual
    std::auto_ptr< Msg > deepCopy() const = 0;

    virtual
    std::ostream & print( std::ostream & out ) const = 0;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const = 0;

    void setVer( const unsigned int min,
                 const unsigned int max );

    unsigned int getMinVer() const;
    unsigned int getMaxVer() const;
    bool isSupported( const unsigned int min,
                      const unsigned int max ) const;
    int getTimeRecv() const;
    void setTimeRecv( const int time_recv );
    int getTimeSend() const;
    void setTimeSend( const int time_send );
    int getSide() const;
    void setSide( const int side );

private:
    int M_time_recv;
    unsigned int M_min_ver;
    unsigned int M_max_ver;

    int M_time_send;
    int M_side;
};

}
}


std::ostream &
operator<<( std::ostream & os,
            const rcss::clang::Msg & m );

#endif
