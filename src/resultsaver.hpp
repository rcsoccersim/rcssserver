// -*-c++-*-

/***************************************************************************
                                resultsaver.hpp
                    Abstract Base Class for saving results
                             -------------------
    begin                : 08-MAY-2003
    copyright            : (C) 2003 by The RoboCup Soccer Simulator 
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


#ifndef RCSSRESULTSAVER_HPP
#define RCSSRESULTSAVER_HPP

#include "rcssserverconfig.hpp"

#include <string>
#include <rcssbase/lib/factory.hpp>
#include <rcssbase/lib/shared_ptr.hpp>

struct tm;



namespace rcss
{
    class ResultSaver
    {
    public:
        typedef lib::shared_ptr< ResultSaver > Ptr;
        typedef Ptr(*Creator)();    
        typedef lib::Factory< Creator > Factory;

		RCSSSERVER_API
        static
        Factory&
        factory();

        enum team_id { TEAM_LEFT = 0, TEAM_RIGHT = 1 };

		RCSSSERVER_API
        ResultSaver()
        {}

		RCSSSERVER_API
        virtual
        ~ResultSaver()
        {}

		RCSSSERVER_API
        bool
        enabled() const
        { return doEnabled(); }

		RCSSSERVER_API
        void
        saveStart()
        { doSaveStart(); }

		RCSSSERVER_API
        void
        saveTime( const tm& time )
        { doSaveTime( time ); }

		RCSSSERVER_API
        void
        saveTeamName( team_id team, const std::string& name )
        { doSaveTeamName( team, name ); }

		RCSSSERVER_API
        void
        saveCoachName( team_id team, const std::string& name )
        { doSaveCoachName( team, name ); }

		RCSSSERVER_API
        void
        saveScore( team_id team, unsigned int score )
        { doSaveScore( team, score ); }

		RCSSSERVER_API
        void
        savePenTaken( team_id team, unsigned int taken )
        { doSavePenTaken( team, taken ); }

		RCSSSERVER_API
        void
        savePenScored( team_id team, unsigned int scored )
        { doSavePenScored( team, scored ); }

		RCSSSERVER_API
        void
        saveCoinTossWinner( team_id team )
        { doSaveCoinTossWinner( team ); }

		RCSSSERVER_API
        bool
        saveComplete()
        { return doSaveComplete(); }

		RCSSSERVER_API
		const char*
		getName() const
		{ return doGetName(); }
    private:

		RCSSSERVER_API
        virtual
        bool
        doEnabled() const
        { return false; }

		RCSSSERVER_API
        virtual
        void
        doSaveStart()
        {}

		RCSSSERVER_API
        virtual
        void
        doSaveTime( const tm& )
        {}

		RCSSSERVER_API
        virtual
        void
        doSaveTeamName( team_id, const std::string& )
        {}

		RCSSSERVER_API
        virtual
        void
        doSaveCoachName( team_id, const std::string& )
        {}

		RCSSSERVER_API
        virtual
        void
        doSaveScore( team_id, unsigned int )
        {}

		RCSSSERVER_API
        virtual
        void
        doSavePenTaken( team_id, unsigned int )
        {}

		RCSSSERVER_API
        virtual
        void
        doSavePenScored( team_id, unsigned int )
        {}

		RCSSSERVER_API
        virtual
        void
        doSaveCoinTossWinner( team_id )
        {}

		RCSSSERVER_API
        virtual
        bool
        doSaveComplete()
        { return true; }
	
		virtual
		const char*
		doGetName() const = 0;

    };
}


#endif
