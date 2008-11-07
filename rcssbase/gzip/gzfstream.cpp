// -*-c++-*-

/***************************************************************************
                                 gzfstream.cpp
                      Compression and decompression file streams
                             -------------------
    begin                : 2007-04-11
    copyright            : (C) 2007 by The RoboCup Soccer Server
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gzfstream.hpp"

#include <string>
#include <cstdio>

#ifdef HAVE_LIBZ
#include <zlib.h>
#endif

namespace rcss {
namespace gz {

/////////////////////////////////////////////////////////////////////

//! the implementation of file stream buffer
struct gzfilebuf_impl {

    //! file open mode flag
    std::ios_base::openmode open_mode_;

#ifdef HAVE_LIBZ
    //! gzip file
    gzFile file_;
#endif

    //! constructor
    gzfilebuf_impl()
        : open_mode_( static_cast< std::ios_base::openmode >( 0 ) )
#ifdef HAVE_LIBZ
        , file_( NULL )
#endif
      { }
};

/////////////////////////////////////////////////////////////////////

/*-------------------------------------------------------------------*/
/*!

*/
gzfilebuf::gzfilebuf()
    : M_impl( new gzfilebuf_impl )
    , M_buf_size( 8192 )
    , M_buf( NULL )
    , M_remained_size( 0 )
{

}

/*-------------------------------------------------------------------*/
/*!

*/
gzfilebuf::~gzfilebuf()
{
    if ( is_open() )
    {
        close();
    }

    if ( M_buf )
    {
        delete [] M_buf;
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
bool
gzfilebuf::is_open()
{
#ifdef HAVE_LIBZ
    if ( M_impl
         && M_impl->file_ != NULL  )
    {
        return true;
    }
#endif
    return false;
}

/*-------------------------------------------------------------------*/
/*!

*/
gzfilebuf *
gzfilebuf::open( const char * path,
                 std::ios_base::openmode mode,
                 int level,
                 int strategy )
{
    gzfilebuf * ret = NULL;
#ifdef HAVE_LIBZ
    if ( ! M_impl )
    {
        return ret;
    }

    if ( ! this->is_open() )
    {
        bool testi = mode & std::ios_base::in;
        bool testo = mode & std::ios_base::out;
        if ( ( testi && testo )
             || ( ! testi && ! testo ) )
        {
            //std::cerr << __FILE__ << ':' << __LINE__
            //          << "openmode is duplicated" << std::endl;
            return ret;
        }

        std::string mode_str = makeModeString( mode, level, strategy );
        if ( mode_str.empty() )
        {
            return ret;
        }

        M_impl->file_ = gzopen( path, mode_str.c_str() );

        if ( M_impl->file_ == NULL )
        {
            return ret;
        }

        if ( M_buf )
        {
            destroyInternalBuffer();
        }

        M_buf = new char_type[M_buf_size];

        if ( testi )
        {
            // initial end point is same to start point,
            // because no data is read at first.
            M_remained_size = 0;
            this->setg( M_buf, M_buf, M_buf );
            M_impl->open_mode_ = std::ios_base::in;
        }

        if ( testo )
        {
            this->setp( M_buf, M_buf + M_buf_size );
            M_impl->open_mode_ = std::ios_base::out;
        }

        ret = this;
    }
#endif
    return ret;
}

/*-------------------------------------------------------------------*/
/*!

*/
gzfilebuf *
gzfilebuf::close() throw()
{
#ifdef HAVE_LIBZ
    if ( this->is_open() )
    {
        flushBuf();
        destroyInternalBuffer();

        if ( ! M_impl )
        {
            return NULL;
        }

        if ( M_impl->file_ == NULL )
        {
            return NULL;
        }

        // TODO: checking close status...
        gzclose( M_impl->file_ );
        M_impl->file_ = NULL;
        M_impl->open_mode_ = static_cast< std::ios_base::openmode >( 0 );
    }
#endif
    return NULL;
}

/*-------------------------------------------------------------------*/
/*!

*/
bool
gzfilebuf::flushBuf()
{
    bool ret = false;
#ifdef HAVE_LIBZ
    if ( is_open()
         && ( M_impl->open_mode_ & std::ios_base::out )
         && pptr() )
    {
        int size = ( pptr() - pbase() ) * sizeof( char_type );

        if ( size == 0 )
        {
            ret = true;
        }

        if ( size > 0 )
        {
            if ( gzwrite( M_impl->file_, M_buf, size ) != 0 )
            {
                // gzflush( M_impl->file_, Z_SYNC_FLUSH );
                ret = true;
            }
        }

        this->setp( M_buf, M_buf+ M_buf_size );
    }
#endif
    return ret;
}

/*-------------------------------------------------------------------*/
/*!

*/
std::string
gzfilebuf::makeModeString( std::ios_base::openmode mode,
                           int level,
                           int strategy )
{
    std::string mode_str("");
    mode_str.reserve( 6 );

    // set I/O
    bool testi = static_cast< bool >( mode & std::ios_base::in );
    bool testo = static_cast< bool >( mode & std::ios_base::out );

    if ( ( testi && testo )
         || ( ! testi && ! testo ) )
    {
        // invalid mode
        return std::string( "" );
    }

    if ( testi )
    {
        // If input mode, it is not necessary to consider
        // about compression level and strategy.
        mode_str = "rb";
        return mode_str;
    }

    if ( testo )
    {
        mode_str = "wb";
    }

    // set compression level
    if ( level == DEFAULT_COMPRESSION )
    {
        // nothing to do
    }
    else if ( NO_COMPRESSION <= level
              && level <= BEST_COMPRESSION )
    {
        char lvstr[4];
        snprintf( lvstr, 4, "%d", level );
        mode_str += lvstr;
    }
    else
    {
        // invalid compression level
        return std::string( "" );
    }

    // set strategy
    switch ( strategy ) {
    case DEFAULT_STRATEGY:
        // nothing to do
        break;
    case FILTERED:
        mode_str += "f";
        break;
    case HUFFMAN_ONLY:
        mode_str += "h";
        break;
    case RLE:
        mode_str += "R";
        break;
    default:
        // why reach here?
        break;
    }

    return mode_str;
}

/*-------------------------------------------------------------------*/
/*!

*/
void
gzfilebuf::destroyInternalBuffer() throw()
{
    if ( M_buf )
    {
        delete [] M_buf;
        M_buf = NULL;
        M_remained_size = 0;
        this->setg( NULL, NULL, NULL );
        this->setp( NULL, NULL );
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
std::streambuf::int_type
gzfilebuf::overflow( std::streambuf::int_type c )
{
    flushBuf();
    if ( c != traits_type::eof() )
    {
        *pptr() = traits_type::to_char_type( c );
        pbump( 1 );
    }
    return 0;
}

/*-------------------------------------------------------------------*/
/*!

*/
std::streampos
gzfilebuf::seekoff( std::streamoff off,
                    std::ios_base::seekdir way,
                    std::ios_base::openmode mode )
{
    if ( way & std::ios_base::end )
    {
        //! zlib does not support seeking from 'end'.
        return -1;
    }

    if ( ! is_open() )
    {
        return -1;
    }

    if ( ! ( M_impl->open_mode_ & mode ) )
    {
        return -1;

    }

    std::streampos ret = -1;
#ifdef HAVE_LIBZ
    if ( M_impl->open_mode_ & std::ios_base::in )
    {
        if ( way & std::ios_base::beg )
        {
            ret = gzseek( M_impl->file_, off, SEEK_SET );
            M_remained_size = 0;
            this->setg( M_buf, M_buf, M_buf );
        }

        if ( way & std::ios_base::cur )
        {
            ret = gzseek( M_impl->file_, off, SEEK_CUR );
            ret -= this->egptr() - this->gptr();
            if ( off != 0 )
            {
                M_remained_size = 0;
                this->setg( M_buf, M_buf, M_buf );
            }
        }
    }

    if ( M_impl->open_mode_ & std::ios_base::out )
    {
        this->sync();
        if ( way & std::ios_base::beg )
        {
            std::streampos cur = gztell( M_impl->file_ );
            if ( off <= cur )
            {
                ret = gzseek( M_impl->file_, off, SEEK_SET );
                this->setp( M_buf, M_buf + M_buf_size );
            }
        }

        if ( way & std::ios_base::cur )
        {
            if ( off < 0 )
            {
                ret = gzseek( M_impl->file_, off, SEEK_CUR );
                this->setp( M_buf, M_buf + M_buf_size );
            }
        }
    }
#endif
    return ret;
}

/*-------------------------------------------------------------------*/
/*!

*/
std::streampos
gzfilebuf::seekpos( std::streampos pos,
                    std::ios_base::openmode mode )
{
    if ( ! is_open() )
    {
        return -1;
    }

    std::streampos ret = -1;
#ifdef HAVE_LIBZ
    if ( ( M_impl->open_mode_ & std::ios_base::in )
         && ( mode & std::ios_base::in ) )
    {
        ret = gzseek( M_impl->file_, pos, SEEK_SET );
        // and reset buffer pointer to initial position
        M_remained_size = 0;
        this->setg( M_buf, M_buf, M_buf );
    }

    if ( ( M_impl->open_mode_ & std::ios_base::out )
         && ( mode & std::ios_base::out ) )
    {
        std::streampos cur = gztell( M_impl->file_ );
        if ( pos <= cur )
        {
            ret = gzseek( M_impl->file_, pos, SEEK_SET );
            this->setp( M_buf, M_buf + M_buf_size );
        }
    }
#endif
    return ret;
}

/*-------------------------------------------------------------------*/
/*!

*/
std::streamsize
gzfilebuf::showmanyc()
{
    std::streamsize ret = 0;

    if ( is_open()
         && ( M_impl->open_mode_ & std::ios_base::in ) )
    {
        ret = std::streamsize( egptr() - gptr() );
    }

    return ret;
}

/*-------------------------------------------------------------------*/
/*!

*/
int
gzfilebuf::sync()
{
    return flushBuf() ? 0 : -1;
}

/*-------------------------------------------------------------------*/
/*!

*/
std::streambuf::int_type
gzfilebuf::underflow()
{
#ifdef HAVE_LIBZ
    if ( ! is_open() || ! gptr() )
    {
        return traits_type::eof();
    }

    if ( M_remained_size )
    {
        M_buf[0] = M_remained_char;
    }

    int read_size = gzread( M_impl->file_,
                            ( void* )( M_buf + M_remained_size ),
                            M_buf_size * sizeof( char_type ) - M_remained_size );
    if ( read_size <= 0 )
    {
        return traits_type::eof();
    }

    int total_size = read_size + M_remained_size;
    this->setg( M_buf, M_buf, M_buf + total_size / sizeof( char_type ) );

    M_remained_size = total_size % sizeof( char_type );
    if ( M_remained_size )
    {
        M_remained_char = M_buf[total_size / sizeof( char_type )];
    }

    return sgetc();
#else
    return traits_type::eof();
#endif
}

///////////////////////////////////////////////////////////////////////

/*-------------------------------------------------------------------*/
/*!

*/
gzifstream::gzifstream()
    : std::istream( static_cast< std::streambuf * >( 0 ) )
    , M_file_buf()
{
    this->init( &M_file_buf );
}

/*-------------------------------------------------------------------*/
/*!

*/
gzifstream::gzifstream( const char * path )
    : std::istream( static_cast< std::streambuf * >( 0 ) )
    , M_file_buf()
{
    this->init( &M_file_buf );
    this->open( path );
}

/*-------------------------------------------------------------------*/
/*!

*/
void
gzifstream::open( const char * path )
{
    if ( ! M_file_buf.open( path, std::ios_base::in ) )
    {
        this->setstate( std::ios_base::failbit );
    }
    else
    {
        this->clear();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
gzifstream::close()
{
    if ( ! M_file_buf.close() )
    {
        this->setstate( std::ios_base::failbit );
    }
}

///////////////////////////////////////////////////////////////////////

/*-------------------------------------------------------------------*/
/*!

*/
gzofstream::gzofstream()
    : std::ostream( static_cast< std::streambuf * >( 0 ) )
    , M_file_buf()
{
    this->init( &M_file_buf );
}

/*-------------------------------------------------------------------*/
/*!

*/
gzofstream::gzofstream( const char * path,
                        int level,
                        int strategy )
    : std::ostream( static_cast< std::streambuf * >( 0 ) )
    , M_file_buf()
{
    this->init( &M_file_buf );
    this->open( path, level, strategy );
}

/*-------------------------------------------------------------------*/
/*!

*/
void
gzofstream::open( const char * path,
                  int level,
                  int strategy )
{
    if ( ! M_file_buf.open( path, std::ios_base::out, level, strategy ) )
    {
        this->setstate( std::ios_base::failbit );
    }
    else
    {
        this->clear();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
gzofstream::close()
{
    if ( ! M_file_buf.close() )
    {
        this->setstate( std::ios_base::failbit );
    }
}

} // end namespace
} // end namespace
