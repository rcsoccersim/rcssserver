
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
	serializercommonjson.cpp \
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
	serializercommonjson.h \
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
	-L$(top_builddir)/rcss/clang \
	-L$(top_builddir)/rcss/conf \
	-L$(top_builddir)/rcss/net \
	-L$(top_builddir)/rcss/gzip

rcssserver_LDADD = \
	-lrcssclangparser \
	-lrcssconfparser \
	-lrcssnet \
	-lrcssgz \
	$(BOOST_FILESYSTEM_LIB) $(BOOST_SYSTEM_LIB)


BUILT_SOURCES = \
	player_command_parser.hpp \
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
	-L$(top_builddir)/rcss/net \
	-L$(top_builddir)/rcss/gzip

rcssclient_LDADD = @RCSSCLIENT_LIBS@ \
	-lrcssnet \
	-lrcssgz \
	$(BOOST_FILESYSTEM_LIB) $(BOOST_SYSTEM_LIB)


AM_CPPFLAGS = -I$(top_srcdir)
AM_CFLAGS = -W -Wall
#AM_CXXFLAGS = -W -Wall -Woverloaded-virtual
AM_CXXFLAGS = -W -Wall


EXTRA_DIST = \
	CMakeLists.txt \
	fix_lexer_file.cmake \
	player_command_parser.ypp \
	player_command_tok.lpp \
	rcsoccersim.in

CLEANFILES = \
	player_command_parser.cpp \
	player_command_parser.hpp \
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
