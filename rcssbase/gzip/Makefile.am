
lib_LTLIBRARIES = librcssgz.la

librcssgz_la_SOURCES= \
	gzfstream.hpp \
	gzfstream.cpp \
	gzstream.hpp \
	gzstream.cpp

librcssgz_la_LDFLAGS = -version-info 1:0:0
#    1. Start with version information of `0:0:0' for each libtool library.
#
#    2. Update the version information only immediately before a public
#       release of your software.  More frequent updates are unnecessary,
#       and only guarantee that the current interface number gets larger
#       faster.
#
#    3. If the library source code has changed at all since the last
#       update, then increment REVISION (`C:R:A' becomes `C:r+1:A').
#
#    4. If any interfaces have been added, removed, or changed since the
#       last update, increment CURRENT, and set REVISION to 0.
#
#    5. If any interfaces have been added since the last public release,
#       then increment AGE.
#
#    6. If any interfaces have been removed since the last public release,
#       then set AGE to 0.

librcssgz_la_LIBADD = @GZ_LIBS@

rcssbase_gzipdir = $(includedir)/rcssbase/gzip

rcssbase_gzip_HEADERS = \
	gzstream.hpp \
	gzfstream.hpp

AM_CPPFLAGS = -I$(top_srcdir)
AM_CFLAGS = -W -Wall
AM_CXXFLAGS = -W -Wall
AM_LDFLAGS =

CLEANFILES = \
	*~ \
	core
