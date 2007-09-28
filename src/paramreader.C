/* -*- Mode: C++ -*- */

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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_SSTREAM
#include <sstream>
#else
#include <strstream>
#endif

#include <cstring>
#include <string>
#include <cstdlib>
#include "paramreader.h"
#include <climits>

ParamReader::ParamReader()
{
}

ParamReader::~ParamReader()
{
}


void
ParamReader::readParams( const std::string& path_name )
{
    std::string path = tildeExpand( path_name );

  std::ifstream ifile;

  ifile.open ( path.c_str () );
  if ( !ifile.is_open () )
    {
      std::ofstream ofile;
      ofile.open ( path.c_str () );
      if ( !ofile.is_open () )
        {
          std::cerr << "can't open config file, " << path << std::endl ;
        }
      else
        {
          writeConfig ( ofile );
          ofile.close ();
          std::cout << "created config file, " << path << std::endl;
          return;
        }
    }

  int line_no = 1;
  while( ifile.good () ) //!ifile.eof () )
    {
      /* ignore remark line */
      if ( ifile.peek () == '#' || ifile.peek () == '\n')
        {
          // ignore this line
//          std::strstreambuf tmp;
//            ifile.get ( tmp, '\n' );
          // get the '\n' that the get above, didn't get
          ifile.ignore( INT_MAX, '\n' );
          line_no++;
          continue ;
        }

      std::string param;
      ifile >> param;
      if ( param.empty () )
        {
          continue;
        }


      bool find_colon = true;
      if ( param[ param.length () - 1 ] == ':' )
        {
          param.resize ( param.length () - 1 );
          find_colon = false;
        }

      bool match = true;
      std::map< std::string, DataHolder< int* > >::iterator int_i;
      std::map< std::string, DataHolder< bool* > >::iterator bool_i;
      std::map< std::string, DataHolder< std::string* > >::iterator str_i;
      std::map< std::string, DataHolder< double* > >::iterator double_i;

      if ( ( bool_i = bool_map.find ( param ) ) != bool_map.end () )
        {
          *(bool_i->second.M_data) = true;
        }
      else
        {
          if ( find_colon )
            {
              std::string colon;
              ifile >> colon;

              if ( colon != std::string ( ":" ) )
                {
                  std::cerr << path << ": " << line_no
                       << ": Expecting colon after parameter name, not `"
                       << colon << "'\n";

                  // ignore the rest of this line
//                  std::strstreambuf tmp;
//                    ifile.get ( tmp, '\n' );
                  // get the `\n' that the get above, didn't get
                  ifile.ignore( INT_MAX, '\n' );
                  line_no++;
                  continue;
                }
            }

          if ( ( int_i = int_map.find ( param ) ) != int_map.end () )
            {
              ifile >> *(int_i->second.M_data);
            }
          else if ( ( bool_i = onoff_map.find ( param ) ) != onoff_map.end () )
            {

              std::string value;
              ifile >> value;
              if ( value == "on" || value == "true" )
                *(bool_i->second.M_data) = true;
              else if ( value == "off" || value == "false" )
                *(bool_i->second.M_data) = false;
              else
                {
                  std::cerr << path << ": " << line_no
                       << ": Expecting `on', `off', `true' or `false', not `"
                       << value << "'\n";

                  // ignore the rest of this line
//                  std::strstreambuf tmp;
//                    ifile.get ( tmp, '\n' );
                  // get the '\n' that the get above, didn't get
                  ifile.ignore( INT_MAX, '\n');
                  line_no++;
                  continue;
                }
            }
          else if ( ( str_i = str_map.find ( param ) ) != str_map.end () )
            {
              char next = ifile.peek();
              while( next == ' ' || next == '\t' )
                {
                  ifile.ignore();
                  next = ifile.peek();
                }

              if( next == '"' )
                {
                  ifile.ignore();
                  *(str_i->second.M_data) = "";
                  std::string tmp_str;
                  do
                    {
                      ifile >> tmp_str;
                      if( !( str_i->second.M_data->empty() ) )
                        *(str_i->second.M_data) += std::string( " " );
                      *(str_i->second.M_data) += tmp_str;
                    }
                  while( ifile.good()
                         && tmp_str[ tmp_str.length() - 1 ] != '"' );

                  *(str_i->second.M_data)
                  = str_i->second.M_data->substr( 0,
                                                  str_i->second.M_data->length() -1 );
                }
              else
                ifile >> *(str_i->second.M_data);

//                cout << str_i->first << " = "
//                     << *(str_i->second.M_data) << std::endl;
            }
          else if ( ( double_i = double_map.find ( param ) )
                    != double_map.end () )
            {
              ifile >> *(double_i->second.M_data);
            }
          else
            match = false;
        }


      if ( ifile.fail () )
        {
          std::string value;
          ifile.clear ();
          ifile >> value;
          std::cerr << path << ": " << line_no
               << ": Error converting value `"
               << value << "'\n";
        }

      if (!match)
        {
          std::cerr << path << ": " << line_no
               << ": Unrecognized parameter in file `"
               << param << "'\n";
        }

      // ignore the rest of this line
//      std::strstreambuf tmp;
//        ifile.get ( tmp, '\n' );
      // get the '\n' that the get above, didn't get
      ifile.ignore( INT_MAX, '\n');
      line_no++;
    }

  ifile.close () ;
}


void ParamReader::readOptions ( const int& orig_argc,
                                const char* const orig_argv[] )
{
  int argc = orig_argc;
  const char * const *argv = orig_argv;
  char buf[MaxMesg];

  argc--;
  argv++;

  while (argc)
    {
      if ( sscanf ( *argv, "-%s", buf ) != 1 )
        {
          argv++ ;
          argc-- ;
          continue;
        }

      std::string param = buf;
      bool match = true;
      std::map< std::string, DataHolder< int* > >::iterator int_i;
      std::map< std::string, DataHolder< bool* > >::iterator bool_i;
      std::map< std::string, DataHolder< std::string* > >::iterator str_i;
      std::map< std::string, DataHolder< double* > >::iterator double_i;

      if ( ( bool_i = bool_map.find ( param ) ) != bool_map.end () )
        {
          *(bool_i->second.M_data) = true;
        }
      else
        {
         if ( ( int_i = int_map.find ( param ) ) != int_map.end () )
           {
             if( argc > 1 )
               {
#ifdef HAVE_SSTREAM
                 std::istringstream ifile( *(argv + 1) );
#else
                 std::istrstream ifile( *(argv + 1) );
#endif
                 ifile >> *(int_i->second.M_data);
               }
             else
               {
                 std::cerr << "Error: Option \"" << param
                      << "\" has no int value specified\n";
                 match = false;
               }
           }
         else if ( ( bool_i = onoff_map.find ( param ) ) != onoff_map.end () )
           {
             if( argc > 1 )
               {
#ifdef HAVE_SSTREAM
                 std::istringstream ifile( *(argv + 1) );
#else
                 std::istrstream ifile( *(argv + 1) );
#endif
                 std::string value;
                 ifile >> value;
                 if ( value == "on" || value == "true" )
                   *(bool_i->second.M_data) = true;
                 else if ( value == "off" || value == "false" )
                   *(bool_i->second.M_data) = false;
                 else
                   {
                     std::cerr << __FILE__ << ": " << __LINE__
                          << ": Expecting `on', `off', `true' or `false', not `"
                          << value << "' for command line option `" << param
                          << "'\n";
                     match = false;
                   }
               }
             else
               {
                 std::cerr << "Error: Option \"" << param <<
                   "\" has no on/off value specified\n";
                 match = false;
               }
           }
         else if ( ( str_i = str_map.find ( param ) ) != str_map.end () )
           {
            if( argc > 1 )
               {
                 *(str_i->second.M_data) = *(argv + 1);
               }
            else
               {
                 std::cerr << "Error: Option \"" << param <<
                   "\" has no string value specified\n";
                 match = false;
               }
           }
         else if ( ( double_i = double_map.find ( param ) )
                   != double_map.end () )
           {
            if( argc > 1 )
               {
#ifdef HAVE_SSTREAM
                 std::istringstream ifile( *(argv + 1) );
#else
                 std::istrstream ifile( *(argv + 1) );
#endif
                 ifile >> *(double_i->second.M_data);
               }
            else
               {
                 std::cerr << "Error: Option \"" << param <<
                   "\" has no real value specified\n";
                 match = false;
               }
           }
         else
           match = false;

         if ( match )
           {
             argv++ ;
             argc-- ;
           }
        }
      if (!match) {
        /* pfr: It's "okay" for options to be unrecognized on the command line because
           it may be intended for the another parameter reader instantiation */
        //std::cerr << "Unrecognized parameter on command line: " << buf << std::endl;
      }

      argv++ ;
      argc-- ;
    }

}
