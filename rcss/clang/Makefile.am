
lib_LTLIBRARIES = \
	librcssclangparser.la

librcssclangparser_la_SOURCES = \
	clangbuilder.cpp \
	clangmsgbuilder.cpp \
	clangparser.cpp \
	clangmsg.cpp \
	clangmetamsg.cpp \
	clangfreeformmsg.cpp \
	clangunsuppmsg.cpp \
	clangrulemsg.cpp \
	clangdelmsg.cpp \
	clanginfomsg.cpp \
	clangadvicemsg.cpp \
	clangdefmsg.cpp \
	clangaction.cpp \
	clangutil.cpp \
	coach_lang_comp.cpp \
	arithop.cpp \
	cond.cpp \
	compop.cpp \
	region.cpp \
	rule.cpp

librcssclangparser_la_LDFLAGS = -version-info 3:0:1
#    Changed from 2.0.0 to 3.0.1 at 9.3.5 for addition of buildCondList
#
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

librcssclangparserincludedir = $(includedir)/rcss/clang

librcssclangparserinclude_HEADERS = \
	clangparser.h \
	coach_lang_tok.h \
	clangbuilder.h \
	clangmsgbuilder.h \
	clangmsg.h \
	clangmetamsg.h \
	clangfreeformmsg.h \
	clangunsuppmsg.h \
	clangrulemsg.h \
	clangdelmsg.h \
	clanginfomsg.h \
	clangadvicemsg.h \
	clangdefmsg.h \
	clangaction.h \
	clangutil.h \
	coach_lang_comp.h \
	coach_lang_parser.hpp \
	arithop.h \
	compop.h \
	cond.h \
	region.h \
	rule.h

nodist_librcssclangparser_la_SOURCES = \
	coach_lang_parser.ypp \
	coach_lang_tok.cpp

BUILT_SOURCES = \
	coach_lang_parser.hpp \
	coach_lang_tok.cpp

AM_CPPFLAGS = -I$(top_srcdir)
AM_CFLAGS = -W -Wall
AM_CXXFLAGS = -W -Wall
AM_LD_FLAGS =

AM_YFLAGS=-d

FLEX=@FLEX@
FLEXFLAGS=@FLEXFLAGS@
AM_FLEXFLEX=@AM_FLEXFLAGS@

.lpp.cpp:	$*.lpp Makefile.am
	$(FLEX) $(FLEXFLAGS) $(AM_FLEXFLAGS) $<
	$(AWK) '/#include <FlexLexer\.h>/ { print "#include \"$*.h\""; getline; } \
		/class istream;/ { print "#include <iosfwd>"; print "using namespace std;"; getline; } \
		/#include <iostream\.h>/ { print "#include <iostream>"; getline; } \
		{ gsub( "lex\.yy\.c", "$@" ); print; }' lex.yy.c > $@
	rm -f lex.yy.c

.ll.cc:	$*.ll Makefile.am
	$(FLEX) $(FLEXFLAGS) $(AM_FLEXFLAGS) $<
	$(AWK) '/#include <FlexLexer\.h>/ { print "#include \"$*.h\""; getline; } \
		/class istream;/ { print "#include <iosfwd>"; print "using namespace std;"; getline; } \
		/#include <iostream\.h>/ { print "#include <iostream>"; getline; } \
		{ gsub( "lex\.yy\.c", "$@" ); print; }' lex.yy.c > $@
	rm -f lex.yy.c


EXTRA_DIST = \
	CMakeLists.txt \
	coach_lang_parser.ypp \
	coach_lang_tok.lpp \
	fix_lexer_file.cmake


CLEANFILES = \
	coach_lang_parser.cpp \
	coach_lang_parser.hpp \
	coach_lang_tok.cpp \
	*~
