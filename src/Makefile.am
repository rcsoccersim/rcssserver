AM_CXXFLAGS = -W -Wall -I@srcdir@/src

moduledir = $(pkglibdir)/modules

AM_CPPFLAGS = -DMODULEDIR="\"$(moduledir)\""

PRELOAD_MODULES = @MODULES@
DYNAMIC_MODULES =

if NODLOPEN
PRELOAD_MODULES += $(DYNAMIC_MODULES)
endif

libimports.cpp:	Makefile
	@RCSSLIBIMPORTER@ $(PRELOAD_MODULES) > $@

CLEANFILES = \
*~ \
core \
libimports.cpp \
moduletest.sh

DISTCLEANILES = \
rcsoccersim

lib_LTLIBRARIES = \
librcssresultsaver.la \
librcssclangparser.la

module_LTLIBRARIES = \
libstdoutsaver.la \
libcsvsaver.la

DYNAMIC_MODULES += \
libstdoutsaver.la \
libcsvsaver.la

# if BUILD_MYSQL_SAVER

# pkglib_LTLIBRARIES += libmysqlsaver.la
# DYNAMIC_MODULES += libmysqlsaver.la

# libmysqlsaver_la_SOURCES = mysqlsaver.cpp

# libmysqlsaver_la_LDFLAGS = -module

# libmysqlsaver_la_LIBADD = @MYSQLLIB@ librcssresultsaver.la

# endif


librcssresultsaver_la_SOURCES = resultsaver.cpp resultsaver.hpp


libstdoutsaver_la_SOURCES = stdoutsaver.cpp

libstdoutsaver_la_LDFLAGS = -module

libstdoutsaver_la_LIBADD = librcssresultsaver.la

libcsvsaver_la_SOURCES = csvsaver.cpp

libcsvsaver_la_LDFLAGS = -module

libcsvsaver_la_LIBADD = -lrcssconfparser librcssresultsaver.la


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
coach_lang_comp.h \
region.h \
coach_lang_par.h \
vector.h \
visitor.h \
clangutil.h \
tuple.h \
cond.h \
compop.h \
arithop.h \
hasa.h \
rule.h \
rcssexceptions.h \
rcssserverconfig.hpp \
resultsaver.hpp

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


librcssclangparser_la_SOURCES = \
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
coach_lang_comp.cpp \
coach_lang_par.ypp \
coach_lang_tok.cpp \
region.cpp \
visitor.cpp \
clangutil.cpp \
cond.cpp \
compop.cpp \
arithop.cpp \
rule.cpp


bin_PROGRAMS = \
rcssserver @RCSSCLIENT@

bin_SCRIPTS = rcsoccersim


BUILT_SOURCES = libimports.cpp \
coach_lang_par.h \
coach_lang_tok.cpp \
player_command_tok.cpp \
player_command_parser.h

EXTRA_DIST = \
coach_lang_tok.lpp \
player_command_tok.lpp \
rcsoccersim.in \
ac_lib_rcssclangparser.m4

nodist_rcssserver_SOURCES = libimports.cpp

rcssserver_SOURCES = \
heteroplayer.cpp \
heteroplayer.h \
param.h \
main.cpp \
types.h \
field.cpp \
playerparam.h \
playerparam.cpp \
field.h \
netif.cpp \
netif.h \
version.h \
object.h \
object.cpp \
team.h \
team.cpp \
xmlreader.h \
xmlreader.cpp \
landmarkreader.h \
landmarkreader.cpp \
coach.cpp \
coach.h \
compress.h \
remoteclient.h \
pcombuilder.h \
pcomparser.h \
player_command_parser.ypp \
player_command_tok.h \
player_command_tok.cpp \
player.h \
player.cpp \
logger.h \
logger.cpp \
monitor.h \
monitor.cpp \
arm.h \
random.h \
audio.h \
audio.cpp \
serializer.h \
serializer.cpp \
serializerplayerstdv1.h \
serializerplayerstdv1.cpp \
serializerplayerstdv7.h \
serializerplayerstdv7.cpp \
serializerplayerstdv8.h \
serializerplayerstdv8.cpp \
serializercoachstdv1.h \
serializercoachstdv1.cpp \
serializercoachstdv7.h \
serializercoachstdv7.cpp \
serializercoachstdv8.h \
serializercoachstdv8.cpp \
serializeronlinecoachstdv1.h \
serializeronlinecoachstdv1.cpp \
serializeronlinecoachstdv6.h \
serializeronlinecoachstdv6.cpp \
serializeronlinecoachstdv7.h \
serializeronlinecoachstdv7.cpp \
serializeronlinecoachstdv8.h \
serializeronlinecoachstdv8.cpp \
serializercommonstdv1.h \
serializercommonstdv1.cpp \
serializercommonstdv7.h \
serializercommonstdv7.cpp \
serializercommonstdv8.h \
serializercommonstdv8.cpp \
serializermonitor.h \
serializermonitor.cpp \
serverparam.h \
serverparam.cpp \
referee.h \
referee.cpp \
xpmholder.cpp \
xpmholder.h \
visual.h \
visual.cpp \
xpmholder.h \
timer.h \
timer.cpp \
stdtimer.h \
stdtimer.cpp \
synctimer.h \
synctimer.cpp \
timeable.h \
utility.h \
utility.cpp \
dispsender.h \
dispsender.cpp \
bodysender.h \
bodysender.cpp \
fullstatesender.h \
fullstatesender.cpp \
initsender.h \
initsender.cpp \
initsendermonitor.h \
initsendermonitor.cpp \
initsenderlogger.h \
initsenderlogger.cpp \
observer.h \
sender.h


rcssserver_LDADD = $(LLLIBS) $(PRELOAD_MODULES) \
-lrcssclangparser \
-lrcssresultsaver \
-lrcssconfparser \
-lrcsslib \
-lrcsserror \
-lrcssnet \
-lrcssgz \
-lrcssbase \
$(BOOST_FILESYSTEM_LIB)

rcssserver_DEPENDENCIES = $(PRELOAD_MODULES)

AM_YFLAGS=-d

#.yy.h: $*.yy
#	if test -e "$*.cc"; then \
#		if test "$*.cc" -ot "$*.yy"; then \
#			$(MAKE) $(AM_MAKEFLAGS) $*.cc; \
#		elif test "$*.h" -ot "$*.yy"; then \
#			$(MAKE) $(AM_MAKEFLAGS) -W "$*.yy" $*.cc; \
#			touch "$*.h"; \
#		fi \
#	else \
#		$(MAKE) $(AM_MAKEFLAGS) $*.cc; \
#	fi

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
client.cc \
client.h \
compress.h

rcssclient_LDADD = @RCSSCLIENT_LIBS@ \
-lrcssnet \
-lrcssgz \
$(BOOST_FILESYSTEM_LIB)

check_SCRIPTS = \
moduletest.sh

TESTS = \
moduletest.sh

moduletest.sh: Makefile.am
	echo "#! /bin/sh" > $@
	for module in $(module_LTLIBRARIES); do \
		echo "echo \"testing $$module\""; >> $@; \
		echo "@RCSSMODTEST@ $$module" >> $@; \
		echo "if test $$? -ne 0; then exit 1; fi" >> $@; \
	done
	chmod +x $@

#@INCLUDE_MK@
