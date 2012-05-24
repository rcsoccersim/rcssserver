
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
	rule.cpp \
	rcssexceptions.cpp

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

pkginclude_HEADERS = \
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
	coach_lang_parser.h \
	arithop.h \
	compop.h \
	cond.h \
	region.h \
	rule.h \
	vector.h \
	rcssexceptions.h

nodist_librcssclangparser_la_SOURCES = \
	coach_lang_parser.ypp \
	coach_lang_tok.cpp



bin_PROGRAMS = \
	rcssserver @RCSSCLIENT@

bin_SCRIPTS = rcsoccersim

rcssserver_SOURCES = \
	audio.cpp \
	bodysender.cpp \
	coach.cpp \
	csvsaver.cpp \
	dispsender.cpp \
	field.cpp \
	fullstatesender.cpp \
	heteroplayer.cpp \
	initsender.cpp \
	initsendercoach.cpp \
	initsenderlogger.cpp \
	initsendermonitor.cpp \
	initsenderonlinecoach.cpp \
	initsenderplayer.cpp \
	landmarkreader.cpp \
	logger.cpp \
	main.cpp \
	monitor.cpp \
	pcombuilder.cpp \
	pcomparser.cpp \
	player.cpp \
	playerparam.cpp \
	object.cpp \
	referee.cpp \
	remoteclient.cpp \
	resultsaver.cpp \
	serializer.cpp \
	serializercoachstdv1.cpp \
	serializercoachstdv7.cpp \
	serializercoachstdv8.cpp \
	serializercoachstdv13.cpp \
	serializercoachstdv14.cpp \
	serializercommonstdv1.cpp \
	serializercommonstdv7.cpp \
	serializercommonstdv8.cpp \
	serializermonitor.cpp \
	serializeronlinecoachstdv1.cpp \
	serializeronlinecoachstdv6.cpp \
	serializeronlinecoachstdv7.cpp \
	serializeronlinecoachstdv8.cpp \
	serializeronlinecoachstdv13.cpp \
	serializeronlinecoachstdv14.cpp \
	serializerplayerstdv1.cpp \
	serializerplayerstdv7.cpp \
	serializerplayerstdv8.cpp \
	serializerplayerstdv13.cpp \
	serializerplayerstdv14.cpp \
	serverparam.cpp \
	stadium.cpp \
	stdoutsaver.cpp \
	stdtimer.cpp \
	synctimer.cpp \
	team.cpp \
	utility.cpp \
	visualsendercoach.cpp \
	visualsenderplayer.cpp \
	weather.cpp \
	xmlreader.cpp \
	xpmholder.cpp

nodist_rcssserver_SOURCES = \
	player_command_parser.ypp \
	player_command_tok.cpp

noinst_HEADERS = \
	arm.h \
	audio.h \
	bodysender.h \
	coach.h \
	compress.h \
	csvsaver.h \
	dispsender.h \
	field.h \
	fullstatesender.h \
	heteroplayer.h \
	initsender.h \
	initsendercoach.h \
	initsenderlogger.h \
	initsendermonitor.h \
	initsenderonlinecoach.h \
	initsenderplayer.h \
	landmarkreader.h \
	logger.h \
	monitor.h \
	observer.h \
	object.h \
	param.h \
	pcombuilder.h \
	pcomparser.h \
	player.h \
	player_command_tok.h \
	playerparam.h \
	random.h \
	referee.h \
	remoteclient.h \
	resultsaver.hpp \
	sender.h \
	serializer.h \
	serializercoachstdv1.h \
	serializercoachstdv7.h \
	serializercoachstdv8.h \
	serializercoachstdv13.h \
	serializercoachstdv14.h \
	serializercommonstdv1.h \
	serializercommonstdv7.h \
	serializercommonstdv8.h \
	serializeronlinecoachstdv1.h \
	serializeronlinecoachstdv6.h \
	serializeronlinecoachstdv7.h \
	serializeronlinecoachstdv8.h \
	serializeronlinecoachstdv13.h \
	serializeronlinecoachstdv14.h \
	serializerplayerstdv1.h \
	serializerplayerstdv7.h \
	serializerplayerstdv8.h \
	serializerplayerstdv13.h \
	serializerplayerstdv14.h \
	serializermonitor.h \
	serverparam.h \
	stadium.h \
	stdoutsaver.h \
	stdtimer.h \
	synctimer.h \
	team.h \
	timeable.h \
	timer.h \
	types.h \
	utility.h \
	version.h \
	visual.h \
	visualsendercoach.h \
	visualsenderplayer.h \
	weather.h \
	xmlreader.h \
	xpmholder.h

rcssserver_LDFLAGS = \
	-L$(top_builddir)/rcssbase/conf \
	-L$(top_builddir)/rcssbase/net \
	-L$(top_builddir)/rcssbase/gzip

rcssserver_LDADD = \
	-lrcssclangparser \
	-lrcssconfparser \
	-lrcssnet \
	-lrcssgz \
	$(BOOST_FILESYSTEM_LIB) $(BOOST_SYSTEM_LIB) @GZ_LIBS@


BUILT_SOURCES = \
	coach_lang_parser.h \
	coach_lang_tok.cpp \
	player_command_parser.h \
	player_command_tok.cpp

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


EXTRA_PROGRAMS = rcssclient

rcssclient_SOURCES = \
	client.cpp

rcssclient_LDFLAGS = \
	-L$(top_builddir)/rcssbase/net \
	-L$(top_builddir)/rcssbase/gzip

rcssclient_LDADD = @RCSSCLIENT_LIBS@ \
	-lrcssnet \
	-lrcssgz \
	$(BOOST_FILESYSTEM_LIB) $(BOOST_SYSTEM_LIB) @GZ_LIBS@


AM_CPPFLAGS = -I$(top_srcdir)
AM_CFLAGS = -W -Wall
#AM_CXXFLAGS = -W -Wall -Woverloaded-virtual
AM_CXXFLAGS = -W -Wall


EXTRA_DIST = \
	coach_lang_parser.ypp \
	coach_lang_tok.lpp \
	player_command_parser.ypp \
	player_command_tok.lpp \
	rcsoccersim.in

CLEANFILES = \
	coach_lang_parser.cpp \
	coach_lang_parser.h \
	coach_lang_tok.cpp \
	player_command_parser.cpp \
	player_command_parser.h \
	player_command_tok.cpp \
	*~ \
	core

DISTCLEANFILES = \
	rcsoccersim

#MAINTAINERCLEANFILES = \
#	coach_lang_parser.cpp \
#	coach_lang_parser.h \
#	coach_lang_tok.cpp \
#	player_command_parser.cpp \
#	player_command_parser.h \
#	player_command_tok.cpp
