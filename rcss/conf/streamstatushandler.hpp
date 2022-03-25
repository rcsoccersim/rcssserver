// -*-c++-*-

/***************************************************************************
                         streamstatushandler.hpp
                Interface for writing parser notifactions to streams
                             -------------------
    begin                : 14-MAY-2003
    copyright            : (C) 2003 by The RoboCup Soccer Server
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

#ifndef STREAMSTATUSHANDLER_HPP
#define STREAMSTATUSHANDLER_HPP

#include "statushandler.hpp"
#include <iosfwd>

namespace rcss {
namespace conf {

class StreamStatusHandler
    : public StatusHandler {
public:

    StreamStatusHandler();

    StreamStatusHandler( std::ostream & errstrm );

    virtual
    ~StreamStatusHandler();

    virtual
    void parseError( const std::string & curr,
                     const std::string & err,
                     const std::string & name,
                     int lineno );

    virtual
    void buildError( const std::string & module,
                     const std::string & param,
                     const std::string & err,
                     const std::string & name,
                     int lineno );

    virtual
    void buildWarning( const std::string & module,
                       const std::string & param,
                       const std::string & warn,
                       const std::string & name,
                       int lineno );

    virtual
    void creatingConfFile( const std::string & conf_name );

    virtual
    void createdConfFile( const std::string & conf_name );

    virtual
    void confCreationFailed( const std::string & conf_name,
                        int error );

    virtual
    void includeFailed( const std::string & filename,
                        const std::string & error,
                        const std::string & name,
                        int lineno );

    virtual
    void loadFailed( const std::string & libname,
                     const std::string & error,
                     const std::vector< boost::filesystem::path > & avail,
                     const std::string & name,
                     int lineno );
private:
    std::ostream & M_errstrm;

};

}
}

#endif
