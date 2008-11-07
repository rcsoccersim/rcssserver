// -*-c++-*-

/***************************************************************************
                               statushandler.hpp
                Implementation for handling parse notifications
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

#include "statushandler.hpp"

namespace rcss {
namespace conf {

StatusHandler::StatusHandler()
{

}

StatusHandler::~StatusHandler()
{

}

/// called after a hander has been subscribed
void
StatusHandler::handlerAdded( Builder & )
{

}

/// called after a handler has been unsubscribed
void
StatusHandler::handlerRemoved( Builder & )
{

}

void
StatusHandler::parseError( const std::string &,
                           const std::string &,
                           const std::string &,
                           int )
{

}

void
StatusHandler::buildError( const std::string &,
                           const std::string &,
                           const std::string &,
                           const std::string &,
                           int )
{

}

void
StatusHandler::buildWarning( const std::string &,
                             const std::string &,
                             const std::string &,
                             const std::string &,
                             int )
{

}

void
StatusHandler::creatingConfFile( const std::string & )
{

}

void
StatusHandler::createdConfFile( const std::string & )
{

}

void
StatusHandler::confCreationFailed( const std::string &,
                                   int )
{

}

void
StatusHandler::includeFailed( const std::string &,
                              const std::string &,
                              const std::string &,
                              int )
{

}

void
StatusHandler::loadFailed( const std::string &,
                           const std::string &,
                           const std::vector< boost::filesystem::path > &,
                           const std::string &,
                           int )
{

}

}
}
