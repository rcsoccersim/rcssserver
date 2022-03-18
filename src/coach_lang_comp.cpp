/* -*- Mode: C++ -*- */

/*
 *Copyright:

 Copyright (C) 1996-2000 Electrotechnical Laboratory.
 Itsuki Noda, Yasuo Kuniyoshi and Hitoshi Matsubara.
 Copyright (C) 2000, 2001 RoboCup Soccer Server Maintainance Group.
 Patrick Riley, Tom Howard, Daniel Polani, Itsuki Noda,
 Mikhail Prokopenko, Jan Wendler
 Copyright (C) 2002- RoboCup Soccer Simulator Maintainance Group.

 This file is a part of SoccerServer.

 This code is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 3 of the License, or (at your option) any later version.

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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "coach_lang_comp.h"

#include <iostream>

namespace rcss {
namespace clang {

std::ostream &
DirComm::print( std::ostream & out ) const
{
    out << "("
        << (isPositive() ? "do" : "dont") << " "
        << (isOurSide() ? "our" : "opp") << " "
        << M_players << " ";
    if ( getActions().empty() )
    {
        out << "(null)";
    }
    else
    {
        for ( Storage::const_reference i : getActions() )
        {
            out << *i;
        }
    }
    return out << ")";
}

std::ostream &
DirComm::printPretty( std::ostream & out,
                      const std::string & line_header ) const
{
    out << line_header
        << (isOurSide() ? "our team" : "opponent") << " "
        << M_players << " "
        << (isPositive() ? "DO" : "DON'T") << ": " << std::endl;
    if ( getActions().empty() )
    {
        out << line_header << " (null)\n";
    }
    else
    {
        for ( Storage::const_reference i : getActions() )
        {
            i->printPretty( out, line_header + " " );
        }
    }
    return out;
}


void
DirComm::clearActions()
{
    M_actions.clear();
}


// DirComm::Storage
// DirComm::detachActions()
// {
//     Storage rval = M_actions;
//     M_actions.clear();
//     return rval;
// }

std::shared_ptr< Dir >
DirComm::deepCopy() const
{
    Storage new_actions;
    for ( Storage::const_reference i : getActions() )
    {
        new_actions.push_back( i->deepCopy() );
    }

    std::shared_ptr< Dir > rval( new DirComm( M_positive,
                                              M_our_side,
                                              M_players,
                                              new_actions ) );
    return rval;
}


std::ostream &
DirNamed::print( std::ostream & out ) const
{
    return out << "\"" << M_name << "\"";
}

std::ostream &
DirNamed::printPretty( std::ostream & out,
                       const std::string & line_header ) const
{
    return out << line_header
               << "directived named \""
               << M_name << "\"" << std::endl;
}


/******** Tokens ********************/

std::ostream &
TokClear::print( std::ostream & out ) const
{
    return out << "(clear)";
}

std::ostream &
TokClear::printPretty( std::ostream & out,
                         const std::string & line_header ) const
{
    return out << line_header << "CLEAR token" << std::endl;
}


std::ostream &
TokRule::print( std::ostream & out ) const
{
    out << "(" << getTTL() << " ";
    if ( ! M_cond )
    {
        out << "(null)";
    }
    else
    {
        out << *M_cond;
    }

    for ( Storage::const_reference dir : getDirs() )
    {
        if ( ! dir )
        {
            out << " (null)";
        }
        else
        {
            out << " " << *dir;
        }
    }
    return out << ")";
}

std::ostream &
TokRule::printPretty( std::ostream & out,
                      const std::string & line_header ) const
{
    out << line_header << "for " << getTTL() << " cycles, if" << std::endl;
    if ( ! M_cond )
    {
        out << line_header << " +(null)";
    }
    else
    {
        M_cond->printPretty( out, line_header + " +" );
    }

    out << line_header << "then" << std::endl;

    for ( Storage::const_reference dir : getDirs() )
    {
        if ( ! dir )
        {
            out << line_header << " *(null)\n";
        }
        else
        {
            dir->printPretty( out, line_header + " *");
        }
    }
    return out;
}

void
TokRule::clearDirs()
{
    M_dirs.clear();
}

std::shared_ptr< Token >
TokRule::deepCopy() const
{
    std::shared_ptr< Cond > new_cond;
    if ( M_cond )
    {
        new_cond = M_cond->deepCopy();
    }

    Storage new_dirs;
    for ( Storage::const_reference dir : M_dirs )
    {
        new_dirs.push_back( dir->deepCopy() );
    }

    std::shared_ptr< Token > rval( new TokRule( M_ttl,
                                                new_cond,
                                                new_dirs ) );
    return rval;
}


std::ostream &
DefCond::print( std::ostream & out ) const
{
    out << "(definec \"" << M_name << "\" ";
    if ( ! M_cond )
    {
        out << " (null)";
    }
    else
    {
        out << *M_cond;
    }
    return out << ")";
}

std::ostream &
DefCond::printPretty( std::ostream & out,
                      const std::string & line_header ) const
{
    out << line_header << "Cond \"" << M_name << "\"" << std::endl;
    if ( ! M_cond )
    {
        out << line_header << " (null)\n";
    }
    else
    {
        M_cond->printPretty( out, line_header + " " );
    }
    return out;
}


 std::ostream &
 DefDir::print( std::ostream & out ) const
 {
     out << "(defined \"" << M_name << "\" ";
     if ( ! M_dir )
     {
         out << "(null)";
     }
     else
     {
         out << *M_dir;
     }
     return out << ")";
 }

std::ostream &
DefDir::printPretty( std::ostream & out,
                     const std::string & line_header ) const
{
    out << line_header << "Dir \"" << M_name << "\"" << std::endl;
    if ( ! M_dir )
    {
        out << line_header << " (null)\n";
    }
    else
    {
        M_dir->printPretty( out, line_header + " " );
    }
    return out;
}

std::ostream &
DefReg::print( std::ostream & out ) const
{
    out << "(definer \"" << M_name << "\" ";
    if ( ! M_reg )
    {
        out << "(null)";
    }
    else
    {
        out << *M_reg;
    }
    return out << ")";
}

std::ostream &
DefReg::printPretty( std::ostream & out,
                     const std::string & line_header ) const
{
    out << line_header << "Region \"" << M_name << "\"" << std::endl;
    if ( ! M_reg )
    {
        out << line_header << " (null)\n";
    }
    else
    {
        M_reg->printPretty( out, line_header + " " );
    }
    return out;
}

std::ostream &
DefAct::print( std::ostream & out ) const
{
    out << "(definea \"" << M_name << "\" ";
    if ( ! M_act )
    {
        out << "(null)";
    }
    else
    {
        out << *M_act;
    }
    return out << ")";
}

std::ostream &
DefAct::printPretty( std::ostream & out,
                     const std::string & line_header ) const
{
    out << line_header << "Action \"" << M_name << "\"" << std::endl;
    if ( ! M_act.get() )
    {
        out << line_header << " (null)\n";
    }
    else
    {
        M_act->printPretty( out, line_header + " ");
    }
    return out;
}



std::ostream &
DefRule::print( std::ostream & out ) const
{
    out << "(definerule " << M_name;
    if ( M_model )
    {
        out << " model ";
    }
    else
    {
        out << " direc ";
    }

    if ( ! M_rule )
    {
        out << "(null)";
    }
    else
    {
        out << *M_rule;
    }
    return out << ")";
}

std::ostream &
DefRule::printPretty( std::ostream & out,
                      const std::string & line_header ) const
{
    out << line_header << "Define Rule " << M_name;
    if ( M_model )
    {
        out << "- model\n";
    }
    else
    {
        out << "- directive\n";
    }

    if ( ! M_rule )
    {
        out << line_header << " (null)\n";
    }
    else
    {
        M_rule->printPretty( out, line_header + " ");
    }
    return out;
}

std::shared_ptr< Def >
DefRule::deepCopy() const
{
    std::shared_ptr< Def > rval;
    if ( M_rule )
    {
        rval = std::shared_ptr< Def >( new DefRule( getName(),
                                                    M_rule->deepCopy(),
                                                    M_model ) );
    }
    else
    {
        rval = std::shared_ptr< Def >( new DefRule( getName(),
                                                    std::shared_ptr< Rule >(),
                                                    M_model ) );
    }

    return rval;
}

}
}
