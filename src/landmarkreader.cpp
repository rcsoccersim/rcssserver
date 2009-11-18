#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "landmarkreader.h"

#include "field.h"

const char LandmarkReader::FLAG_KW[] = "flag";
const char LandmarkReader::GOAL_KW[] = "goal";
const char LandmarkReader::MAINPITCH_KW[] = "mainPitch";
const char LandmarkReader::OUTERPITCH_KW[] = "outerPitch";
const char LandmarkReader::GOALMOUTH_KW[] = "goalMouth";
const char LandmarkReader::PENALTYAREA_KW[] = "penaltyArea";

const char LandmarkReader::X_KW[] = "x";
const char LandmarkReader::Y_KW[] = "y";
const char LandmarkReader::STARTX_KW[] = "startx";
const char LandmarkReader::STARTY_KW[] = "starty";
const char LandmarkReader::ENDX_KW[] = "endx";
const char LandmarkReader::ENDY_KW[] = "endy";
const char LandmarkReader::NAME_KW[] = "name";
const char LandmarkReader::VER_KW[] = "ver";
const char LandmarkReader::SIDE_KW[] = "side";

const char LandmarkReader::SIDE_L = 'l';
const char LandmarkReader::SIDE_R = 'r';

const LandmarkReader::flag_t LandmarkReader::DEF_FLAG( 0.0, 0.0, "", 8.0 );

// standard landmarks
// Be very carefull when changing the default values.  If you
// change from abs to percent or visa-versa, make sure you add
// or remove the % sign in the print function and the loadDefaults.
const LandmarkReader::rect_t LandmarkReader::DEF_PITCH = { -52.5, -34.0, 52.5, 34.0 };

const LandmarkReader::flag_t LandmarkReader::FLAG_C( 0.0, 0.0, "c", 3.0 );
const LandmarkReader::flag_t LandmarkReader::FLAG_C_T( 0.0, -100.0, "c t", 3.0 );
const LandmarkReader::flag_t LandmarkReader::FLAG_C_B( 0.0, 100.0, "c b", 3.0 );

const LandmarkReader::flag_t LandmarkReader::FLAG_R_T( 100.0, -100.0, "r t", 3.0 );
const LandmarkReader::flag_t LandmarkReader::FLAG_R_B( 100.0, 100.0, "r b", 3.0 );

const LandmarkReader::flag_t LandmarkReader::FLAG_L_T( -100.0, -100.0, "l t", 3.0 );
const LandmarkReader::flag_t LandmarkReader::FLAG_L_B( -100.0, 100.0, "l b", 3.0 );

const LandmarkReader::goal_mouth_t LandmarkReader::GOAL_MOUTH_R = { 'r', -7.01, 7.01 };
const LandmarkReader::goal_mouth_t LandmarkReader::GOAL_MOUTH_L = { 'l', -7.01, 7.01 };
const LandmarkReader::flag_t LandmarkReader::FLAG_G_R_B( 0.0, 100.0, "g r b", 3.0 );
const LandmarkReader::flag_t LandmarkReader::GOAL_R( 0.0, 0.0, "r", 3.0 );
const LandmarkReader::flag_t LandmarkReader::FLAG_G_R_T( 0.0, -100.0, "g r t", 3.0 );
const LandmarkReader::flag_t LandmarkReader::FLAG_G_L_B( 0.0, 100.0, "g l b", 3.0 );
const LandmarkReader::flag_t LandmarkReader::GOAL_L( 0.0, 0.0, "l", 3.0 );
const LandmarkReader::flag_t LandmarkReader::FLAG_G_L_T( 0.0, -100.0, "g l t", 3.0 );

const LandmarkReader::rect_t LandmarkReader::PEN_R = { 36.0, -20.16, 100, 20.16 };
// NOTE: the default endx for PEN_R is a % and the others are absoulute

const LandmarkReader::flag_t LandmarkReader::FLAG_P_R_B( -100.0, 100.0, "p r b", 3.0 );
const LandmarkReader::flag_t LandmarkReader::FLAG_P_R_C( -100.0, 0.0, "p r c", 3.0 );
const LandmarkReader::flag_t LandmarkReader::FLAG_P_R_T( -100.0, -100.0, "p r t", 3.0 );

const LandmarkReader::rect_t LandmarkReader::PEN_L = { -36.0, -20.16, -100, 20.16 };
// NOTE: the default endx for PEN_L is a % and the others are absoulute

const LandmarkReader::flag_t LandmarkReader::FLAG_P_L_B( -100.0, 100.0, "p l b", 3.0 );
const LandmarkReader::flag_t LandmarkReader::FLAG_P_L_C( -100.0, 0.0, "p l c", 3.0 );
const LandmarkReader::flag_t LandmarkReader::FLAG_P_L_T( -100.0, -100.0, "p l t", 3.0 );
const LandmarkReader::rect_t LandmarkReader::OUTER_PITCH = { -57.5, -39.0, 57.5, 39.0 };

// NOTE:  The top and bottom outer pitch flags have a abs x and a % y
const LandmarkReader::flag_t LandmarkReader::FLAG_T_0( 0.0, -100.0, "t 0", 4.0 );

const LandmarkReader::flag_t LandmarkReader::FLAG_T_R_10( 10.0, -100.0, "t r 10", 4.0 );
const LandmarkReader::flag_t LandmarkReader::FLAG_T_R_20( 20.0, -100.0, "t r 20", 4.0 );
const LandmarkReader::flag_t LandmarkReader::FLAG_T_R_30( 30.0, -100.0, "t r 30", 4.0 );
const LandmarkReader::flag_t LandmarkReader::FLAG_T_R_40( 40.0, -100.0, "t r 40", 4.0 );
const LandmarkReader::flag_t LandmarkReader::FLAG_T_R_50( 50.0, -100.0, "t r 50", 4.0 );

const LandmarkReader::flag_t LandmarkReader::FLAG_T_L_10( -10.0, -100.0, "t l 10", 4.0 );
const LandmarkReader::flag_t LandmarkReader::FLAG_T_L_20( -20.0, -100.0, "t l 20", 4.0 );
const LandmarkReader::flag_t LandmarkReader::FLAG_T_L_30( -30.0, -100.0, "t l 30", 4.0 );
const LandmarkReader::flag_t LandmarkReader::FLAG_T_L_40( -40.0, -100.0, "t l 40", 4.0 );
const LandmarkReader::flag_t LandmarkReader::FLAG_T_L_50( -50.0, -100.0, "t l 50", 4.0 );

const LandmarkReader::flag_t LandmarkReader::FLAG_B_0( 0.0, 100.0, "b 0", 4.0 );

const LandmarkReader::flag_t LandmarkReader::FLAG_B_R_10( 10.0, 100.0, "b r 10", 4.0 );
const LandmarkReader::flag_t LandmarkReader::FLAG_B_R_20( 20.0, 100.0, "b r 20", 4.0 );
const LandmarkReader::flag_t LandmarkReader::FLAG_B_R_30( 30.0, 100.0, "b r 30", 4.0 );
const LandmarkReader::flag_t LandmarkReader::FLAG_B_R_40( 40.0, 100.0, "b r 40", 4.0 );
const LandmarkReader::flag_t LandmarkReader::FLAG_B_R_50( 50.0, 100.0, "b r 50", 4.0 );

const LandmarkReader::flag_t LandmarkReader::FLAG_B_L_10( -10.0, 100.0, "b l 10", 4.0 );
const LandmarkReader::flag_t LandmarkReader::FLAG_B_L_20( -20.0, 100.0, "b l 20", 4.0 );
const LandmarkReader::flag_t LandmarkReader::FLAG_B_L_30( -30.0, 100.0, "b l 30", 4.0 );
const LandmarkReader::flag_t LandmarkReader::FLAG_B_L_40( -40.0, 100.0, "b l 40", 4.0 );
const LandmarkReader::flag_t LandmarkReader::FLAG_B_L_50( -50.0, 100.0, "b l 50", 4.0 );

// NOTE:  The left and right outer pitch flags have a % x and a abs y
const LandmarkReader::flag_t LandmarkReader::FLAG_R_0( 100.0, 0.0, "r 0", 4.0 );

const LandmarkReader::flag_t LandmarkReader::FLAG_R_T_10( 100.0, -10.0, "r t 10", 4.0 );
const LandmarkReader::flag_t LandmarkReader::FLAG_R_T_20( 100.0, -20.0, "r t 20", 4.0 );
const LandmarkReader::flag_t LandmarkReader::FLAG_R_T_30( 100.0, -30.0, "r t 30", 4.0 );
const LandmarkReader::flag_t LandmarkReader::FLAG_R_B_10( 100.0, 10.0, "r b 10", 4.0 );
const LandmarkReader::flag_t LandmarkReader::FLAG_R_B_20( 100.0, 20.0, "r b 20", 4.0 );
const LandmarkReader::flag_t LandmarkReader::FLAG_R_B_30( 100.0, 30.0, "r b 30", 4.0 );

const LandmarkReader::flag_t LandmarkReader::FLAG_L_0( -100.0, 0.0, "l 0", 4.0 );

const LandmarkReader::flag_t LandmarkReader::FLAG_L_T_10( -100.0, -10.0, "l t 10", 4.0 );
const LandmarkReader::flag_t LandmarkReader::FLAG_L_T_20( -100.0, -20.0, "l t 20", 4.0 );
const LandmarkReader::flag_t LandmarkReader::FLAG_L_T_30( -100.0, -30.0, "l t 30", 4.0 );
const LandmarkReader::flag_t LandmarkReader::FLAG_L_B_10( -100.0, 10.0, "l b 10", 4.0 );
const LandmarkReader::flag_t LandmarkReader::FLAG_L_B_20( -100.0, 20.0, "l b 20", 4.0 );
const LandmarkReader::flag_t LandmarkReader::FLAG_L_B_30( -100.0, 30.0, "l b 30", 4.0 );

LandmarkReader::LandmarkReader( Field & field,
                                const std::string &
#ifdef HAVE_LIBEXPAT
                                path_name
#endif
                                )
    : M_field( field )
{
#ifdef HAVE_LIBEXPAT
    if ( ! readXml( path_name ) )
    {
        std::cerr << "trying to create default landmark file...";

        if ( createFile( path_name ) )
        {
            std::cerr << "\t...Created\n";
        }
        else
        {
            std::cerr << "\t...Failed\n";
        }
        loadDefaults();
    }
#else // !HAVE_LIBEXPAT
    loadDefaults();
#endif // !HAVE_LIBEXPAT
}

#ifdef HAVE_LIBEXPAT
void
LandmarkReader::start( const char * el,
                       const char ** attr )
{
    if ( std::string( el ) == LandmarkReader::FLAG_KW )
    {
        processFlag( attr );
    }
    else if ( std::string( el ) == LandmarkReader::GOAL_KW )
    {
        processFlag( attr, true );
    }
    else if ( std::string( el ) == LandmarkReader::MAINPITCH_KW )
    {
        processPitch( attr );
    }
    else if ( std::string( el ) == LandmarkReader::PENALTYAREA_KW )
    {
        processPitch( attr, false );
    }
    else if ( std::string( el ) == LandmarkReader::GOALMOUTH_KW )
    {
        processGoalMouth( attr );
    }
    else if ( std::string( el ) == LandmarkReader::OUTERPITCH_KW )
    {
        processPitch( attr, false );
    }
    else
    {
        std::cerr << path << ": " << line
                  << ": unknown tag " << el << std::endl;
    }
}

void
LandmarkReader::end( const char * el )
{
    if ( std::string( el ) == LandmarkReader::MAINPITCH_KW
         || std::string( el ) == LandmarkReader::PENALTYAREA_KW
         || std::string( el ) == LandmarkReader::GOALMOUTH_KW )
    {
        rect_stack.pop();
    }
}

bool
LandmarkReader::processGoalMouth( const char ** attr )
{
    if ( rect_stack.size() == 0 )
    {
        return false;
    }

    LandmarkReader::goal_mouth_t goal_mouth;
    LandmarkReader::rect_t rect;

    bool side_found = false;

    for( int i = 0; attr[i]; i += 2 )
    {
        if ( std::string( attr[i] ) == LandmarkReader::SIDE_KW )
        {
            if ( ! extract( attr[i], attr[i+1], goal_mouth.side ) )
            {
                return false;
            }

            if ( goal_mouth.side == LandmarkReader::SIDE_L )
            {
                rect.startx = rect.endx = rect_stack.top().startx;
                side_found = true;
                goal_mouth.side = LandmarkReader::SIDE_L;
            }
            else if ( goal_mouth.side == LandmarkReader::SIDE_R )
            {
                rect.startx = rect.endx = rect_stack.top().endx;
                side_found = true;
                goal_mouth.side = LandmarkReader::SIDE_R;
            }
            else
            {
                // error
                std::cerr << path << ": " << line
                          << ": Cannot convert " << attr[i]
                          << " attribute from " << attr[i+1] << std::endl;
                return false;
            }
        }
        else if ( std::string( attr[i] ) == LandmarkReader::STARTY_KW )
        {
            if ( rect_stack.size() == 0 )
            {
                if ( ! extract( attr[i], attr[i+1], rect.starty ) )
                {
                    return false;
                }
            }
            else
            {
                if ( ! extract( attr[i], attr[i+1], rect.starty,
                                rect_stack.top().starty, rect_stack.top().endy ) )
                {
                    return false;
                }
            }
        }
        else if ( std::string( attr[i] ) == LandmarkReader::ENDY_KW )
        {
            if ( rect_stack.size() == 0 )
            {
                if ( ! extract( attr[i], attr[i+1], rect.endy ) )
                {
                    return false;
                }
            }
            else
            {
                if ( ! extract( attr[i], attr[i+1], rect.endy,
                                rect_stack.top().starty, rect_stack.top().endy ) )
                {
                    return false;
                }
            }
        }
        else
        {
            // error
            std::cerr << path << ": " << line
                      << ": unknown attribute " << attr[i] << std::endl;
            return false;
        }
    }

    if ( ! side_found )
    {
        // error
        std::cerr << path << ": " << line
                  << ": goalMouth must specify a side\n" << std::endl;
        return false;
    }

    rect_stack.push( rect );

    if ( goal_mouth.side == LandmarkReader::SIDE_L )
    {
        goal_width_l = std::fabs( goal_mouth.starty - goal_mouth.endy );
    }
    else
    {
        goal_width_r = std::fabs( goal_mouth.starty - goal_mouth.endy );
    }

    return true;
}

bool
LandmarkReader::processPitch( const char ** attr,
                              bool main )
{
    LandmarkReader::rect_t rect = LandmarkReader::DEF_PITCH;

    for ( int i = 0; attr[i]; i += 2 )
    {
        if ( std::string( attr[i] ) == LandmarkReader::STARTX_KW )
        {
            if ( rect_stack.size() == 0 )
            {
                if ( ! extract( attr[i], attr[i+1], rect.startx ) )
                {
                    return false;
                }
            }
            else
            {
                if ( ! extract( attr[i], attr[i+1], rect.startx,
                                rect_stack.top().startx, rect_stack.top().endx ) )
                {
                    return false;
                }
            }
        }
        else if ( std::string( attr[i] ) == LandmarkReader::STARTY_KW )
        {
            if ( rect_stack.size() == 0 )
            {
                if ( ! extract( attr[i], attr[i+1], rect.starty ) )
                {
                    return false;
                }
            }
            else
            {
                if ( ! extract( attr[i], attr[i+1], rect.starty,
                                rect_stack.top().starty, rect_stack.top().endy ) )
                {
                    return false;
                }
            }
        }
        else if ( std::string( attr[i] ) == LandmarkReader::ENDX_KW )
        {
            if ( rect_stack.size() == 0 )
            {
                if ( ! extract( attr[i], attr[i+1], rect.endx ) )
                {
                    return false;
                }
            }
            else
            {
                if ( ! extract( attr[i], attr[i+1], rect.endx,
                                rect_stack.top().startx, rect_stack.top().endx ) )
                {
                    return false;
                }
            }
        }
        else if ( std::string( attr[i] ) == LandmarkReader::ENDY_KW )
        {
            if ( rect_stack.size() == 0 )
            {
                if ( ! extract( attr[i], attr[i+1], rect.endy ) )
                {
                    return false;
                }
            }
            else
            {
                if ( ! extract( attr[i], attr[i+1], rect.endy,
                                rect_stack.top().starty, rect_stack.top().endy ) )
                {
                    return false;
                }
            }
        }
        else
        {
            // error
            std::cerr << path << ": " << line
                      << ": unknown attribute " << attr[i] << std::endl;
            return false;
        }
    }
    rect_stack.push(rect);

    if ( main )
    {
        pitch = rect;
    }
    return true;
}

bool
LandmarkReader::processFlag( const char **attr,
                             bool goal )
{
    LandmarkReader::flag_t flag = LandmarkReader::DEF_FLAG;
    if ( rect_stack.size() > 0 )
    {
        flag.x =( rect_stack.top().startx + rect_stack.top().endx ) * 0.5;
        flag.y =( rect_stack.top().starty + rect_stack.top().endy ) * 0.5;
    }

    for(int i = 0; attr[i]; i += 2)
    {
        if ( std::string( attr[i] ) == LandmarkReader::X_KW )
        {
            if ( rect_stack.size() == 0 )
            {
                if ( ! extract( attr[i], attr[i+1], flag.x ) )
                {
                    return false;
                }
            }
            else
            {
                if ( ! extract( attr[i], attr[i+1], flag.x,
                                rect_stack.top().startx, rect_stack.top().endx ) )
                {
                    return false;
                }
            }
        }
        else if ( std::string( attr[i] ) == LandmarkReader::Y_KW )
        {
            if ( rect_stack.size() == 0 )
            {
                if ( ! extract( attr[i], attr[i+1], flag.y ) )
                {
                    return false;
                }
            }
            else
            {
                if ( ! extract( attr[i], attr[i+1], flag.y,
                                rect_stack.top().starty, rect_stack.top().endy ) )
                {
                    return false;
                }
            }
        }
        else if ( std::string( attr[i] ) == LandmarkReader::NAME_KW )
        {
            if ( ! extract( attr[i], attr[i+1], flag.name ) )
            {
                return false;
            }
        }
        else if ( std::string( attr[i] ) == LandmarkReader::VER_KW )
        {
            if ( ! extract( attr[i], attr[i+1], flag.ver ) )
            {
                return false;
            }
        }
        else
        {
            std::cerr << path << ": " << line
                      << ": Unknown attribute " << attr[i] << std::endl;
            return false;
        }
    }

    addFlag( flag, goal );
}
#endif // HAVE_LIBEXPAT

// Be very carefull when changing the default values.  If you
// change from abs to percent or visa-versa, make sure you add
// or remove the % sign in the print function and the loadDefaults.
std::ostream &
LandmarkReader::print( std::ostream & o ) const
{
    o << "<?xml version=\"1.0\"?>" << std::endl;
    o << "" << std::endl;
    o << "" << std::endl;
    o << "<mainPitch startx=\"" << LandmarkReader::DEF_PITCH.startx
      << "\" starty=\"" << LandmarkReader::DEF_PITCH.starty
      << "\" endx=\"" << LandmarkReader::DEF_PITCH.endx
      << "\" endy=\"" << LandmarkReader::DEF_PITCH.endy
      << "\">" << std::endl;
    LandmarkReader::FLAG_C.toXml( o << "\t" ) << std::endl;
    LandmarkReader::FLAG_C_T.toXml( o << "\t" ) << std::endl;
    LandmarkReader::FLAG_C_B.toXml( o << "\t" ) << std::endl;
    LandmarkReader::FLAG_R_T.toXml( o << "\t" ) << std::endl;
    LandmarkReader::FLAG_R_B.toXml( o << "\t" ) << std::endl;
    LandmarkReader::FLAG_L_T.toXml( o << "\t" ) << std::endl;
    LandmarkReader::FLAG_L_B.toXml( o << "\t" ) << std::endl;
    LandmarkReader::GOAL_MOUTH_R.toXml( o << "\t" ) << std::endl;
    LandmarkReader::FLAG_G_R_B.toXml( o << "\t\t" ) << std::endl;
    o << "\t\t<goal y=\"" << LandmarkReader::GOAL_R.y
      << "%\" name=\"" << LandmarkReader::GOAL_R.name
      << "\" ver=\"" << LandmarkReader::GOAL_R.ver
      << "\"/>" << std::endl;
    LandmarkReader::FLAG_G_R_T.toXml( o << "\t\t" ) << std::endl;
    o << "\t</goalMouth>" << std::endl;
    LandmarkReader::GOAL_MOUTH_L.toXml( o << "\t" ) << std::endl;
    LandmarkReader::FLAG_G_L_B.toXml( o << "\t\t" ) << std::endl;
    o << "\t\t<goal y=\"" << LandmarkReader::GOAL_L.y
      << "%\" name=\"" << LandmarkReader::GOAL_L.name
      << "\" ver=\"" << LandmarkReader::GOAL_L.ver
      << "\"/>" << std::endl;
    LandmarkReader::FLAG_G_L_T.toXml( o << "\t\t" ) << std::endl;
    o << "\t</goalMouth>" << std::endl;
    o << "\t<penaltyArea startx=\"" <<  LandmarkReader::PEN_R.startx
      << "\" starty=\"" <<  LandmarkReader::PEN_R.starty
      << "\" endx=\"" <<  LandmarkReader::PEN_R.endx << "%\" "
      << "endy=\"" <<  LandmarkReader::PEN_R.endy
      << "\">" << std::endl;
    LandmarkReader::FLAG_P_R_B.toXml( o << "\t\t" ) << std::endl;
    LandmarkReader::FLAG_P_R_C.toXml( o << "\t\t" ) << std::endl;
    LandmarkReader::FLAG_P_R_T.toXml( o << "\t\t" ) << std::endl;
    o << "\t</penaltyArea>" << std::endl;
    o << "\t<penaltyArea startx=\"" << LandmarkReader::PEN_L.startx
      << "\" starty=\"" << LandmarkReader::PEN_L.starty
      << "\" endx=\"" << LandmarkReader::PEN_L.endx << "%\" "
      << "endy=\"" << LandmarkReader::PEN_L.endy
      << "\">" << std::endl;
    LandmarkReader::FLAG_P_L_B.toXml( o << "\t\t" ) << std::endl;
    LandmarkReader::FLAG_P_L_C.toXml( o << "\t\t" ) << std::endl;
    LandmarkReader::FLAG_P_L_T.toXml( o << "\t\t" ) << std::endl;
    o << "\t</penaltyArea>" << std::endl;
    o << "\t<outerPitch startx=\"" <<  LandmarkReader::OUTER_PITCH.startx
      << "\" starty=\"" <<  LandmarkReader::OUTER_PITCH.starty
      << "\" endx=\"" <<  LandmarkReader::OUTER_PITCH.endx
      << "\" endy=\"" <<  LandmarkReader::OUTER_PITCH.endy
      << "\">" << std::endl;
    LandmarkReader::FLAG_T_0.toXml( o << "\t\t", false, true ) << std::endl;
    LandmarkReader::FLAG_T_R_10.toXml( o << "\t\t", false, true ) << std::endl;
    LandmarkReader::FLAG_T_R_20.toXml( o << "\t\t", false, true ) << std::endl;
    LandmarkReader::FLAG_T_R_30.toXml( o << "\t\t", false, true ) << std::endl;
    LandmarkReader::FLAG_T_R_40.toXml( o << "\t\t", false, true ) << std::endl;
    LandmarkReader::FLAG_T_R_50.toXml( o << "\t\t", false, true ) << std::endl;
    LandmarkReader::FLAG_T_L_10.toXml( o << "\t\t", false, true ) << std::endl;
    LandmarkReader::FLAG_T_L_20.toXml( o << "\t\t", false, true ) << std::endl;
    LandmarkReader::FLAG_T_L_30.toXml( o << "\t\t", false, true ) << std::endl;
    LandmarkReader::FLAG_T_L_40.toXml( o << "\t\t", false, true ) << std::endl;
    LandmarkReader::FLAG_T_L_50.toXml( o << "\t\t", false, true ) << std::endl;
    LandmarkReader::FLAG_B_0.toXml( o << "\t\t", false, true ) << std::endl;
    LandmarkReader::FLAG_B_R_10.toXml( o << "\t\t", false, true ) << std::endl;
    LandmarkReader::FLAG_B_R_20.toXml( o << "\t\t", false, true ) << std::endl;
    LandmarkReader::FLAG_B_R_30.toXml( o << "\t\t", false, true ) << std::endl;
    LandmarkReader::FLAG_B_R_40.toXml( o << "\t\t", false, true ) << std::endl;
    LandmarkReader::FLAG_B_R_50.toXml( o << "\t\t", false, true ) << std::endl;
    LandmarkReader::FLAG_B_L_10.toXml( o << "\t\t", false, true ) << std::endl;
    LandmarkReader::FLAG_B_L_20.toXml( o << "\t\t", false, true ) << std::endl;
    LandmarkReader::FLAG_B_L_30.toXml( o << "\t\t", false, true ) << std::endl;
    LandmarkReader::FLAG_B_L_40.toXml( o << "\t\t", false, true ) << std::endl;
    LandmarkReader::FLAG_B_L_50.toXml( o << "\t\t", false, true ) << std::endl;
    LandmarkReader::FLAG_R_0.toXml( o << "\t\t", true, false ) << std::endl;
    LandmarkReader::FLAG_R_T_10.toXml( o << "\t\t", true, false ) << std::endl;
    LandmarkReader::FLAG_R_T_20.toXml( o << "\t\t", true, false ) << std::endl;
    LandmarkReader::FLAG_R_T_30.toXml( o << "\t\t", true, false ) << std::endl;
    LandmarkReader::FLAG_R_B_10.toXml( o << "\t\t", true, false ) << std::endl;
    LandmarkReader::FLAG_R_B_20.toXml( o << "\t\t", true, false ) << std::endl;
    LandmarkReader::FLAG_R_B_30.toXml( o << "\t\t", true, false ) << std::endl;
    LandmarkReader::FLAG_L_0.toXml( o << "\t\t", true, false ) << std::endl;
    LandmarkReader::FLAG_L_T_10.toXml( o << "\t\t", true, false ) << std::endl;
    LandmarkReader::FLAG_L_T_20.toXml( o << "\t\t", true, false ) << std::endl;
    LandmarkReader::FLAG_L_T_30.toXml( o << "\t\t", true, false ) << std::endl;
    LandmarkReader::FLAG_L_B_10.toXml( o << "\t\t", true, false ) << std::endl;
    LandmarkReader::FLAG_L_B_20.toXml( o << "\t\t", true, false ) << std::endl;
    LandmarkReader::FLAG_L_B_30.toXml( o << "\t\t", true, false ) << std::endl;
    o << "\t</outerPitch>" << std::endl;
    o << "</mainPitch>" << std::endl;
    return o;
}

// Be very carefull when changing the default values.  If you
// change from abs to percent or visa-versa, make sure you add
// or remove the % sign in the print function and the loadDefaults.
void
LandmarkReader::loadDefaults()
{
    LandmarkReader::flag_t tmp_flag;

    rect_stack.push( LandmarkReader::DEF_PITCH );

    addFlag( LandmarkReader::FLAG_C.percentify( rect_stack.top(),
                                                true, true ) );
    addFlag( LandmarkReader::FLAG_C_T.percentify( rect_stack.top(),
                                                  true, true ) );
    addFlag( LandmarkReader::FLAG_C_B.percentify( rect_stack.top(),
                                                  true, true ) );
    addFlag( LandmarkReader::FLAG_R_T.percentify( rect_stack.top(),
                                                  true, true ) );
    addFlag( LandmarkReader::FLAG_R_B.percentify( rect_stack.top(),
                                                  true, true ) );
    addFlag( LandmarkReader::FLAG_L_T.percentify( rect_stack.top(),
                                                  true, true ) );
    addFlag( LandmarkReader::FLAG_L_B.percentify( rect_stack.top(),
                                                  true, true ) );
    rect_stack.push( LandmarkReader::GOAL_MOUTH_R.toRect( rect_stack.top() ) );
    addFlag( LandmarkReader::FLAG_G_R_B.percentify( rect_stack.top(),
                                                    true, true ) );
    addFlag( LandmarkReader::GOAL_R.percentify( rect_stack.top(),
                                                true, true ), true );
    addFlag( LandmarkReader::FLAG_G_R_T.percentify( rect_stack.top(),
                                                    true, true ) );
    rect_stack.pop();
    rect_stack.push( LandmarkReader::GOAL_MOUTH_L.toRect( rect_stack.top() ) );
    addFlag( LandmarkReader::FLAG_G_L_B.percentify( rect_stack.top(),
                                                    true, true ) );
    addFlag( LandmarkReader::GOAL_L.percentify( rect_stack.top(),
                                                true, true ), true );
    addFlag( LandmarkReader::FLAG_G_L_T.percentify( rect_stack.top(),
                                                    true, true ) );
    rect_stack.pop();
    rect_stack.push( LandmarkReader::PEN_R.percentify( rect_stack.top(),
                                                       false, false,
                                                       true, false ) );

    addFlag( LandmarkReader::FLAG_P_R_B.percentify( rect_stack.top(),
                                                    true, true ) );
    addFlag( LandmarkReader::FLAG_P_R_C.percentify( rect_stack.top(),
                                                    true, true ) );
    addFlag( LandmarkReader::FLAG_P_R_T.percentify( rect_stack.top(),
                                                    true, true ) );
    rect_stack.pop();
    rect_stack.push( LandmarkReader::PEN_L.percentify( rect_stack.top(),
                                                       false, false,
                                                       true, false ) );
    addFlag( LandmarkReader::FLAG_P_L_B.percentify( rect_stack.top(),
                                                    true, true ) );
    addFlag( LandmarkReader::FLAG_P_L_C.percentify( rect_stack.top(),
                                                    true, true ) );
    addFlag( LandmarkReader::FLAG_P_L_T.percentify( rect_stack.top(),
                                                    true, true ) );
    rect_stack.pop();
    rect_stack.push( LandmarkReader::OUTER_PITCH );
    addFlag( LandmarkReader::FLAG_T_0.percentify( rect_stack.top(),
                                                  false, true ) );
    addFlag( LandmarkReader::FLAG_T_R_10.percentify( rect_stack.top(),
                                                     false, true ) );
    addFlag( LandmarkReader::FLAG_T_R_20.percentify( rect_stack.top(),
                                                     false, true ) );
    addFlag( LandmarkReader::FLAG_T_R_30.percentify( rect_stack.top(),
                                                     false, true ) );
    addFlag( LandmarkReader::FLAG_T_R_40.percentify( rect_stack.top(),
                                                     false, true ) );
    addFlag( LandmarkReader::FLAG_T_R_50.percentify( rect_stack.top(),
                                                     false, true ) );
    addFlag( LandmarkReader::FLAG_T_L_10.percentify( rect_stack.top(),
                                                     false, true ) );
    addFlag( LandmarkReader::FLAG_T_L_20.percentify( rect_stack.top(),
                                                     false, true ) );
    addFlag( LandmarkReader::FLAG_T_L_30.percentify( rect_stack.top(),
                                                     false, true ) );
    addFlag( LandmarkReader::FLAG_T_L_40.percentify( rect_stack.top(),
                                                     false, true ) );
    addFlag( LandmarkReader::FLAG_T_L_50.percentify( rect_stack.top(),
                                                     false, true ) );
    addFlag( LandmarkReader::FLAG_B_0.percentify( rect_stack.top(),
                                                  false, true ) );
    addFlag( LandmarkReader::FLAG_B_R_10.percentify( rect_stack.top(),
                                                     false, true ) );
    addFlag( LandmarkReader::FLAG_B_R_20.percentify( rect_stack.top(),
                                                     false, true ) );
    addFlag( LandmarkReader::FLAG_B_R_30.percentify( rect_stack.top(),
                                                     false, true ) );
    addFlag( LandmarkReader::FLAG_B_R_40.percentify( rect_stack.top(),
                                                     false, true ) );
    addFlag( LandmarkReader::FLAG_B_R_50.percentify( rect_stack.top(),
                                                     false, true ) );
    addFlag( LandmarkReader::FLAG_B_L_10.percentify( rect_stack.top(),
                                                     false, true ) );
    addFlag( LandmarkReader::FLAG_B_L_20.percentify( rect_stack.top(),
                                                     false, true ) );
    addFlag( LandmarkReader::FLAG_B_L_30.percentify( rect_stack.top(),
                                                     false, true ) );
    addFlag( LandmarkReader::FLAG_B_L_40.percentify( rect_stack.top(),
                                                     false, true ) );
    addFlag( LandmarkReader::FLAG_B_L_50.percentify( rect_stack.top(),
                                                     false, true ) );
    addFlag( LandmarkReader::FLAG_R_0.percentify( rect_stack.top(),
                                                  true, false ) );
    addFlag( LandmarkReader::FLAG_R_T_10.percentify( rect_stack.top(),
                                                     true, false ) );
    addFlag( LandmarkReader::FLAG_R_T_20.percentify( rect_stack.top(),
                                                     true, false ) );
    addFlag( LandmarkReader::FLAG_R_T_30.percentify( rect_stack.top(),
                                                     true, false ) );
    addFlag( LandmarkReader::FLAG_R_B_10.percentify( rect_stack.top(),
                                                     true, false ) );
    addFlag( LandmarkReader::FLAG_R_B_20.percentify( rect_stack.top(),
                                                     true, false ) );
    addFlag( LandmarkReader::FLAG_R_B_30.percentify( rect_stack.top(),
                                                     true, false ) );
    addFlag( LandmarkReader::FLAG_L_0.percentify( rect_stack.top(),
                                                  true, false ) );
    addFlag( LandmarkReader::FLAG_L_T_10.percentify( rect_stack.top(),
                                                     true, false ) );
    addFlag( LandmarkReader::FLAG_L_T_20.percentify( rect_stack.top(),
                                                     true, false ) );
    addFlag( LandmarkReader::FLAG_L_T_30.percentify( rect_stack.top(),
                                                     true, false ) );
    addFlag( LandmarkReader::FLAG_L_B_10.percentify( rect_stack.top(),
                                                     true, false ) );
    addFlag( LandmarkReader::FLAG_L_B_20.percentify( rect_stack.top(),
                                                     true, false ) );
    addFlag( LandmarkReader::FLAG_L_B_30.percentify( rect_stack.top(),
                                                     true, false ) );
    rect_stack.pop();
    rect_stack.pop();
}


void
LandmarkReader::addFlag( const LandmarkReader::flag_t & flag,
                         const bool goal )
{
    // add goal to stadium
    std::string sname;
    std::string lname;
    if ( goal )
    {
        sname = std::string( "(g " ) + flag.name + ")";
        lname = std::string( "(goal " ) + flag.name + ")";
        M_field.addLandmark( new PObject( lname, sname,
                                          O_TYPE_GOAL_NAME,
                                          O_TYPE_GOAL_NAME_SHORT,
                                          PVector( flag.x, flag.y ),
                                          flag.ver ) ) ;
    }
    else
    {
        sname = std::string( "(f " ) + flag.name + ")";
        lname = std::string( "(flag " ) + flag.name + ")";
        M_field.addLandmark( new PObject( lname, sname,
                                          O_TYPE_FLAG_NAME,
                                          O_TYPE_FLAG_NAME_SHORT,
                                          PVector( flag.x, flag.y ),
                                          flag.ver ) ) ;
    }
}
