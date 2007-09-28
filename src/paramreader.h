/* -*- Mode: c++ -*- */

/*
 *Copyright:

    Copyright (C) 1996-2000 Electrotechnical Laboratory. 
    	Itsuki Noda, Yasuo Kuniyoshi and Hitoshi Matsubara.
    Copyright (C) 2000, 2001 RoboCup Soccer Server Maintainance Group.
    	Patrick Riley, Tom Howard, Daniel Polani, Itsuki Noda,
	Mikhail Prokopenko, Jan Wendler 

    This file is a part of SoccerServer.

    This code is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 *EndCopyright:
 */

#ifndef _PARAMREADER_H_
#define _PARAMREADER_H_

//#include <stdlib.h>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "param.h"
#include "types.h"
#include "utility.h"

class ParamReader
{
public:

  enum param_type
  {
    INT,
    STRING,
    DOUBLE,
    BOOL,
    ONOFF
  };

  struct param_t {
    std::string	optname ;
    union
    {
      std::string*	str_p;
      int*	int_p;
      bool*	bool_p;
      double*	double_p;
    };
    ParamReader::param_type	vsize ;
  };

protected:
  ParamReader();

private:
  ParamReader( const ParamReader& ) {}

public:
  virtual ~ParamReader();

protected:
  void
  readParams( const std::string& path );

  void 
  readOptions( const int& argc, 
               const char* const *argv );
    
  virtual void getOptions( const int& argc, const char * const *argv ) = 0;
  virtual void writeConfig( std::ostream& o ) = 0;

public:
  template< typename TYPE >
  struct DataHolder
  {
    TYPE         M_data;
    unsigned int M_version;

    DataHolder()
    {}

    DataHolder( TYPE data, unsigned int version )
      : M_data( data ),
        M_version( version )
      {}
  };

  class Printer
  {
  private:
    std::ostream& M_out;
    unsigned int M_version;
  public:
    Printer( std::ostream& out, unsigned int version )
      : M_out( out ),
        M_version( version )
    {}
    
    template< typename TYPE >
    void
    operator()( const std::pair< const std::string, ParamReader::DataHolder< TYPE > >& item )
    {
      if( item.second.M_version <= M_version )
        M_out << "(" << item.first << " " << *(item.second.M_data) << ")";
    }
  };
  
  class QuotedPrinter
  {
  private:
    std::ostream& M_out;
    unsigned int M_version;
  public:
    QuotedPrinter( std::ostream& out, unsigned int version )
      : M_out( out ),
        M_version( version )
    {}

    template< typename TYPE >
    void
    operator()( const std::pair< const std::string, ParamReader::DataHolder< TYPE > >& item )
    {
      if( item.second.M_version <= M_version )
        M_out << "(" << item.first << " \"" << *(item.second.M_data) << "\")";
    }
  };

  
  // get a dataholder without needing to know the type
  template< typename TYPE >
  DataHolder< TYPE >
  group( TYPE data, unsigned int version )
  {
    return DataHolder< TYPE >( data, version );
  }

private:
  std::map < std::string, DataHolder< int* > > 		int_map;
  std::map < std::string, DataHolder< std::string* > > 	str_map;
  std::map < std::string, DataHolder< bool* > > 	bool_map;
  std::map < std::string, DataHolder< bool* > > 	onoff_map;
  std::map < std::string, DataHolder< double* > > 	double_map;

public:
    const std::map< std::string, DataHolder< int* > >&
    intMap() const
    { return int_map; }

    const std::map< std::string, DataHolder< std::string* > >&
    strMap() const
    { return str_map; }

    const std::map< std::string, DataHolder< bool* > >&
    boolMap() const
    { return bool_map; }

    const std::map< std::string, DataHolder< bool* > >&
    onOffMap() const
    { return onoff_map; }

    const std::map< std::string, DataHolder< double* > >&
    doubleMap() const
    { return double_map; }

protected:

  void
  add2Maps( const std::string& key, DataHolder< int* > value )
  {
    int_map[key] = value;
  }

  void
  add2Maps( const std::string& key, DataHolder< bool* > value, bool onoff = false )
  {
    if ( onoff )
      onoff_map[key] = value;
    else
      bool_map[key] = value;
  }

  void
  add2Maps( const std::string& key, DataHolder< std::string* > value )
  {
    str_map[key] = value;
  }

  void
  add2Maps( const std::string& key, DataHolder< double* > value )
  {
    double_map[key] = value;
  }
};


#endif
