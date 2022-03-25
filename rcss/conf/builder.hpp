// -*-c++-*-

/***************************************************************************
                               builder.hpp
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

#ifndef BUILDER_HPP
#define BUILDER_HPP

#include "paramsetter.hpp"
#include "paramgetter.hpp"

#include <string>
#include <list>
#include <map>
#include <iostream>

namespace rcss {
namespace conf {

class StatusHandler;
class Parser;

class Builder {
public:

    Builder( const std::string & progname,
             const std::string & version,
             const std::string & module_name );

    Builder( Builder * parent,
             const std::string & version,
             const std::string & module_name );

    virtual
    ~Builder();

    template< typename V >
    bool buildParam( const std::string & module_name,
                     const std::string & param_name,
                     V value,
                     const std::string & name,
                     int lineno );

    void createConfFile( std::ostream & conf,
                         const std::string & module_name );

    void displayHelp();

    /// removeHandler must be called before the handler is destroyed
    void addHandler( StatusHandler & handler );

    void removeHandler( StatusHandler & handler );

    /// sets the specified parameter to the specified value
    template< typename P >
    bool set( const std::string & param,
              P value )
      {
          return doBuildParam( param, value, "none", 0 );
      }

    /// gets the value of the specified parameter
    template< typename P >
    bool get( const std::string & param,
              P & value ) const;

    bool genericHelpRequested() const;

    bool detailedHelpRequested() const;

    Parser * parser();

    template< typename V >
    void addParam( const std::string & name,
                   const Setter< V > & setter,
                   const Getter< V > & getter,
                   const std::string & desc )
      {
          addParam( name, ParamInfo< V >( setter, getter, desc ) );
      }

    template< typename V >
    void addParam( const std::string & name,
                   V & param,
                   const std::string & desc )
      {
          addParam( name,
                    makeSetter( param ),
                    makeGetter( param ),
                    desc );
      }

    const
    std::string & getModuleName() const;

    const
    std::string & version() const;

    const
    std::string & parsedVersion() const;


private:
    friend class Parser;
    friend class ParseErrorHandler;


    void addChild( Builder & child );

    void removeChild( Builder & child );


    template< typename V >
    void createConfFileEntry( std::ostream & conf,
                              const std::string & module_name,
                              const std::string & param_name,
                              V value,
                              const std::string & desc );

    template< typename V >
    void displayHelpEntry( std::ostream & out,
                           const std::string & module_name,
                           const std::string & param_name,
                           V value,
                           const std::string & desc );

    void displayHelpEntry( std::ostream & out,
                           const std::string & module_name,
                           const std::string & param_name,
                           const std::string & desc );

    void displayHelpEntry( std::ostream & out,
                           const std::string & param_name,
                           const std::string & desc );

    template< typename V >
    void writeConfValue( std::ostream & conf,
                         V value );


    template< typename V >
    void writeConfType( std::ostream & conf,
                        V value );

    void parseError( const std::string & curr,
                     const std::string & err,
                     const std::string & name,
                     int lineno );

    void buildError( const std::string & module,
                     const std::string & param,
                     const std::string & err,
                     const std::string & name,
                     int lineno );

    void buildWarning( const std::string & module,
                       const std::string & param,
                       const std::string & warn,
                       const std::string & name,
                       int lineno );

    void creatingConfFile( const std::string & conf_name );

    void createdConfFile( const std::string & conf_name );

    void confCreationFailed( const std::string & conf_name,
                             int error );

    void includeFailed( const std::string & filename,
                        const std::string & error,
                        const std::string & name,
                        int lineno );

    bool success() const;

    void reset();

    const
    std::string & progName() const;

    void requestGenericHelp();

    void requestDetailedHelp();

    void requestDetailedHelp( const std::string & module_name );

    void requestDetailedHelpX( const char * begin,
                               const char * end )
      {
          requestDetailedHelp( std::string( begin, end ) );
      }

    void addedToParser( Parser & p );

    void removedFromParser();

    // does not take ownership
    Parser * m_parser;
    std::list< StatusHandler * > m_handlers;
    std::list< Builder * > m_children;

    bool m_err;

    std::string m_progname;
    const std::string m_version;
    std::string m_parsed_version;

    Builder* m_parent;
    bool m_generic_help_requested;
    bool m_detailed_help_requested;


    template< typename V >
    class ParamInfo {
    public:
        ParamInfo( const Setter< V > & setter,
                   const Getter< V > & getter,
                   const std::string & desc )
            : m_setter( setter ),
              m_getter( getter ),
              m_desc( desc )
          { }

        void set( V value )
          {
              m_setter( value );
          }

        V get() const
          {
              return m_getter();
          }

        const
        std::string & desc() const
          {
              return m_desc;
          }
    private:
        Setter< V > m_setter;
        Getter< V > m_getter;
        const std::string m_desc;
    };

    typedef std::map< std::string, ParamInfo< int > > IntMap;
    typedef std::map< std::string, ParamInfo< bool > > BoolMap;
    typedef std::map< std::string, ParamInfo< double > > DoubMap;
    typedef std::map< std::string, ParamInfo< std::string > > StrMap;

    virtual
    void doRequestDetailedHelp( const std::string & module_name );

    virtual
    void displayUsage( const std::string & progname );

    virtual
    void displayGenericHelp();

    virtual
    void displayDetailedHelp();

    virtual
    bool doBuildParam( const std::string & module_name,
                       const std::string & param_name,
                       int value,
                       const std::string & name,
                       int lineno );

    virtual
    bool doBuildParam( const std::string & module_name,
                       const std::string & param_name,
                       bool value,
                       const std::string & name,
                       int lineno );

    virtual
    bool doBuildParam( const std::string & module_name,
                       const std::string & param_name,
                       double value,
                       const std::string & name,
                       int lineno );

    virtual
    bool doBuildParam( const std::string & module_name,
                       const std::string & param_name,
                       const std::string & value,
                       const std::string & name,
                       int lineno );

    virtual
    void doCreateConfFile( std::ostream & conf );

    template< typename V >
    void addParam( const std::string & name,
                   const ParamInfo< V > & info );

    bool doBuildParam( const std::string & param_name,
                       int value,
                       const std::string & name,
                       int lineno );

    bool doBuildParam( const std::string & param_name,
                       bool value,
                       const std::string & name,
                       int lineno );

    bool doBuildParam( const std::string & param_name,
                       double value,
                       const std::string & name,
                       int lineno );

    bool doBuildParam( const std::string & param_name,
                       const std::string & value,
                       const std::string & name,
                       int lineno );

    bool setParam( const std::string & param_name,
                   ParamInfo< int > param,
                   int value,
                   const std::string & name,
                   int lineno );

    bool setParam( const std::string & param_name,
                   ParamInfo< int > param,
                   bool value,
                   const std::string & name,
                   int lineno );

    bool setParam( const std::string & param_name,
                   ParamInfo< int > param,
                   double value,
                   const std::string & name,
                   int lineno );

    bool setParam( const std::string & param_name,
                   ParamInfo< int > param,
                   const std::string & value,
                   const std::string & name,
                   int lineno );

    bool setParam( const std::string & param_name,
                   ParamInfo< bool > param,
                   int value,
                   const std::string & name,
                   int lineno );

    bool setParam( const std::string & param_name,
                   ParamInfo< bool > param,
                   bool value,
                   const std::string & name,
                   int lineno );

    bool setParam( const std::string & param_name,
                   ParamInfo< bool > param,
                   double value,
                   const std::string & name,
                   int lineno );

    bool setParam( const std::string & param_name,
                   ParamInfo< bool > param,
                   const std::string & value,
                   const std::string & name,
                   int lineno );

    bool setParam( const std::string & param_name,
                   ParamInfo< double > param,
                   int value,
                   const std::string & name,
                   int lineno );

    bool setParam( const std::string & param_name,
                   ParamInfo< double > param,
                   bool value,
                   const std::string & name,
                   int lineno );

    bool setParam( const std::string & param_name,
                   ParamInfo< double > param,
                   double value,
                   const std::string & name,
                   int lineno );

    bool setParam( const std::string & param_name,
                   ParamInfo< double > param,
                   const std::string & value,
                   const std::string & name,
                   int lineno );

    bool setParam( const std::string & param_name,
                   ParamInfo< std::string > param,
                   int value,
                   const std::string & name,
                   int lineno );

    bool setParam( const std::string & param_name,
                   ParamInfo< std::string > param,
                   bool value,
                   const std::string & name,
                   int lineno );

    bool setParam( const std::string & param_name,
                   ParamInfo< std::string > param,
                   double value,
                   const std::string & name,
                   int lineno );

    bool setParam( const std::string & param_name,
                   ParamInfo< std::string > param,
                   const std::string & value,
                   const std::string & name,
                   int lineno );


    const std::string m_module_name;
    IntMap m_ints;
    BoolMap m_bools;
    DoubMap m_doubs;
    StrMap m_strs;
};

template<>
inline
bool
Builder::get< int >( const std::string & param,
                     int & value ) const
{
    IntMap::const_iterator i = m_ints.find( param );
    if ( i == m_ints.end() )
    {
        return false;
    }

    value = i->second.get();
    return true;
}

template<>
inline
bool
Builder::get< bool >( const std::string & param,
                      bool & value ) const
{
    BoolMap::const_iterator i = m_bools.find( param );
    if ( i == m_bools.end() )
    {
        return false;
    }
    value = i->second.get();
    return true;
}

template<>
inline
bool
Builder::get< double >( const std::string & param,
                        double & value ) const
{
    DoubMap::const_iterator i = m_doubs.find( param );
    if ( i == m_doubs.end() )
    {
        return false;
    }
    value = i->second.get();
    return true;
}

template<>
inline
bool
Builder::get< std::string >( const std::string & param,
                             std::string & value ) const
{
    StrMap::const_iterator i = m_strs.find( param );
    if ( i == m_strs.end() )
    {
        return false;
    }
    value = i->second.get();
    return true;
}


template<>
inline
void
Builder::addParam< int >( const std::string & name,
                          const ParamInfo< int > & info )
{
    m_ints.insert( std::make_pair( name, info ) );
}

template<>
inline
void
Builder::addParam< bool >( const std::string & name,
                           const ParamInfo< bool > & info )
{
    m_bools.insert( std::make_pair( name, info ) );
}

template<>
inline
void
Builder::addParam< double >( const std::string & name,
                             const ParamInfo< double > & info )
{
    m_doubs.insert( std::make_pair( name, info ) );
}

template<>
inline
void
Builder::addParam< std::string >( const std::string & name,
                                  const ParamInfo< std::string > & info )
{
    m_strs.insert( std::make_pair( name, info ) );
}

template< typename V >
inline
bool
Builder::buildParam( const std::string & module_name,
                     const std::string & param_name,
                     V value,
                     const std::string& name,
                     int lineno )
{
    bool rval = doBuildParam( module_name,
                              param_name,
                              value,
                              name,
                              lineno );
    for ( std::list< Builder* >::iterator i = m_children.begin();
          i != m_children.end();
          ++i )
    {
        rval = (*i)->buildParam( module_name, param_name, value, name, lineno )
            && rval;
    }
    return rval;
}


template< typename V >
inline
void
Builder::createConfFileEntry( std::ostream & conf,
                              const std::string & module_name,
                              const std::string & param_name,
                              V value,
                              const std::string & desc )
{
    conf << "# " << module_name << "::"
         << param_name << std::endl;
    if ( ! desc.empty() )
    {
        conf << "/* ";
        int count = 3;
        std::string::const_iterator start = desc.begin();
        std::string::const_iterator end = desc.begin();
        for ( std::string::const_iterator i = desc.begin();
              i != desc.end();
              ++i, ++count )
        {
            switch ( *i ) {
            case '\n':
                end = i;
                conf << std::string( start, end ) << std::endl;
                count = 0;
                start = end = i+1;
                break;
            case ' ':
            case '\t':
                end = i;
                break;
            default:
                if ( count > 70 )
                {
                    conf << std::string( start, end ) << std::endl;
                    for( std::string::const_iterator j = end;
                         j != i; ++j )
                    {
                        if( *j == ' ' || *j == '\t' )
                            ++end;
                        else
                            break;
                    }
                    count = std::distance( end, i );
                    start = end;
                }
                break;
            }
        }
        conf << std::string( start, desc.end() ) << " */\n";
    }
    conf << module_name << "::" << param_name
         << " = ";
    writeConfValue( conf, value );
    conf << std::endl << std::endl;
}


template< typename V >
inline
void
Builder::displayHelpEntry( std::ostream & conf,
                           const std::string & module_name,
                           const std::string & param_name,
                           V value,
                           const std::string & desc )
{
    conf << "\t" << module_name << "::"
         << param_name << "=";
    writeConfType( conf, value );
    conf << std::endl;
    if ( ! desc.empty() )
    {
        conf << "\t\t";
        int count = 3;
        std::string::const_iterator start = desc.begin();
        std::string::const_iterator end = desc.begin();
        for ( std::string::const_iterator i = desc.begin();
              i != desc.end();
              ++i, ++count )
        {
            switch ( *i ) {
            case '\n':
                end = i;
                conf << std::string( start, end ) << std::endl << "\t\t";
                count = 0;
                start = end = i+1;
                break;
            case ' ':
            case '\t':
                end = i;
                break;
            default:
                if ( count > 62 )
                {
                    conf << std::string( start, end ) << std::endl << "\t\t";
                    for( std::string::const_iterator j = end;
                         j != i; ++j )
                    {
                        if( *j == ' ' || *j == '\t' )
                            ++end;
                        else
                            break;
                    }
                    count = std::distance( end, i );
                    start = end;
                }
                break;
            }
        }
        conf << std::string( start, desc.end() ) << "\n\n";
    }
    conf << "\t\tcurrent value: ";
    writeConfValue( conf, value );
    conf << std::endl << std::endl;
}

inline
void
Builder::displayHelpEntry( std::ostream & strm,
                           const std::string & module_name,
                           const std::string & param_name,
                           const std::string & desc )
{
    strm << "\t" << module_name << "::" << param_name;
    strm << std::endl;
    if ( ! desc.empty() )
    {
        strm << "\t\t";
        int count = 3;
        std::string::const_iterator start = desc.begin();
        std::string::const_iterator end = desc.begin();
        for( std::string::const_iterator i = desc.begin();
             i != desc.end(); ++i, ++count )
        {
            switch( *i ) {
            case '\n':
                end = i;
                strm << std::string( start, end ) << std::endl << "\t\t";
                count = 0;
                start = end = i+1;
                break;
            case ' ':
            case '\t':
                end = i;
                break;
            default:
                if( count > 62 )
                {
                    strm << std::string( start, end ) << std::endl << "\t\t";
                    for( std::string::const_iterator j = end;
                         j != i;
                         ++j )
                    {
                        if( *j == ' ' || *j == '\t' )
                            ++end;
                        else
                            break;
                    }
                    count = std::distance( end, i );
                    start = end;
                }
                break;
            }
        }
        strm << std::string( start, desc.end() ) << "\n\n";
    }
}


inline
void
Builder::displayHelpEntry( std::ostream & conf,
                           const std::string & param_name,
                           const std::string & desc )
{
    conf << "\t" << param_name;
    conf << std::endl;;
    if ( ! desc.empty() )
    {
        conf << "\t\t";
        int count = 3;
        std::string::const_iterator start = desc.begin();
        std::string::const_iterator end = desc.begin();
        for ( std::string::const_iterator i = desc.begin();
              i != desc.end();
              ++i, ++count )
        {
            switch ( *i ) {
            case '\n':
                end = i;
                conf << std::string( start, end ) << std::endl << "\t\t";
                count = 0;
                start = end = i+1;
                break;
            case ' ':
            case '\t':
                end = i;
                break;
            default:
                if ( count > 62 )
                {
                    conf << std::string( start, end ) << std::endl << "\t\t";
                    for( std::string::const_iterator j = end;
                         j != i; ++j )
                    {
                        if( *j == ' ' || *j == '\t' )
                            ++end;
                        else
                            break;
                    }
                    count = std::distance( end, i );
                    start = end;
                }
                break;
            }
        }
        conf << std::string( start, desc.end() ) << "\n\n";
        conf.flush();
    }
}


template< typename V >
inline
void
Builder::writeConfValue( std::ostream & conf,
                         V value )
{
    conf << value;
}

template<>
inline
void
Builder::writeConfValue< bool >( std::ostream & conf,
                                 bool value )
{
    if ( value )
    {
        conf << "true";
    }
    else
    {
        conf << "false";
    }
}

template<>
inline
void
Builder::writeConfValue< const bool& >( std::ostream & conf,
                                        const bool & value )
{
    if ( value )
    {
        conf << "true";
    }
    else
    {
        conf << "false";
    }
}

template<>
inline
void
Builder::writeConfValue< const std::string & >( std::ostream & conf,
                                                const std::string & value )
{
    conf << '\'' << value << '\'';
}

template<>
inline
void
Builder::writeConfValue< std::string >( std::ostream & conf,
                                        std::string value )
{
    conf << '\'' << value << '\'';
}



template< typename V >
inline
void
Builder::writeConfType( std::ostream & conf,
                        V value )
{
    conf << "<VALUE>";
}

template<>
inline
void
Builder::writeConfType< bool >( std::ostream & conf,
                                bool )
{
    conf << "<on|off|true|false|1|0|>";
}

template<>
inline
void
Builder::writeConfType< int >( std::ostream & conf,
                               int )
{
    conf << "<INTEGER>";
}


template<>
inline
void
Builder::writeConfType< double >( std::ostream& conf, double )
{
    conf << "<REAL>";
}

template<>
inline
void
Builder::writeConfType< const std::string & >( std::ostream & conf,
                                               const std::string & )
{
    conf << "'<STRING>'";
}

template<>
inline
void
Builder::writeConfType< std::string >( std::ostream & conf,
                                       std::string )
{
    conf << "'<STRING>'";
}

}
}

#endif
