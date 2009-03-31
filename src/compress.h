/* -*-c++-*- */

/*
 *Copyright:

 Copyright (C) 2001 RoboCup Soccer Server Maintainance Group.
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

#ifndef COMPRESS_H
#define COMPRESS_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#ifdef HAVE_LIBZ

#include <zlib.h>
#include <iostream>
#include <cstdlib>
#include <fstream>

class Compressor
{
protected:
    z_stream M_stream;

    char* M_out_buffer;
    int M_out_size;
    int M_out_avail;

public:
    Compressor( int level = Z_DEFAULT_COMPRESSION,
                int strategy = Z_DEFAULT_STRATEGY )
        : M_out_buffer( NULL ),
          M_out_size( 0 ),
          M_out_avail( 0 )
      {
          M_stream.zalloc = Z_NULL;
          M_stream.zfree = Z_NULL;
          M_stream.opaque = NULL;

          deflateInit( &M_stream, level );
          deflateParams( &M_stream, level, strategy );

      }

    ~Compressor()
      {
          deflateEnd( &M_stream );
          free( M_out_buffer );
      }

    int setCompression( int level = Z_DEFAULT_COMPRESSION,
                        int strategy =  Z_DEFAULT_STRATEGY )
      {
          return deflateParams( &M_stream, level, strategy );
      }

    int reset()
      {
          return deflateReset( &M_stream );
      }

    int compress( const char* in_buffer, int in_size, int z_flush = Z_NO_FLUSH )
      {
          if( M_out_buffer == NULL )
          {
              M_out_avail = (int)(in_size * 1.01 + 12);
              M_out_buffer = (char*)malloc( M_out_avail );
              if( M_out_buffer == NULL )
                  return Z_MEM_ERROR;

              M_stream.next_out = (Bytef*)M_out_buffer;
              M_stream.avail_out = M_out_avail;
          }

          M_stream.next_in = (Bytef*)in_buffer;
          M_stream.avail_in = in_size;

          int bytes_out = M_stream.total_out;

          int err;
          for(;;)
          {
              if( M_stream.avail_out == 0 )
              {
                  int extra = (int)(M_out_avail * 0.5);
                  M_out_buffer = (char*)realloc( M_out_buffer, M_out_avail + extra );
                  if( M_out_buffer == NULL )
                  {
                      err = Z_MEM_ERROR;
                      break;
                  }

                  M_stream.next_out = (Bytef*)( M_out_buffer + M_out_avail );
                  M_stream.avail_out += extra;
                  M_out_avail += extra;
              }

              err = deflate( &M_stream, z_flush );
              if( err != Z_OK )
                  break;
          }

          M_out_size += M_stream.total_out - bytes_out;
          return err;
      }

    int getOutput( char*& out, int& size, bool detach = false )
      {
          out = M_out_buffer;
          size = M_out_size;

          if( detach )
          {
              out = (char*)realloc( out, size );
              if( out == NULL )
                  return Z_MEM_ERROR;

              M_out_buffer = NULL;
              M_out_avail = 0;
          }

          M_out_size = 0;
          deflateReset( &M_stream );
          M_stream.next_out = (Bytef*)M_out_buffer;
          M_stream.avail_out = M_out_avail;

          return Z_OK;
      }
};

class Decompressor
{
protected:
    z_stream M_stream;

    char* M_out_buffer;
    int M_out_size;
    int M_out_avail;

public:
    Decompressor()
        : M_out_buffer( NULL ),
          M_out_size( 0 ),
          M_out_avail( 0 )
      {
          M_stream.zalloc = Z_NULL;
          M_stream.zfree = Z_NULL;
          M_stream.opaque = NULL;

          inflateInit( &M_stream );
      }

    ~Decompressor()
      {
          inflateEnd( &M_stream );
          free( M_out_buffer );
      }

    int decompress( const char* in_buffer,
                    int in_size,
                    int z_flush = Z_NO_FLUSH )
      {
          if( M_out_buffer == NULL )
          {
              M_out_avail = in_size * 2;
              M_out_buffer = (char*)malloc( M_out_avail );
              if( M_out_buffer == NULL )
                  return Z_MEM_ERROR;

              M_stream.next_out = (Bytef*)M_out_buffer;
              M_stream.avail_out = M_out_avail;
          }

          M_stream.next_in = (Bytef*)in_buffer;
          M_stream.avail_in = in_size;

          int bytes_out = M_stream.total_out;

          int err;
          for(;;)
          {
              if( M_stream.avail_out == 0 )
              {
                  int extra = (int)(M_out_avail * 0.5);
                  M_out_buffer = (char*)realloc( M_out_buffer,
                                                 M_out_avail + extra );
                  if( M_out_buffer == NULL )
                  {
                      err = Z_MEM_ERROR;
                      break;
                  }

                  M_stream.next_out = (Bytef*)( M_out_buffer + M_out_avail );
                  M_stream.avail_out += extra;
                  M_out_avail += extra;
              }

              err = inflate( &M_stream, z_flush );
              if( err != Z_OK )
                  break;
          }

          M_out_size = M_stream.total_out - bytes_out;
          return err;
      }

    int getOutput( char*& out, int& size, bool detach = false )
      {
          out = M_out_buffer;
          size = M_out_size;

          if( detach )
          {
              out = (char*)realloc( out, size );
              if( out == NULL )
                  return Z_MEM_ERROR;

              M_out_buffer = NULL;
              M_out_avail = 0;
          }

          M_out_size = 0;
          inflateReset( &M_stream );
          M_stream.next_out = (Bytef*)M_out_buffer;
          M_stream.avail_out = M_out_avail;

          return Z_OK;
      }
};



#endif // HAVE_LIBZ

#endif // _COMPRESS_H_
