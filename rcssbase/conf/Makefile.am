
lib_LTLIBRARIES = librcssconfparser.la

librcssconfparser_la_SOURCES = \
	parser.cpp \
	builder.cpp \
	statushandler.cpp \
	streamstatushandler.cpp

librcssconfparser_la_LDFLAGS = -version-info 3:0:0
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

#librcssconfparser_la_LIBADD = $(BOOST_FILESYSTEM_LIB)

rcssbase_confdir = $(includedir)/rcssbase/conf

rcssbase_conf_HEADERS = \
	builder.hpp \
	parser.hpp \
	paramgetter.hpp \
	paramsetter.hpp \
	statushandler.hpp \
	streamstatushandler.hpp

AM_CPPFLAGS = -I$(top_srcdir)
AM_CFLAGS = -W -Wall
AM_CXXFLAGS = -W -Wall
AM_LD_FLAGS =

CLEANFILES = \
	*~ \
	core
