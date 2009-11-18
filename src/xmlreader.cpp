#include "xmlreader.h"

#ifdef HAVE_LIBEXPAT

#include "utility.h"

#include <cstdlib>
#include <fstream>
#include <cmath>
#include <sstream>

void _XML_READER_start( void *data, const char *el, const char **attr )
{
    XmlReader * reader = ( XmlReader* ) data;
    reader->start ( el, attr );
}

void _XML_READER_end( void *data, const char *el )
{
    XmlReader* reader = ( XmlReader* ) data;
    reader->end ( el );
}

XmlReader::XmlReader()
{

}

XmlReader::~XmlReader ()
{

}



bool
XmlReader::readXml( const string & path_name )
{
    path = tildeExpand( path_name );

    std::ifstream file;
    file.open( path.c_str() );

    if ( ! file.is_open() )
    {
        return false;
    }

    parser = XML_ParserCreate( NULL );
    if ( ! parser )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << ": Could not allocate memory for xml parser" << std::endl;
        std::exit( EXIT_FAILURE );
    }

    XML_SetUserData ( parser, this );

    XML_SetElementHandler(parser, &::_XML_READER_start, &::_XML_READER_end);

    line = 0;
    while ( ! file.eof () )
    {
        // I would have prefered `ifstream >> string' but the
        // XML parser can't give line numbers for some reason.
        // Anoying...
        char * buffer_str = (char*)XML_GetBuffer ( parser, 256 );
        if ( buffer_str == NULL )
        {
            std::cerr << __FILE__ << ": " << __LINE__
                      << ": Cannot allocate XML buffer" << std::endl;;
            std::exit( EXIT_FAILURE );
        }

        std::istringstream buffer( buffer_str, 256 );
        file.getline( buffer_str, 256 );
        // get the '\n' that the get above, didn't get
        // buffer << file.get ();
        ++line;
        if ( ! XML_ParseBuffer( parser,
                                std::strlen( buffer_str ),
                                false ) )
        {
            std::cerr << path << ": " << line
                      << ": XML Parser error : "
                      << XML_ErrorString ( XML_GetErrorCode ( parser ) )
                      << endl;
            std::exit( EXIT_FAILURE );
        }
    }
    file.close();
    XML_Parse( parser, NULL, 0, 1 );
    XML_ParserFree( parser );
    return true;
}

bool
XmlReader::createFile( const string & path_name )
{
    path = tildeExpand( path_name );

    std::ofstream file;
    file.open( path.c_str() );
    if ( ! file.is_open () )
    {
        return false;
    }

    print( file );
    file.close();
    return true;
}


bool
XmlReader::extract( const string & attr,
                    const string & val,
                    double & dest,
                    double start,
                    double end )
{
    std::istringstream str( val.c_str(), val.length() );
    str >> dest;
    if ( !str.fail () && str.get () == '%' )
    {
        dest *= 0.01;
        dest = ( start * ( 1 - dest )
                 + end * ( 1 + dest ) ) * 0.5;
    }
    if ( str.fail () || str.peek () > 0 )
    {
        // error
        std::cerr << path << ": " << line
                  << ": Cannot convert " << attr
                  << " attribute from " << val << std::endl;
        return false;
    }
    return true;
}

bool
XmlReader::extract( const std::string & attr,
                    const std::string & val,
                    std::string & dest )
{
    if ( val.length () == 0 )
    {
        // error
        std::cerr << path << ": " << line
                  << ": Cannot convert " << attr
                  << " attribute from \"\"" << std::endl;
        return false;
    }
    dest = val;
    return true;
}

bool
XmlReader::extract( const std::string & attr,
                    const std::string & val,
                    char & dest )
{
    if ( val.length () == 0 )
    {
        // error
        std::cerr << path << ": " << line
                  << ": Cannot convert " << attr
                  << " attribute from \"\"" << std::endl;
        return false;
    }

    dest = val[ 0 ];
    return true;
}

#endif // HAVE_LIBEXPAT
