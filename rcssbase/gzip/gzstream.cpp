// -*-c++-*-

/***************************************************************************
                                 gzstream.cpp
                      Compression and decompression streams
                             -------------------
    begin                : 14-DEC-2003
    copyright            : (C) 2002, 2003 by The RoboCup Soccer Server
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

#include "gzstream.hpp"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_LIBZ
#include <zlib.h>
#endif

#ifndef EOF
#define EOF (-1)
#endif

namespace rcss {
namespace gz {

class gzstreambuf_impl {
public:
    gzstreambuf_impl()
#ifdef HAVE_LIBZ
        : M_compression_stream( NULL ),
          M_decompression_stream( NULL )
#endif
      { }

#ifdef HAVE_LIBZ
    z_stream * M_compression_stream; // used to perform the compression
    z_stream * M_decompression_stream; // used to perform the decompression
#endif
};

gzstreambuf::gzstreambuf( std::streambuf & strm,
                          unsigned int bufsize )
    : std::streambuf(),
      M_strmbuf( strm ),
      M_output_stream( NULL ),
      M_input_stream( NULL ),
      M_buffer_size( bufsize ),
      M_read_buffer( NULL ),
      M_input_buffer( NULL ),
      M_output_buffer( NULL ),
      M_write_buffer( NULL ),
      M_remained( 0 ),
      m_streams( new gzstreambuf_impl ),
      M_level(
#ifdef HAVE_LIBZ
              Z_DEFAULT_COMPRESSION
#else
              -1
#endif
              )
{

}

gzstreambuf::~gzstreambuf ()
{
    writeData(
#ifdef HAVE_LIBZ
              Z_FINISH
#endif
              );

#ifdef HAVE_LIBZ
    if ( m_streams->M_compression_stream )
    {
        deflateEnd( m_streams->M_compression_stream );
    }
    if ( m_streams->M_decompression_stream )
    {
        deflateEnd( m_streams->M_decompression_stream );
    }
#endif

    delete M_output_stream;
    delete M_input_stream;
    delete[] M_read_buffer;
    delete[] M_input_buffer;
    delete[] M_output_buffer;
    delete[] M_write_buffer;
#ifdef HAVE_LIBZ
    delete m_streams->M_compression_stream;
    delete m_streams->M_decompression_stream;
#endif
}

bool
gzstreambuf::setLevel( int level )
{
#ifdef HAVE_LIBZ
    if ( level <= Z_BEST_COMPRESSION )
    {
        sync(); // make sure there is room for the deflate to flush
        if ( level >= 0 )
        {
            if( m_streams->M_compression_stream )
                deflateParams( m_streams->M_compression_stream, level, Z_DEFAULT_STRATEGY );
            sync(); // write any data that was flushed by deflateParams
        }
        M_level = level;
        return true;
    }
    else
        return false;
#else
    if ( M_level >= 0 )
    {
        return false;
    }
    else
    {
        return true;
    }
#endif
}

bool
gzstreambuf::writeData( int sync )
{
    int size = ( pptr() - M_output_buffer ) * sizeof( char_type );
    if ( size == 0 )
    {
        return true;
    }


    if ( M_output_stream == NULL )
    {
        M_output_stream = new std::ostream( &M_strmbuf );
    }

#ifdef HAVE_LIBZ
    if( M_level < 0 )
    {
#endif
        M_output_stream->write( M_output_buffer, size );
#ifdef HAVE_LIBZ
    }
    else
    {
        if ( m_streams->M_compression_stream == NULL )
        {
            m_streams->M_compression_stream = new z_stream;
            m_streams->M_compression_stream->zalloc = Z_NULL;
            m_streams->M_compression_stream->zfree = Z_NULL;
            m_streams->M_compression_stream->opaque = NULL;
            m_streams->M_compression_stream->avail_in = 0;
            m_streams->M_compression_stream->next_in = 0;
            m_streams->M_compression_stream->next_out = 0;
            m_streams->M_compression_stream->avail_out = 0;
            if( deflateInit( m_streams->M_compression_stream, M_level ) != Z_OK )
            {
                //                     cerr << "error in init\n";
                return false;
            }
            if( M_write_buffer == NULL )
                M_write_buffer = new char[ M_buffer_size ];
            m_streams->M_compression_stream->next_out = (Bytef*)M_write_buffer;
            m_streams->M_compression_stream->avail_out = M_buffer_size;
        }
        m_streams->M_compression_stream->next_in = (Bytef*)M_output_buffer;
        m_streams->M_compression_stream->avail_in = size;

        do
        {
            int bytes_out = -m_streams->M_compression_stream->total_out;
            int err = deflate( m_streams->M_compression_stream, sync );
            if( err != Z_OK && err != Z_STREAM_END )
            {
                //                      cerr << "error deflating\n";
                //                      cerr << M_compression_stream->msg << endl;
                deflateReset( m_streams->M_compression_stream );
                return false;
            }
            bytes_out += m_streams->M_compression_stream->total_out;
            M_output_stream->write( M_write_buffer, bytes_out );
            m_streams->M_compression_stream->next_out = (Bytef*)M_write_buffer;
            m_streams->M_compression_stream->avail_out = M_buffer_size;
        }
        while( m_streams->M_compression_stream->avail_in != 0 );
        // we want to keep writing until all the data has been
        // consumed by the compression stream

        deflateReset( m_streams->M_compression_stream );
    }
#endif

    if( sync !=
#ifdef HAVE_LIBZ
        Z_NO_FLUSH
#else
        0
#endif
        )
        M_output_stream->flush(); // flush the underlying stream if a flush has
    // been used
    return true;
}

int
gzstreambuf::readData( char * dest,
                       int & dest_size )
{
    if ( M_input_stream == NULL )
    {
        M_input_stream = new std::istream( &M_strmbuf );
    }

    int rval = 0;

    if ( ! M_input_stream->good() )
    {
        M_input_stream->setstate( std::istream::failbit );
        return rval;
    }

    int bytes_to_read = M_strmbuf.in_avail();

    if ( bytes_to_read < 0 )
    {
        M_input_stream->setstate( std::istream::eofbit );
    }
    else if ( bytes_to_read == 0 )
    {
        rval = M_input_stream->read( dest, 1 ).gcount();
        bytes_to_read = M_strmbuf.in_avail();
        if ( bytes_to_read > dest_size - 1 )
        {
            bytes_to_read = dest_size - 1;
        }
        rval += M_input_stream->read( dest + 1, bytes_to_read ).gcount();
        dest_size = bytes_to_read + 1;
    }
    else
    {
        if ( bytes_to_read > dest_size )
        {
            bytes_to_read = dest_size;
        }
        rval = M_input_stream->read( dest, bytes_to_read ).gcount();
        dest_size = bytes_to_read;
    }
    return rval;
}

gzstreambuf::int_type
gzstreambuf::overflow( int_type c )
{
    // this method is supposed to flush the put area of the buffer
    // to the I/O device

    // if the buffer was not already allocated nor set by user,
    // do it just now
    if ( pptr() == NULL )
    {
        M_output_buffer = new char_type[ M_buffer_size ];
    }
    else
    {
        if ( ! writeData() )
        {
            return EOF;
        }
    }

    setp( M_output_buffer, M_output_buffer + M_buffer_size );
    if ( c != EOF )
    {
        sputc( c );
    }
    return 0;
}

int
gzstreambuf::sync()
{
    if ( pptr() != NULL )
    {
        // just flush the put area
        if ( ! writeData(
#ifdef HAVE_LIBZ
                         Z_SYNC_FLUSH
#else
                         1
#endif
                         ) )
            return -1;
        setp( M_output_buffer, M_output_buffer + M_buffer_size );
    }
    return 0;
}

gzstreambuf::int_type
gzstreambuf::underflow()
{
    // this method is supposed to read some bytes from the I/O device

    // if the buffer was not already allocated nor set by user,
    // do it just now
    if ( gptr() == NULL )
    {
        M_input_buffer = new char_type[ M_buffer_size ];
    }

    int readn = 0;
#ifdef HAVE_LIBZ
    if ( M_level < 0 )
    {
#endif
        if ( M_remained != 0 )
        {
            M_input_buffer[ 0 ] = M_remaining_char;
        }

        readn = M_buffer_size * sizeof(char_type) - M_remained;
        readData( M_input_buffer + M_remained, readn );
        int totalbytes = readn + M_remained;
        setg( M_input_buffer, M_input_buffer,
              M_input_buffer + totalbytes / sizeof( char_type ) );

        M_remained = totalbytes % sizeof( char_type );
        if ( M_remained != 0 )
        {
            M_remaining_char = M_input_buffer[ totalbytes / sizeof(char_type) ];
        }
#ifdef HAVE_LIBZ
    }
    else
    {
        if ( M_read_buffer == NULL )
        {
            M_read_buffer = new char_type[ M_buffer_size ];
        }

        if ( M_remained != 0 )
        {
            M_input_buffer[ 0 ] = M_remaining_char;
        }

        if ( m_streams->M_decompression_stream == NULL )
        {
            m_streams->M_decompression_stream = new z_stream;
            m_streams->M_decompression_stream->zalloc = Z_NULL;
            m_streams->M_decompression_stream->zfree = Z_NULL;
            m_streams->M_decompression_stream->opaque = NULL;
            m_streams->M_decompression_stream->avail_in = 0;
            m_streams->M_decompression_stream->next_in = 0;
            m_streams->M_decompression_stream->avail_out = 0;
            m_streams->M_decompression_stream->next_out = 0;
            if( inflateInit( m_streams->M_decompression_stream ) != Z_OK )
                return false;
        }
        m_streams->M_decompression_stream->next_out = (Bytef*)( M_input_buffer
                                                                + M_remained );
        m_streams->M_decompression_stream->avail_out = ( M_buffer_size
                                                         * sizeof(char_type)
                                                         - M_remained );
        do
        {
            if ( m_streams->M_decompression_stream->avail_in == 0 )
            {
                int bytes_read = M_buffer_size;
                readData( M_read_buffer, bytes_read );

                m_streams->M_decompression_stream->next_in = (Bytef*)M_read_buffer;
                m_streams->M_decompression_stream->avail_in = bytes_read;
            }

            readn -= m_streams->M_decompression_stream->total_out;

            if( inflate( m_streams->M_decompression_stream, Z_NO_FLUSH ) != Z_OK )
            {
                //cerr << M_decompression_stream->msg << endl;
                inflateReset( m_streams->M_decompression_stream );
                return EOF;
            }
            readn += m_streams->M_decompression_stream->total_out;

        }
        while( readn == 0 );
        int totalbytes = readn + M_remained;
        setg( M_input_buffer, M_input_buffer,
              M_input_buffer + totalbytes / sizeof( char_type ) );

        M_remained = totalbytes % sizeof( char_type );
        if ( M_remained != 0 )
        {
            M_remaining_char = M_input_buffer[ ( totalbytes
                                                 / sizeof( char_type ) ) ];
        }

        inflateReset( m_streams->M_decompression_stream );
    }
#endif
    // if (readn == 0 && remained_ != 0)
    // error - there is not enough bytes for completing
    // the last character before the end of the stream
    // - this can mean error on the remote end
    if ( readn == 0 )
    {
        return EOF;
    }

    return sgetc();
}


// Gzstream Class

gzstream::gzstream( std::streambuf & strm,
                    unsigned int buffer_size )
    : std::iostream( static_cast< std::streambuf * >( 0 ) )
    , M_gzstreambuf( strm, buffer_size )
{
    this->init( &M_gzstreambuf );
}

gzstream::gzstream( std::iostream & strm,
                    unsigned int buffer_size )
    : std::iostream( static_cast< std::streambuf * >( 0 ) )
    , M_gzstreambuf( *(strm.rdbuf()), buffer_size )
{
    this->init( &M_gzstreambuf );
}

// Gzistream Class

gzistream::gzistream( std::streambuf & src,
                      unsigned int buffer_size )
    : std::istream( static_cast< std::streambuf * >( 0 ) )
    , M_gzstreambuf( src, buffer_size )
{
    this->init( &M_gzstreambuf );
}

gzistream::gzistream( std::istream & src,
                      unsigned int buffer_size )
    : std::istream( static_cast< std::streambuf * >( 0 ) )
    , M_gzstreambuf( *(src.rdbuf()), buffer_size )
{
    this->init( &M_gzstreambuf );
}

// Gzostream Class

gzostream::gzostream( std::streambuf & dest,
                      unsigned int buffer_size )
    : std::ostream( static_cast< std::streambuf * >( 0 ) )
    , M_gzstreambuf( dest, buffer_size )
{
    this->init( &M_gzstreambuf );
}

gzostream::gzostream( std::ostream & dest,
                      unsigned int buffer_size )
    : std::ostream( static_cast< std::streambuf * >( 0 ) )
    , M_gzstreambuf( *(dest.rdbuf()), buffer_size )
{
    this->init( &M_gzstreambuf );
}

} // namespace gz
} // namespace rcss
