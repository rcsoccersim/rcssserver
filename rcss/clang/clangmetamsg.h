// -*-c++-*-

/***************************************************************************
                                clangmetamsg.h
                       Class for CLang Meta messages
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

#ifndef RCSS_CLANGMETAMSG_H
#define RCSS_CLANGMETAMSG_H

#include <rcss/clang/clangmsg.h>

#include <list>
#include <memory>

namespace rcss {
namespace clang {

class MetaTokenVer;

class MetaToken {
protected:
    MetaToken();

public:
    virtual
    ~MetaToken();

    virtual
    std::shared_ptr< MetaToken > deepCopy() const = 0;

    virtual
    std::ostream & print( std::ostream & out ) const = 0;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const = 0;
};



class MetaTokenVer
    : public MetaToken {
public:
    MetaTokenVer( const double& ver = 0.0 );

    virtual
    ~MetaTokenVer() override;

    virtual
    std::shared_ptr< MetaToken > deepCopy() const override;

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

//     double getVer() const
//       {
//           return M_ver;
//       }

private:
	double M_ver;
};



class MetaMsg
    : public Msg {
public:
    typedef std::list< std::shared_ptr< MetaToken > > Storage;

    MetaMsg();
private:
    MetaMsg( const Storage& tokens );
public:
    virtual
    ~MetaMsg() override;

    virtual
    std::shared_ptr< Msg > deepCopy() const override;

    virtual
    std::ostream & print( std::ostream & out ) const override;

    virtual
    std::ostream & printPretty( std::ostream & out,
                                const std::string & line_header ) const override;

    Storage & getTokens()
      {
          return M_tokens;
      }

    const Storage & getTokens() const
      {
          return M_tokens;
      }

	virtual
	Types getType() const
      {
          return META;
      }


private:
	Storage M_tokens;
};


inline
std::ostream &
operator<<( std::ostream & os,
            const rcss::clang::MetaToken & mt )
{
    return mt.print( os );
}

}
}

#endif
