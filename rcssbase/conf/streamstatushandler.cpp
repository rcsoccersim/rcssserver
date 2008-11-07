// -*-c++-*-

/***************************************************************************
                         streamstatushandler.cpp
        Implementation for writing parser notifactions to streams
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

#include "streamstatushandler.hpp"

#include <iostream>
#include <cstring>

namespace rcss {
namespace conf {

StreamStatusHandler::StreamStatusHandler()
    : M_errstrm( std::cerr )
{

}

StreamStatusHandler::StreamStatusHandler( std::ostream & errstrm )
    : M_errstrm( errstrm )
{

}

StreamStatusHandler::~StreamStatusHandler()
{

}

void
StreamStatusHandler::parseError( const std::string & curr,
                                 const std::string & err,
                                 const std::string & name,
                                 int lineno )
{
    M_errstrm << name << ": "
              << lineno
              << ": error: while parsing \"" << curr << "\"\n";
    M_errstrm << name << ": "
              << lineno
              << ": error: " << err << std::endl;

}

void
StreamStatusHandler::buildError( const std::string & module_name,
                                 const std::string & param_name,
                                 const std::string & error,
                                 const std::string & name,
                                 int lineno )
{
    M_errstrm << name << ": "
              << lineno
              << ": error: while parsing \"" << module_name
              << "::" << param_name << "\"\n"
              << name << ": "
              << lineno
              << ": error: " << error << std::endl;
}

void
StreamStatusHandler::buildWarning( const std::string & module_name,
                                   const std::string & param_name,
                                   const std::string & warning,
                                   const std::string & name,
                                   int lineno )
{
    M_errstrm << name << ": "
              << lineno
              << ": warning: while parsing \"" << module_name
              << "::" << param_name << "\"\n"
              << name << ": " << lineno
              << ": warning: " << warning << std::endl;
}

void
StreamStatusHandler::creatingConfFile( const std::string & conf_name )
{
    M_errstrm << "Trying to create configuration file: "
              << conf_name << std::endl;
}

void
StreamStatusHandler::createdConfFile( const std::string & conf_name )
{
    M_errstrm << "Created configuration file: "
              << conf_name << std::endl;
}

void
StreamStatusHandler::confCreationFailed( const std::string & conf_name,
                                         int error )
{
    M_errstrm << conf_name
              << ": Configuration file creation failed: "
              << strerror( error ) << std::endl;
}

void
StreamStatusHandler::includeFailed( const std::string & filename,
                                    const std::string & error,
                                    const std::string & name,
                                    int lineno )
{
    M_errstrm << name << ": " << lineno
              << ": error: include '" << filename << "' failed\n";
    M_errstrm << name << ": " << lineno
              << ": error: " << error << std::endl;
}

void
StreamStatusHandler::loadFailed( const std::string & libname,
                                 const std::string & error,
                                 const std::vector< boost::filesystem::path > & avail,
                                 const std::string & name,
                                 int lineno )
{
    M_errstrm << name << ": " << lineno << ": error: loading '" << libname << "' failed\n";
    M_errstrm << name << ": " << lineno << ": error: " << error << std::endl;
    if ( avail.empty() )
    {
				M_errstrm << name << ": " << lineno << ": error: no available modules\n";
    }
    else
    {
				M_errstrm << name << ": " << lineno << ": error: available modules:\n";
				for( std::vector< boost::filesystem::path >::const_iterator i = avail.begin();
             i != avail.end(); ++i )
				{
            M_errstrm << "\t" << i->native_file_string() << std::endl;
				}
    }
}

}
}
