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


/* This file contains miscellaneous components for the coach language */

#include "coach_lang_comp.h"


namespace rcss
{
  namespace clang
  {    
  
    std::ostream&
    DirComm::print( std::ostream& out ) const
    {
      out << "("
          << (isPositive() ? "do" : "dont") << " "
          << (isOurSide() ? "our" : "opp") << " "
          << m_players << " ";
      if( getActions().empty() )
        out << "(null)";
      else
        {
          for( Storage::const_iterator i = getActions().begin();
               i != getActions().end(); ++i )
            out << **i;
        }
      return out << ")";
    }

    std::ostream&
    DirComm::printPretty( std::ostream& out, const std::string& line_header ) const
    {
      out << line_header 
          << (isOurSide() ? "our team" : "opponent") << " "
          << m_players << " "
          << (isPositive() ? "DO" : "DON'T") << ": " << std::endl;
      if( getActions().empty() )
        out << line_header << " (null)\n";
      else
        {
          for( Storage::const_iterator i = getActions().begin();
               i != getActions().end(); ++i )
            (*i)->printPretty( out, line_header + " " );
        }
      return out;
    }

    
    /******** Tokens ********************/
    std::ostream&
    TokRule::print( std::ostream& out ) const
    {
      out << "(" << getTTL() << " ";
      if( getCond() == NULL )
        out << "(null)";
      else
        out << *getCond();

      for( Storage::const_iterator iter = getDirs().begin();
           iter != getDirs().end(); ++iter )
        {
          if( *iter == NULL )
            out << " (null)";
          else
            out << " " << **iter;
        }
      return out << ")";
    }

    std::ostream&
    TokRule::printPretty( std::ostream& out, const std::string& line_header ) const
    {
      out << line_header << "for " << getTTL() << " cycles, if" << std::endl;
      if( getCond() == NULL )
        out << line_header << " +(null)";
      else
        getCond()->printPretty( out, line_header + " +" );

      out << line_header << "then" << std::endl;
  
      for( Storage::const_iterator iter = getDirs().begin();
           iter != getDirs().end(); ++iter )
        {
          if( *iter == NULL )
            out << line_header << " *(null)\n";
          else
            (*iter)->printPretty( out, line_header + " *");
        }
      return out;
    }

    std::ostream&
    DefRule::print( std::ostream& out ) const
    { 
      out << "(definerule " << M_name;
      if( M_model )
        out << " model ";
      else
        out << " direc ";
      
      if( getRule() == NULL )
        out << "(null)";
      else
        out << *getRule();
      return out << ")"; 
      }
    
    std::ostream& 
    DefRule::printPretty( std::ostream& out, const std::string& line_header ) const
    { 
      out << line_header << "Define Rule " << M_name;
      if( M_model )
        out << "- model\n";
      else
        out << "- directive\n";
      
      if( getRule() == NULL )
        out << line_header << " (null)\n";
      else
        getRule()->printPretty( out, line_header + " ");
      return out;
    } 
  }
}
