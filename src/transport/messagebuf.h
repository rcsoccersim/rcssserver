// -*-c++-*-


/*!
  \file messagebuf.h
  \brief The stream buffer that collects one outgoing message
*/

/*
 *Copyright:

 Copyright (C) The RoboCup Soccer Simulator Maintenance Group.

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

/////////////////////////////////////////////////////////////////////

#ifndef RCSS_MESSAGEBUF_H
#define RCSS_MESSAGEBUF_H

#include <functional>
#include <streambuf>
#include <string>

namespace rcss {

/*!
  \class MessageBuf
  \brief The stream buffer that collects one outgoing message.

  The characters written to this buffer are kept until sync() is called
  (i.e. std::flush on the stream). Then the whole message is passed to the
  sender, however long it is.
 */
class MessageBuf
    : public std::streambuf {
public:
    //! receives a complete message. It may modify or move the message.
    typedef std::function< void( std::string & ) > Sender;

private:
    Sender M_sender;
    std::string M_message;

public:
    explicit
    MessageBuf( const Sender & sender );

protected:
    int_type overflow( int_type c ) override;
    std::streamsize xsputn( const char_type * s,
                            std::streamsize n ) override;
    int sync() override;
};

}

#endif
