#include "xmlreader.h"

#ifdef HAVE_LIBEXPAT

void _XML_READER_start ( void *data, const char *el, const char **attr )
{
  XmlReader* reader = ( XmlReader* ) data;
  reader->start ( el, attr );
}

void _XML_READER_end ( void *data, const char *el )
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



bool XmlReader::readXml ( const string& path_name )
{
    path = tildeExpand ( path_name );

  ifstream file;
  file.open ( path.c_str () );

  if ( !file.is_open () )
      return false;

  parser = XML_ParserCreate ( NULL );
  if (! parser)
    {
      cerr << __FILE__ << ": " << __LINE__
	   << ": Could not allocate memory for xml parser\n";
      exit ( EXIT_FAILURE );
    }

  XML_SetUserData ( parser, this );

  XML_SetElementHandler(parser, &::_XML_READER_start, &::_XML_READER_end);

  line = 0;
  while ( !file.eof () )
    {
      // I would have prefered `ifstream >> string' but the
      // XML parser can't give line numbers for some reason.
      // Anoying...
      char* buffer_str = (char*)XML_GetBuffer ( parser, 256 );
      if ( buffer_str == NULL )
        {
          cerr << __FILE__ << ": " << __LINE__
               << ": Cannot allocate XML buffer\n";
          exit ( EXIT_FAILURE );
        }

      strstream buffer ( buffer_str, 256 );
      file.getline ( buffer_str, 256 );
      // get the '\n' that the get above, didn't get
      // buffer << file.get ();
      line++;
      if ( ! XML_ParseBuffer( parser,
                              std::strlen( buffer_str ),
                              false ) )
        {
          cerr << path << ": " << line
               << ": XML Parser error : "
               << XML_ErrorString ( XML_GetErrorCode ( parser ) )
               << endl;
          exit ( EXIT_FAILURE );
        }
    }
  file.close ();
  XML_Parse ( parser, NULL, 0, 1 );
  XML_ParserFree ( parser );
  return true;
}

bool XmlReader::createFile ( const string& path_name )
{
    path = tildeExpand ( path_name );

  ofstream file;
  file.open ( path.c_str () );
  if ( !file.is_open () )
    return false;

  file << *this;
  file.close ();
  return true;
}

#endif // HAVE_LIBEXPAT
