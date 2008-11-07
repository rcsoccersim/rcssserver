// -*-c++-*-

/***************************************************************************
                                 gzfstream.hpp
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

#ifndef RCSSBASE_GZIP_GZFSTREAM_H
#define RCSSBASE_GZIP_GZFSTREAM_H


#include <iostream>
#include <string>

#include <boost/scoped_ptr.hpp>


namespace rcss {
namespace gz {

struct gzfilebuf_impl;

/*!
  \class gzfilebuf
  \brief gzip file stream buffer class.

  This class implements basic_filebuf for gzipped files.
  It doesn't yet support seeking (allowed by zlib but slow/limited),
  putback and read/write access(tricky). Otherwise, it attempts
  to be a drop-in replacement for the standard file streambuf.
*/
class gzfilebuf
    : public std::streambuf {
public:
    /*!
      \brief typical compression level enumeration

      The list of compresion level defined in zlib.h
      - Z_NO_COMPRESSION
      - Z_BEST_SPEED
      - Z_BEST_COMPRESSION
      - Z_DEFAULT_COMPRESSION
    */
    enum CompressionLevel {
        DEFAULT_COMPRESSION = -1,
        NO_COMPRESSION = 0,
        BEST_SPEED = 1,
        BEST_COMPRESSION = 9,
    };

    /*!
      \brief typical compression strategy enumeration.

      The list of strategy defined in zlib.h
      - Z_DEFAULT_STRATEGY
      - Z_FILTERED
      - Z_HUFFMAN_ONLY
      - Z_RLE  // run-length

      For more details, see deflateInit2 in zlib.h.
    */
    enum Strategy {
        DEFAULT_STRATEGY = 0,
        FILTERED = 1,
        HUFFMAN_ONLY = 2,
        RLE = 3,
    };

private:

    //! Pimpl ideom. the instance of a file buffer.
    boost::scoped_ptr< gzfilebuf_impl > M_impl;

    //! buffer size (default: 8192)
    std::size_t M_buf_size;
    //! pointer to the stream buffer. This is used as array.
    char_type * M_buf;

    //! used in underflow
    int M_remained_size;
    //! used in underflow
    char_type M_remained_char;


    //! not used
    gzfilebuf( const gzfilebuf & );
    //! not used
    gzfilebuf & operator=( const gzfilebuf & );

public:
    /*!
      \brief default constructor.

      Default constructor creates an internal file buffer using boost::scoped_ptr.
      This buffer is deleted automatically.
     */
    gzfilebuf();

    /*!
      \brief destructor.

      close opend file
      delete dynamically allocated internal buffer
    */
    virtual
    ~gzfilebuf();

    /*!
      \brief check if file is open.
      \return returns true if file is opend, else false.
     */
    bool is_open();

    /*!
      \brief open the file.
      \param path file path
      \param mode specifies mode of I/O. Only std::ios_base::in or
      std::ios_base::out can be used.
      \param level compression level(0-9, -1 means default level)
      \param strategy compression strategy.
      Z_DEFAULT_COMPRESSION, Z_FILTERD, Z_HUFFMAN_ONLY or Z_RLE.
      For more details, see deflateInit2 in zlib.h.

      If file is already opened, this method has no effect.
      If file is opened successfully, buffer is also allocated.
      This buffer is deleted when gzfilebuf is deleted.
     */
    gzfilebuf * open( const char * path,
                      std::ios_base::openmode mode,
                      int level = DEFAULT_COMPRESSION,
                      int strategy = DEFAULT_STRATEGY );

    /*!
      \brief closes the file if opened.
      \return NULL.
    */
    gzfilebuf * close() throw();


private:

    /*!
      \brief flush current internal buffer.
      \return true if successfully flushed, else false

      This method is called from close(), sync() and overflow().
    */
    bool flushBuf();

    /*!
      \brief make open mode string for gzope
      \param mode determins I/O mode. Only std::ios_base::in or
      std::ios_base::out can be used.
      \param level compression level.
      \param strategy compression strategy.

      create a string to determine the type of zlib file operation.
      For more details, see gzopen in zlib.h
    */
    std::string makeModeString( std::ios_base::openmode mode,
                                int level,
                                int strategy );

    /*!
      \brief release the buffer and set pointers to NULL.

      Internal buffer is deleted.
      The pointers used in super class are reset.
     */
    void destroyInternalBuffer() throw();

protected:
    //virtual
    //void imbue( const locale& loc );
    //virtual
    //int_type pbackfail( int_type c );
    //virtual
    //std::streambuf* setbuf( char_type* p, std::streamsize n );

    /*!
      \brief overrided method.
      set relative position of internal position pointer
      \param off offset to move. This is decompressed data size.
      \param way object of type ios_base::seekdir.
      ios_base::beg (offset from the beginning of the stream's buffer).
      ios_base::cur (offset from the current position in the stream's buffer).
      ios_base::cur (offset from the end of the stream's buffer).
      \param mode IO mode
      \return new position value of the modified position pointer.
      in case of error, returned -1.
    */
    virtual
    std::streampos seekoff( std::streamoff off,
                            std::ios_base::seekdir way,
                            std::ios_base::openmode mode );

    /*!
      \brief overrided method. seek stream.
      \param pos position for the position pointer.
      \param mode IO mode
      \return new position value of the modified position pointer.
      in case of error, returned -1.
    */
    virtual
    std::streampos seekpos( std::streampos pos,
                            std::ios_base::openmode mode );

    /*!
      \brief overrided method.
      get number of characters availbale in input sequence
      \return number of characters remaining to be read in the object
     */
    virtual
    std::streamsize showmanyc();

    /*!
      \brief synchronize stream buffer
      \retval 0 data was successfully flushed
      \retval -1 failed to synchronize
    */
    virtual
    int sync();

    /*!
      \brief put character at current put position.
      \param c this char is put to file.
      \return always 0
    */
    virtual
    std::streambuf::int_type overflow( std::streambuf::int_type c );

    // Not need to override because super class works sufficiently.
    //virtual
    //int_type uflow();

    /*!
      \brief get current character
      \return current character. in the case error, returned EOF.
     */
    virtual
    std::streambuf::int_type underflow();

    // Not need to override because super class works sufficiently.
    //virtual
    //std::streamsize xsgetn( char_type* s, std::streamsize n );

    // Not need to override because super class works sufficiently.
    //virtual
    //std::streamsize xsputn( char_type * s, std::streamsize n );

    /*   virtual methods defined in std::streambuf.
         See: http://www.cplusplus.com/ref/iostream/
      XXX imbue Imbue locale [virtual]
      OK overflow Put character at current position [virtual]
      XXX pbackfail Put character back [virtual]
      N/A seekoff Set relative position of internal position pointer [virtual]
      OK seekpos  Set absolute position of internal position pointer [virtual]
      XXX setbuf  Set buffer [virtual]
      OK showmanyc  Get number of characters availbale in input sequence [virtual]
      OK sync Synchronize stream buffer [virtual]
      OK uflow  Get current character [virtual]
      OK underflow  Get current character [virtual]
      OK xsgetn Get some characters [virtual]
      OK xsputn Write some characters [virtual]
    */
};


/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/
/*-------------------------------------------------------------------*/


/*!
  \class gzifstream
  \brief gzipped file input stream class.
*/
class gzifstream
    : public std::istream {
private:
    //! underlying stream buffer.
    gzfilebuf M_file_buf;
public:
    /*!
      \brief default constructor
    */
    gzifstream();

    /*!
      \brief init stream buffer and open file.
      \param path file path to be opened.
     */
    explicit
    gzifstream( const char * path );

    /*!
      \brief get underlying stream buffer.
      \return pointer to the file buffer
     */
    gzfilebuf * rdbuf() const
      {
          return const_cast< gzfilebuf * >( &M_file_buf );
      }

    /*!
      \brief check if file is open.
      \retval true file opened.
      \retval false file is not opened.
    */
    bool is_open()
      {
          return M_file_buf.is_open();
      }

    /*!
      \brief open gzipped file.
      \param path file path.

      Stream will be in state good() if file opens successfully;
      otherwise will be in state fail(). This differs from the behavior
      of ifstream, which never sets the state to good() and therefore
      won't allow you to reuse the stream for a second file unless
      you manually clear() the state. The choice is a matter of
      convenience.
     */
    void open( const char * path );

    /*!
      \brief close gzipped file.

      if failed, stream will become state fail()
     */
    void close();
};

/*****************************************************************************/

/*!
  \class gzofstream
  \brief gzipped file output stream class.
*/
class gzofstream
    : public std::ostream {
private:
    //! underlying stream buffer.
    gzfilebuf M_file_buf;

public:
    /*!
      \brief default constructor

      initializes stream buffer
     */
    gzofstream();

    /*!
      \brief construct streambuf with file name.
      \param path file path.
      \param level compression level
      \param strategy compression strategy

      initialize stream buffer and open file
     */
    explicit
    gzofstream( const char * path,
                int level = gzfilebuf::DEFAULT_COMPRESSION,
                int strategy = gzfilebuf::DEFAULT_STRATEGY );

    /*!
      \brief get const_cast<> pointer to the underlying stream buffer.
      \return pointer to the file buffer.
    */
    gzfilebuf * rdbuf() const
      {
          return const_cast< gzfilebuf * >( &M_file_buf );
      }

    /*!
      \brief check if file is open
      \retval true file opened.
      \retval false file is not opened.
     */
    bool is_open()
      {
          return M_file_buf.is_open();
      }

    /*!
      \brief open gzipped file.
      \param path file path.
      \param level compression level
      \param strategy compression strategy

      Stream will be in state good() if file opens successfully;
      otherwise in state fail().
      This differs from the behavior of ofstream, which never sets
      the state to good() and therefore won't allow you to reuse
      the stream for a second file unless you manually clear() the state.
      The choice is a matter of convenience.
    */
    void  open( const char * path,
                int level = gzfilebuf::DEFAULT_COMPRESSION,
                int strategy = gzfilebuf::DEFAULT_STRATEGY );

    /*!
      \brief close gzipped file.

      if close failed, stream will become state fail().
    */
    void close();
};

} // end namespace
} // end namespace

#endif
