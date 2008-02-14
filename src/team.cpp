

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "team.h"

#include "field.h"
#include "param.h"
#include "player.h"

Team::Team( Stadium * stad, const Side s )
    : M_stadium( stad ),
      M_name( "" ),
      M_side( s ),
      M_size( 0 ),
      M_point( 0 ),
      M_pen_taken( 0 ),
      M_pen_point( 0 ),
      M_pen_won( false ),
      M_subs_count( 0 ),
      M_olcoach( static_cast< OnlineCoach * >( 0 ) )
{
    for ( int i = 0; i < PlayerParam::instance().playerTypes(); ++i )
    {
        M_ptype_count[ i ] = 0;
        M_ptype_used_count[ i ] = 0;
    }

#ifdef NEW_QSTEP
    if ( side() == LEFT )
    {
        M_dist_qstep_team = ServerParam::instance().dist_qstep_l;
        M_land_qstep_team = ServerParam::instance().land_qstep_l;
        M_dir_qstep_team  = ServerParam::instance().dir_qstep_l;
    }
    else
    {
        dist_qstep_team = ServerParam::instance().dist_qstep_r;
        land_qstep_team = ServerParam::instance().land_qstep_r;
        dir_qstep_team  = ServerParam::instance().dir_qstep_r;
    }
#endif
    // pfr 8/14/00: for RC2000 evaluation
    if ( ServerParam::instance().teamActuatorNoise() )
    {
        if ( side() == LEFT )
        {
            M_prand_factor_team     = ServerParam::instance().playerRandFactorLeft();
            M_kick_rand_factor_team = ServerParam::instance().kickRandFactorLeft();
        }
        else
        {
            M_prand_factor_team     = ServerParam::instance().playerRandFactorRight();
            M_kick_rand_factor_team = ServerParam::instance().kickRandFactorRight();
        }
    }

    for ( int i = 0; i < MAX_PLAYER; ++i )
    {
        M_players[i] = static_cast< Player * >( 0 );
    }
}

Team::~Team()
{
    for ( GraphCont::iterator i = M_graphics.begin(); i != M_graphics.end(); ++i )
    {
        delete i->second;
    }

}

Player *
Team::newPlayer( const double & version,
                 const bool goalie_flag )
{
    if ( size() >= MAX_PLAYER )
    {
        if ( ServerParam::instance().verboseMode() )
            std::cerr << "Warning:Too many players." << std::endl;
        return NULL;
    }

    if ( goalie_flag )
    {
        for ( int i = 0; i < size(); ++i )
        {
            if ( M_players[i]->isGoalie() )
            {
                if ( ServerParam::instance().verboseMode() )
                    std::cerr << "Warning:Too many goalies." << std::endl;
                return NULL;
            }
        }
    }

    Player * p = M_players[size()];

    if ( ! p->init( version, goalie_flag ) )
    {
        return NULL;
    }

    ++M_size;

#if 1
    ++M_ptype_count[ 0 ];
    ++M_ptype_used_count[ 0 ];
#else
    if ( goalie_flag
         || PlayerParam::instance().allowMultDefaultType() )
    {
        ++M_ptype_count[ 0 ];
        ++M_ptype_used_count[ 0 ];
    }
    else
    {
        bool substituted = false;
        for ( int type = 1; type < PlayerParam::instance().playerTypes(); ++type )
        {
            if ( M_ptype_used_count[type] >= PlayerParam::instance().ptMax() )
            {
                continue;
            }

            substituted = true;
            p->substitute( type );
            ++M_ptype_count[ type ];
            ++M_ptype_used_count[ type ];

            M_stadium->broadcastSubstitution( M_side, p->unum(), type );
            break;
        }

        if ( ! substituted )
        {
            ++M_ptype_count[ 0 ];
            ++M_ptype_used_count[ 0 ];
        }
    }
#endif

    return p;
}

void
Team::assignPlayer( int n, Player * player )
{
    if ( n < 0 || MAX_PLAYER <= n )
    {
        std::cerr << "Error: Too many players are assigned." << std::endl;
        return;
    }
    M_players[n] = player;
}

void
Team::assignCoach( OnlineCoach * coach )
{
    M_olcoach = coach;
}

void
Team::assignPlayerTypes()
{
    if ( PlayerParam::instance().allowMultDefaultType()
         || ( ServerParam::instance().nrNormalHalfs() == 0
              && ServerParam::instance().nrExtraHalfs() == 0 ) )
    {
        return;
    }

    for ( int i = 0; i < size(); ++i )
    {
        Player * p = M_players[i];

        if ( p->isGoalie() )
        {
            continue;
        }

        int old_type = p->playerTypeId();

        if ( M_ptype_used_count[ old_type ] <= PlayerParam::instance().ptMax() )
        {
            continue;
        }

        for ( int type = 0; type < PlayerParam::instance().playerTypes(); ++type )
        {
            if ( M_ptype_used_count[ type ] >= PlayerParam::instance().ptMax() )
            {
                continue;
            }

            p->substitute( type );

            M_ptype_count[ type ] += 1;
            M_ptype_used_count[ type ] += 1;

            M_ptype_count[ old_type ] -= 1;
            M_ptype_used_count[ old_type ] -= 1;

            std::cout << "rcssserver changes (player "
                      << M_name << " " << p->unum()
                      << ") to type " << type << std::endl;

            M_stadium->broadcastSubstitution( M_side, p->unum(), type );
            break;
        }
    }
}

int
Team::ptypeCount( const int player_type ) const
{
    std::map< int, int >::const_iterator it = M_ptype_count.find( player_type );
    if ( it == M_ptype_count.end() )
    {
        return 0;
    }
    return it->second;
}


void
Team::substitute( const Player * player,
                  const int player_type )
{
    if ( ! player )
    {
        return;
    }

    std::map< int, int >::iterator old_it = M_ptype_count.find( player->playerTypeId() );
    std::map< int, int >::iterator new_it = M_ptype_count.find( player_type );

    if ( old_it == M_ptype_count.end()
         || new_it == M_ptype_count.end() )
    {
        return;
    }

    if ( old_it->second <= 0 )
    {
        return;
    }

    Player * candidate = NULL;
    for ( int i = 0; i < this->size(); ++i )
    {
        if ( player == M_players[i] )
        {
            candidate = M_players[i];
            break;
        }
    }

    if ( ! candidate )
    {
        std::cerr << __FILE__ << ":" << __LINE__
                  << " Warning: no such player." << std::endl;
        return;
    }

    candidate->substitute( player_type );

    old_it->second -= 1;
    new_it->second += 1;

    if ( M_stadium->playmode() != PM_BeforeKickOff )
    {
        ++M_subs_count;
    }

    if ( M_stadium->time() <= 0 ) // before kick off
    {
        for ( std::map< int, int >::iterator it = M_ptype_used_count.begin();
              it != M_ptype_used_count.end();
              ++it )
        {
            it->second = 0;
        }

        for ( int i = 0; i < this->size(); ++i )
        {
            M_ptype_used_count[ M_players[i]->playerTypeId() ] += 1;
        }
    }
    else
    {
        M_ptype_used_count[player_type] += 1;
    }
}


void
Team::addGraphic( unsigned int x,
                  unsigned int y,
                  std::auto_ptr< XPMHolder > holder )
{
    GraphKey key( x, y );
    GraphCont::iterator i = M_graphics.find( key );
    if( i != M_graphics.end() )
    {
        delete i->second;
    }

    M_graphics[ key ] = holder.release();
}
