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
librcssutil.la \
librcsstimer.la \
librcssresultsaver.la \
librcssplayerparam.la \
librcssserverparam.la \
librcssclangparser.la \
librcsspcomparser.la

module_LTLIBRARIES = \
librcssstdtimer.la \
librcsssynctimer.la \
libstdoutsaver.la \
libcsvsaver.la

DYNAMIC_MODULES += \
librcssstdtimer.la \
librcsssynctimer.la \
libstdoutsaver.la \
libcsvsaver.la

# if BUILD_MYSQL_SAVER

# pkglib_LTLIBRARIES += libmysqlsaver.la
# DYNAMIC_MODULES += libmysqlsaver.la

# libmysqlsaver_la_SOURCES = mysqlsaver.cpp

# libmysqlsaver_la_LDFLAGS = -module

# libmysqlsaver_la_LIBADD = @MYSQLLIB@ librcssresultsaver.la

# endif

librcsstimer_la_SOURCES = \
timer.cpp timer.h

librcssstdtimer_la_SOURCES = \
stdtimer.h \
stdtimer.cc
librcssstdtimer_la_LDFLAGS = -module

librcssstdtimer_la_LIBADD = librcsstimer.la librcssserverparam.la

librcsssynctimer_la_SOURCES = \
synctimer.h \
synctimer.cc

librcsssynctimer_la_LIBADD = librcsstimer.la librcssserverparam.la

librcsssynctimer_la_LDFLAGS = -module


librcssresultsaver_la_SOURCES = resultsaver.cpp resultsaver.hpp


libstdoutsaver_la_SOURCES = stdoutsaver.cpp

libstdoutsaver_la_LDFLAGS = -module

libstdoutsaver_la_LIBADD = librcssresultsaver.la

libcsvsaver_la_SOURCES = csvsaver.cpp

libcsvsaver_la_LDFLAGS = -module

libcsvsaver_la_LIBADD = -lrcssconfparser librcssresultsaver.la


pkginclude_HEADERS = \
pcombuilder.h \
pcomparser.h \
player_command_tok.h \
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
serverparam.h \
region.h \
coach_lang_par.h \
vector.h \
visitor.h \
utility.h \
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


librcssutil_la_LDFLAGS = -version-info 0:0:0
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


librcssutil_la_SOURCES = \
utility.cpp


librcssserverparam_la_LDFLAGS = -version-info 3:0:1
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


librcssserverparam_la_SOURCES = \
serverparam.cpp \
serverparam.h


##librcssserverparam_la_LIBADD = -lrcssconfparser -lrcsslib librcssplayerparam.la librcssutil.la


librcssplayerparam_la_LDFLAGS = -version-info 3:0:1
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


librcssplayerparam_la_SOURCES = \
playerparam.cpp \
playerparam.h

##librcssplayerparam_la_LIBADD = -lrcssconfparser -lrcsslib \
##librcssutil.la


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
clangmsgbuilder.cc \
clangparser.cpp \
clangmsg.cc \
clangmetamsg.cc \
clangfreeformmsg.cc \
clangunsuppmsg.cc \
clangrulemsg.cc \
clangdelmsg.cc \
clanginfomsg.cc \
clangadvicemsg.cc \
clangdefmsg.cc \
clangaction.cc \
coach_lang_comp.cpp \
coach_lang_par.yy \
coach_lang_tok.cc \
region.cpp \
visitor.cc \
clangutil.cc \
cond.cc \
compop.cc \
arithop.cc \
rule.cc

##librcssclangparser_la_LIBADD = -lrcssconfparser -lrcsslib librcssresultsaver.la librcssserverparam.la

librcsspcomparser_la_SOURCES = \
player_command_parser.yy \
player_command_tok.cc





bin_PROGRAMS = \
rcssserver @RCSSCLIENT@

bin_SCRIPTS = rcsoccersim


BUILT_SOURCES = libimports.cpp \
coach_lang_par.h \
coach_lang_tok.cc \
player_command_tok.cc \
player_command_parser.h

EXTRA_DIST = \
coach_lang_tok.ll \
player_command_tok.ll \
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
player.h \
player.cc \
monitor.h \
monitor.cc \
arm.h \
random.h \
audio.h \
audio.cc \
serializer.h \
serializer.cc \
serializerplayerstdv1.h \
serializerplayerstdv1.cc \
serializerplayerstdv7.h \
serializerplayerstdv7.cc \
serializerplayerstdv8.h \
serializerplayerstdv8.cc \
serializercoachstdv1.h \
serializercoachstdv1.cc \
serializercoachstdv7.h \
serializercoachstdv7.cc \
serializercoachstdv8.h \
serializercoachstdv8.cc \
serializeronlinecoachstdv1.h \
serializeronlinecoachstdv1.cc \
serializeronlinecoachstdv6.h \
serializeronlinecoachstdv6.cc \
serializeronlinecoachstdv7.h \
serializeronlinecoachstdv7.cc \
serializeronlinecoachstdv8.h \
serializeronlinecoachstdv8.cc \
serializercommonstdv1.h \
serializercommonstdv1.cc \
serializercommonstdv7.h \
serializercommonstdv7.cc \
serializercommonstdv8.h \
serializercommonstdv8.cc \
referee.h \
referee.cc \
xpmholder.h \
visual.h \
visual.cc \
xpmholder.h \
timer.h \
timeable.h \
bodysender.h \
bodysender.cc \
fullstatesender.h \
fullstatesender.cc \
initsender.h \
initsender.cc \
observer.h \
sender.h


rcssserver_LDADD = $(LLLIBS) $(PRELOAD_MODULES) \
-lrcsstimer \
-lrcssclangparser \
-lrcsspcomparser \
-lrcssresultsaver \
-lrcssplayerparam \
-lrcssserverparam \
-lrcssutil \
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
	$(AWK) '/#include <FlexLexer\.h>/ { print "#include \"$*.hpp\""; getline; } \
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

check_PROGRAMS = \
playerparamtest \
serverparamtest

TESTS = \
moduletest.sh \
playerparamtest \
serverparamtest

moduletest.sh: Makefile.am
	echo "#! /bin/sh" > $@
	for module in $(module_LTLIBRARIES); do \
		echo "echo \"testing $$module\""; >> $@; \
		echo "@RCSSMODTEST@ $$module" >> $@; \
		echo "if test $$? -ne 0; then exit 1; fi" >> $@; \
	done
	chmod +x $@

playerparamtest_SOURCES = \
playerparamtest.cpp

playerparamtest_LDADD = librcssplayerparam.la \
-lrcssconfparser \
-lrcsslib

serverparamtest_SOURCES = \
serverparamtest.cpp

serverparamtest_LDADD = librcssserverparam.la \
-lrcssplayerparam \
-lrcssconfparser \
-lrcsslib


@INCLUDE_MK@
