
SUBDIRS = net conf gzip clang

rcssincludedir = $(includedir)/rcss

rcssinclude_HEADERS = \
	factory.hpp \
	parser.h \
	vector.h

AM_CPPFLAGS = -I$(top_srcdir)
AM_CFLAGS = -W -Wall
AM_CXXFLAGS = -W -Wall
AM_LDFLAGS =

EXTRA_DIST = \
	CMakeLists.txt

CLEANFILES = \
	*~ \
	core
