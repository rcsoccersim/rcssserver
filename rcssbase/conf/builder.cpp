// -*-c++-*-

/***************************************************************************
                               builder.cpp
                Interface for building config parameter
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "builder.hpp"

#include "statushandler.hpp"

#ifdef HAVE_SSTREAM
#include <sstream>
#else
#include <strstream>
#endif

#include <iostream>
#include <iomanip>

namespace rcss {
namespace conf {


Builder::Builder( const std::string & progname,
                  const std::string & version,
                  const std::string & module_name )
    : m_err( false ),
      m_progname( progname ),
      m_version( version ),
      m_parent( NULL ),
      m_generic_help_requested( false ),
      m_detailed_help_requested( false ),
      m_module_name( module_name )
{}

Builder::Builder( Builder* parent,
                  const std::string & version,
                  const std::string & module_name )
    : m_err( false ),
      m_version( version ),
      m_parent( parent ),
      m_generic_help_requested( false ),
      m_detailed_help_requested( false ),
      m_module_name( module_name )
{
    m_parent->addChild( *this );
}

Builder::~Builder()
{
    if ( m_parent )
    {
        m_parent->removeChild( *this );
    }
}

void
Builder::createConfFile( std::ostream & conf,
                         const std::string & module_name )
{
    if ( module_name == getModuleName() )
    {
        doCreateConfFile( conf );
    }
    else
    {
        for ( std::list< Builder * >::iterator i = m_children.begin();
              i != m_children.end();
              ++i )
        {
            (*i)->createConfFile( conf, module_name );
        }
    }
}

void
Builder::addHandler( StatusHandler & handler )
{
    m_handlers.push_back( &handler );
    handler.handlerAdded( *this );
}

void
Builder::removeHandler( StatusHandler & handler )
{
    m_handlers.erase( std::find( m_handlers.begin(), m_handlers.end(), &handler ) );
    handler.handlerRemoved( *this );
}

void
Builder::addChild( Builder & child )
{
    m_children.push_back( &child );
    if ( m_parser )
    {
        child.addedToParser( *m_parser );
    }
}

void
Builder::removeChild( Builder & child )
{
    m_children.erase( std::find( m_children.begin(), m_children.end(), &child ) );
    child.removedFromParser();
}


void
Builder::parseError( const std::string & curr,
                     const std::string & err,
                     const std::string & name,
                     int lineno )
{
    requestGenericHelp();
    m_err = true;

    for ( std::list< StatusHandler * >::iterator i = m_handlers.begin();
          i != m_handlers.end();
          ++i )
    {
        (*i)->parseError( curr, err, name, lineno );
    }

    for ( std::list< Builder * >::iterator i = m_children.begin();
          i != m_children.end();
          ++i )
    {
        (*i)->parseError( curr, err, name, lineno );
    }
}

void
Builder::buildError( const std::string & module,
                     const std::string & param,
                     const std::string & err,
                     const std::string & name,
                     int lineno )
{
    requestDetailedHelp();
    m_err = true;

    for ( std::list< StatusHandler * >::iterator i = m_handlers.begin();
          i != m_handlers.end();
          ++i )
    {
        (*i)->buildError( module, param, err, name, lineno );
    }

    for ( std::list< Builder * >::iterator i = m_children.begin();
          i != m_children.end();
          ++i )
    {
        (*i)->buildError( module, param, err, name, lineno );
    }
}

void
Builder::buildWarning( const std::string & module,
                       const std::string & param,
                       const std::string & warn,
                       const std::string & name,
                       int lineno )
{
    for ( std::list< StatusHandler * >::iterator i = m_handlers.begin();
          i != m_handlers.end();
          ++i )
    {
        (*i)->buildWarning( module, param, warn, name, lineno );
    }

    for ( std::list< Builder * >::iterator i = m_children.begin();
          i != m_children.end();
          ++i )
    {
        (*i)->buildWarning( module, param, warn, name, lineno );
    }
}

void
Builder::creatingConfFile( const std::string & conf_name )
{
    for ( std::list< StatusHandler * >::iterator i = m_handlers.begin();
          i != m_handlers.end();
          ++i )
    {
        (*i)->creatingConfFile( conf_name );
    }

    for ( std::list< Builder * >::iterator i = m_children.begin();
          i != m_children.end();
          ++i )
    {
        (*i)->creatingConfFile( conf_name );
    }
}

void
Builder::createdConfFile( const std::string & conf_name )
{
    for ( std::list< StatusHandler * >::iterator i = m_handlers.begin();
          i != m_handlers.end();
          ++i )
    {
        (*i)->createdConfFile( conf_name );
    }

    for ( std::list< Builder * >::iterator i = m_children.begin();
          i != m_children.end();
          ++i )
    {
        (*i)->createdConfFile( conf_name );
    }
}

void
Builder::confCreationFailed( const std::string & conf_name,
                             int error )
{
    for( std::list< StatusHandler * >::iterator i = m_handlers.begin();
         i != m_handlers.end();
         ++i )
    {
        (*i)->confCreationFailed( conf_name, error );
    }

    for ( std::list< Builder * >::iterator i = m_children.begin();
          i != m_children.end();
          ++i )
    {
        (*i)->confCreationFailed( conf_name, error );
    }
}

void
Builder::includeFailed( const std::string & filename,
                        const std::string & error,
                        const std::string & name,
                        int lineno )
{
    m_err = true;

    for ( std::list< StatusHandler * >::iterator i = m_handlers.begin();
          i != m_handlers.end();
          ++i )
    {
        (*i)->includeFailed( filename, error, name, lineno );
    }

    for ( std::list< Builder * >::iterator i = m_children.begin();
          i != m_children.end();
          ++i )
    {
        (*i)->includeFailed( filename, error, name, lineno );
    }
}

bool
Builder::success() const
{
    return !m_err;
}

void
Builder::reset()
{
    m_err = false;
}

const std::string &
Builder::progName() const
{
    if ( m_parent )
    {
        return m_parent->progName();
    }
    else
    {
        return m_progname;
    }
}

bool
Builder::genericHelpRequested() const
{
    if ( m_parent )
    {
        return m_parent->genericHelpRequested();
    }
    else
    {
        return m_generic_help_requested;
    }
}

bool
Builder::detailedHelpRequested() const
{
    return m_detailed_help_requested;
}

void
Builder::requestGenericHelp()
{
    if ( m_parent )
    {
        m_parent->requestGenericHelp();
    }
    else
    {
        m_generic_help_requested = true;
    }
}

void
Builder::requestDetailedHelp( const std::string & module_name )
{
    for( std::list< Builder * >::iterator i = m_children.begin();
         i != m_children.end();
         ++i )
    {
        (*i)->requestDetailedHelp( module_name );
    }
    doRequestDetailedHelp( module_name );
}


void
Builder::doRequestDetailedHelp( const std::string & module_name )
{
    if ( module_name == m_module_name )
    {
        Builder::requestDetailedHelp();
    }
}


void
Builder::addedToParser( Parser & p )
{
    m_parser = &p;
    for( std::list< Builder * >::iterator i = m_children.begin();
         i != m_children.end();
         ++i )
    {
        (*i)->addedToParser( p );
    }
}

void
Builder::removedFromParser()
{
    m_parser = NULL;
    m_parent = NULL;
    for( std::list< Builder * >::iterator i = m_children.begin();
         i != m_children.end();
         ++i )
    {
        (*i)->removedFromParser();
    }
}

Parser*
Builder::parser()
{
    return m_parser;
}

void
Builder::requestDetailedHelp()
{
    requestGenericHelp();
    m_detailed_help_requested = true;
}

void
Builder::displayHelp()
{
    if ( genericHelpRequested() )
    {
        if ( m_parent == NULL )
        {
            displayUsage( m_progname );
        }

        displayGenericHelp();

        for ( std::list< Builder * >::iterator i = m_children.begin();
              i != m_children.end();
              ++i )
        {
            (*i)->displayGenericHelp();
        }

        if ( detailedHelpRequested() )
        {
            displayDetailedHelp();
        }

        for ( std::list< Builder * >::iterator i = m_children.begin();
              i != m_children.end();
              ++i )
        {
            if ( (*i)->detailedHelpRequested() )
            {
                (*i)->displayDetailedHelp();
            }
        }
    }
}

const
std::string &
Builder::getModuleName() const
{
    return m_module_name;
}

const
std::string &
Builder::version() const
{
    return m_version;
}

const
std::string &
Builder::parsedVersion() const
{
    return m_parsed_version;
}



void
Builder::displayUsage( const std::string & progname )
{
    std::cout << "\nUsage: " << progname << " [[-[-]]namespace::option=value]\n"
              <<   "       " << std::setfill(' ')  << std::setw( progname.length() ) <<  ' '
              << " [[-[-]][namespace::]help]\n"
              <<   "       " << std::setfill(' ')  << std::setw( progname.length() ) <<  ' '
              << " [[-[-]]include=file]\n"
//               <<   "       " << std::setfill(' ')  << std::setw( progname.length() ) <<  ' '
//               << " [[-[-]]load=library]\n"
//               <<   "       " << std::setfill(' ')  << std::setw( progname.length() ) <<  ' '
//               << " [[-[-]]setpath=path]\n"
//               <<   "       " << std::setfill(' ')  << std::setw( progname.length() ) <<  ' '
//               << " [[-[-]]addpath=path] ...\n";
        ;
    std::cout << "Options:\n";
    displayHelpEntry( std::cout,
                      "help",
                      "display generic help" );
    displayHelpEntry( std::cout,
                      "include=file",
                      "parse the specified configuration file."
                      "  Configuration files have the same format as the command line options."
                      " The configuration file specified will be parsed before all subsequent options.");
}

void
Builder::displayGenericHelp()
{
    displayHelpEntry( std::cout,
                      m_module_name,
                      "help",
                      "display detailed help for the \"" + m_module_name + "\" module" );
}

void
Builder::displayDetailedHelp()
{
    std::cout << m_module_name << " Options:\n";

    std::ostream & conf = std::cout;
    for ( IntMap::iterator iter = m_ints.begin();
          iter != m_ints.end();
          ++iter )
    {
        displayHelpEntry( conf,
                          m_module_name,
                          iter->first,
                          iter->second.get(),
                          iter->second.desc() );
    }

    for ( BoolMap::iterator iter = m_bools.begin();
          iter != m_bools.end();
          ++iter )
    {
        displayHelpEntry( conf,
                          m_module_name,
                          iter->first,
                          iter->second.get(),
                          iter->second.desc() );
    }

    for ( DoubMap::iterator iter = m_doubs.begin();
          iter != m_doubs.end();
          ++iter )
    {
        displayHelpEntry( conf,
                          m_module_name,
                          iter->first,
                          iter->second.get(),
                          iter->second.desc() );
    }

    for ( StrMap::iterator iter = m_strs.begin();
          iter != m_strs.end();
          ++iter )
    {
        displayHelpEntry( conf,
                          m_module_name,
                          iter->first,
                          iter->second.get(),
                          iter->second.desc() );
    }
}

bool
Builder::doBuildParam( const std::string & module_name,
                       const std::string & param_name,
                       int value,
                       const std::string & name,
                       int lineno )
{
    if ( module_name == getModuleName() )
    {
        return doBuildParam( param_name, value, name, lineno );
    }
    return true;
}


bool
Builder::doBuildParam( const std::string & module_name,
                       const std::string & param_name,
                       bool value,
                       const std::string & name,
                       int lineno )
{
    if ( module_name == getModuleName() )
    {
        return doBuildParam( param_name, value, name, lineno );
    }
    return true;
}

bool
Builder::doBuildParam( const std::string & module_name,
                       const std::string & param_name,
                       double value,
                       const std::string & name,
                       int lineno )
{
    if ( module_name == getModuleName() )
    {
        return doBuildParam( param_name, value, name, lineno );
    }
    return true;
}

bool
Builder::doBuildParam( const std::string & module_name,
                       const std::string & param_name,
                       const std::string & value,
                       const std::string & name,
                       int lineno )
{
    if ( module_name == getModuleName() )
    {
        return doBuildParam( param_name, value, name, lineno );
    }
    return true;
}

bool
Builder::doBuildParam( const std::string & param_name,
                       int value,
                       const std::string & name,
                       int lineno )
{
    IntMap::iterator int_iter = m_ints.find( param_name );
    if ( int_iter != m_ints.end() )
    {
        return setParam( param_name, int_iter->second, value, name, lineno );
    }

    BoolMap::iterator bool_iter = m_bools.find( param_name );
    if ( bool_iter != m_bools.end() )
    {
        return setParam( param_name, bool_iter->second, value, name, lineno );
    }

    DoubMap::iterator doub_iter = m_doubs.find( param_name );
    if ( doub_iter != m_doubs.end() )
    {
        return setParam( param_name, doub_iter->second, value, name, lineno );
    }

    StrMap::iterator str_iter = m_strs.find( param_name );
    if ( str_iter != m_strs.end() )
    {
        return setParam( param_name, str_iter->second, value, name, lineno );
    }

    buildError( m_module_name,
                param_name,
                "'" + m_module_name
                + "' has no parameter called '"
                + param_name + "'",
                name,
                lineno );
    return false;
}

bool
Builder::doBuildParam( const std::string & param_name, bool value,
                       const std::string & name,
                       int lineno )
{
    IntMap::iterator int_iter = m_ints.find( param_name );
    if ( int_iter != m_ints.end() )
    {
        return setParam( param_name, int_iter->second, value, name, lineno );
    }

    BoolMap::iterator bool_iter = m_bools.find( param_name );
    if ( bool_iter != m_bools.end() )
    {
        return setParam( param_name, bool_iter->second, value, name, lineno );
    }

    DoubMap::iterator doub_iter = m_doubs.find( param_name );
    if ( doub_iter != m_doubs.end() )
    {
        return setParam( param_name, doub_iter->second, value, name, lineno );
    }

    StrMap::iterator str_iter = m_strs.find( param_name );
    if ( str_iter != m_strs.end() )
    {
        return setParam( param_name, str_iter->second, value, name, lineno );
    }

    buildError( m_module_name,
                param_name,
                "'" + m_module_name
                + "' has no parameter called '"
                + param_name + "'",
                name,
                lineno );
    return false;
}

bool
Builder::doBuildParam( const std::string & param_name, double value,
                       const std::string & name,
                       int lineno )
{
    IntMap::iterator int_iter = m_ints.find( param_name );
    if ( int_iter != m_ints.end() )
    {
        return setParam( param_name, int_iter->second, value, name, lineno );
    }

    BoolMap::iterator bool_iter = m_bools.find( param_name );
    if ( bool_iter != m_bools.end() )
    {
        return setParam( param_name, bool_iter->second, value, name, lineno );
    }

    DoubMap::iterator doub_iter = m_doubs.find( param_name );
    if ( doub_iter != m_doubs.end() )
    {
        return setParam( param_name, doub_iter->second, value, name, lineno );
    }

    StrMap::iterator str_iter = m_strs.find( param_name );
    if ( str_iter != m_strs.end() )
    {
        return setParam( param_name, str_iter->second, value, name, lineno );
    }

    buildError( m_module_name,
                param_name,
                "'" + m_module_name
                + "' has no parameter called '"
                + param_name + "'",
                name,
                lineno );
    return false;
}

bool
Builder::doBuildParam( const std::string & param_name, const std::string & value,
                       const std::string & name,
                       int lineno )
{
    IntMap::iterator int_iter = m_ints.find( param_name );
    if ( int_iter != m_ints.end() )
    {
        return setParam( param_name, int_iter->second, value, name, lineno );
    }

    BoolMap::iterator bool_iter = m_bools.find( param_name );
    if ( bool_iter != m_bools.end() )
    {
        return setParam( param_name, bool_iter->second, value, name, lineno );
    }

    DoubMap::iterator doub_iter = m_doubs.find( param_name );
    if ( doub_iter != m_doubs.end() )
    {
        return setParam( param_name, doub_iter->second, value, name, lineno );
    }

    StrMap::iterator str_iter = m_strs.find( param_name );
    if ( str_iter != m_strs.end() )
    {
        return setParam( param_name, str_iter->second, value, name, lineno );
    }

    if ( param_name == "version" )
    {
        m_parsed_version = value;
        return ( m_version == value );
    }

    buildError( m_module_name,
                param_name,
                "'" + m_module_name
                + "' has no parameter called '"
                + param_name + "'",
                name,
                lineno );
    return false;
}

void
Builder::doCreateConfFile( std::ostream& conf )
{
    conf << "/* " << m_module_name << " Configuration file */\n\n";

    createConfFileEntry( conf,
                         m_module_name,
                         "version",
                         m_version,
                         "" );
    m_parsed_version = m_version;

    for ( IntMap::iterator iter = m_ints.begin();
          iter != m_ints.end();
          ++iter )
    {
        createConfFileEntry( conf,
                             m_module_name,
                             iter->first,
                             iter->second.get(),
                             iter->second.desc() );
    }

    for ( BoolMap::iterator iter = m_bools.begin();
          iter != m_bools.end();
          ++iter )
    {
        createConfFileEntry( conf,
                             m_module_name,
                             iter->first,
                             iter->second.get(),
                             iter->second.desc() );
    }

    for ( DoubMap::iterator iter = m_doubs.begin();
          iter != m_doubs.end();
          ++iter )
    {
        createConfFileEntry( conf,
                             m_module_name,
                             iter->first,
                             iter->second.get(),
                             iter->second.desc() );
    }

    for ( StrMap::iterator iter = m_strs.begin();
          iter != m_strs.end();
          ++iter )
    {
        createConfFileEntry( conf,
                             m_module_name,
                             iter->first,
                             iter->second.get(),
                             iter->second.desc() );
    }
}

bool
Builder::setParam( const std::string &,
                   ParamInfo< int > param,
                   int value,
                   const std::string &,
                   int )
{
    param.set( value );
    return true;
}

bool
Builder::setParam( const std::string & param_name,
                   ParamInfo< bool > param,
                   int value,
                   const std::string & name,
                   int lineno )
{
    param.set( (bool)value );
    if ( value != 0 && value != 1 )
    {
        buildWarning( m_module_name,
                      param_name,
                      "casting from int to bool for parameter '"
                      + param_name + "'",
                      name,
                      lineno );
    }
    return true;
}

bool
Builder::setParam( const std::string &,
                   ParamInfo< double > param,
                   int value,
                   const std::string &,
                   int )
{
    param.set( value );
    return true;
}

bool
Builder::setParam( const std::string & param_name,
                   ParamInfo< std::string >,
                   int,
                   const std::string & name,
                   int lineno )
{
    buildError( m_module_name,
                param_name,
                "'" + m_module_name
                + "' has no numeric parameter called '"
                + param_name + "'",
                name,
                lineno );
    return false;
}

bool
Builder::setParam( const std::string & param_name,
                   ParamInfo< int >,
                   bool,
                   const std::string & name,
                   int lineno )
{
    buildError( m_module_name,
                param_name,
                "'" + m_module_name
                + "' has no boolean parameter called '"
                + param_name + "'",
                name,
                lineno );
    return false;
}

bool
Builder::setParam( const std::string &,
                   ParamInfo< bool > param,
                   bool value,
                   const std::string &,
                   int )
{
    param.set( value );
    return true;
}

bool
Builder::setParam( const std::string & param_name,
                   ParamInfo< double >,
                   bool,
                   const std::string & name,
                   int lineno )
{
    buildError( m_module_name,
                param_name,
                "'" + m_module_name
                + "' has no boolean parameter called '"
                + param_name + "'",
                name,
                lineno );
    return false;
}

bool
Builder::setParam( const std::string & param_name,
                   ParamInfo< std::string >,
                   bool,
                   const std::string & name,
                   int lineno )
{
    buildError( m_module_name,
                param_name,
                "'" + m_module_name
                + "' has no boolean parameter called '"
                + param_name + "'",
                name,
                lineno );
    return false;
}


bool
Builder::setParam( const std::string & param_name,
                   ParamInfo< int > param,
                   double value,
                   const std::string & name,
                   int lineno )
{
    param.set( (int)value );
    if ( value != (int)value )
    {
        buildWarning( m_module_name,
                      param_name,
                      "casting from floating point to integer for parameter '"
                      + param_name + "'",
                      name,
                      lineno );
    }
    return false;
}

bool
Builder::setParam( const std::string & param_name,
                   ParamInfo< bool > param,
                   double value,
                   const std::string & name,
                   int lineno )
{
    param.set( (bool)value );
    if ( value != 0 && value != 1 )
    {
        buildWarning( m_module_name,
                      param_name,
                      "casting from floating point to bool for parameter '"
                      + param_name + "'",
                      name,
                      lineno );
    }
    return false;
}

bool
Builder::setParam( const std::string &,
                   ParamInfo< double > param,
                   double value,
                   const std::string &,
                   int )
{
    param.set( value );
    return true;
}

bool
Builder::setParam( const std::string & param_name,
                   ParamInfo< std::string >,
                   double,
                   const std::string & name,
                   int lineno )
{
    buildError( m_module_name,
                param_name,
                "'" + m_module_name
                + "' has no numeric parameter called '"
                + param_name + "'",
                name,
                lineno );
    return false;
}

bool
Builder::setParam( const std::string & param_name,
                   ParamInfo< int >,
                   const std::string & value,
                   const std::string & name,
                   int lineno )
{
    buildError( m_module_name,
                param_name,
                "'" + m_module_name
                + "' has no string parameter (" + value + ") called '"
                + param_name + "'",
                name,
                lineno );
    return false;
}

bool
Builder::setParam( const std::string & param_name,
                   ParamInfo< bool >,
                   const std::string &,
                   const std::string & name,
                   int lineno )
{
    buildError( m_module_name,
                param_name,
                "'" + m_module_name
                + "' has no string parameter called '"
                + param_name + "'",
                name,
                lineno );
    return false;
}

bool
Builder::setParam( const std::string & param_name,
                   ParamInfo< double >,
                   const std::string &,
                   const std::string & name,
                   int lineno )
{
    buildError( m_module_name,
                param_name,
                "'" + m_module_name
                + "' has no string parameter called '"
                + param_name + "'",
                name, lineno );
    return false;
}

bool
Builder::setParam( const std::string &,
                   ParamInfo< std::string > param,
                   const std::string & value,
                   const std::string &,
                   int )
{
    param.set( value );
    return true;
}

}
}
