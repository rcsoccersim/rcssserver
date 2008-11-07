// -*-c++-*-

/***************************************************************************
                               statushandler.hpp
                Interface for handling parse notifications
                             -------------------
    begin                : 16-JUN-2003
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

#ifndef RCSS_CONF_STATUSHANDLER_HPP
#define RCSS_CONF_STATUSHANDLER_HPP


#include <string>
#include <vector>
#include <boost/filesystem/path.hpp>

namespace rcss {
namespace conf {

class Builder;

class StatusHandler {
public:

    StatusHandler();

    virtual
    ~StatusHandler();

    /// called after a hander has been subscribed
    virtual
    void handlerAdded( Builder & publisher );


    /// called after a handler has been unsubscribed
    virtual
    void handlerRemoved( Builder & publisher );

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
};

}
}

#endif
