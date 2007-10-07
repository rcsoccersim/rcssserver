ACLOCAL_AMFLAGS=-I m4

LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool:	$(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

SUBDIRS = src \
.

EXTRA_DIST = \
BUGS \
AUTHORS \
Acknowledgement \
ChangeLog \
NEWS \
README \
INSTALL \
coach_lang_grammar \
tools/clangparser/Licence \
tools/clangparser/main.C \
tools/clangparser/Makefile \
tools/clangparser/README \
tools/clangparser/test.input \
tools/exteamname/exteamname.c \
tools/exteamname/exteamname.pro \
tools/exteamname/Makefile \
tools/exteamname/test.sh \
tools/striplog/Makefile \
tools/striplog/striplog.c \
tools/striplog/striplog.pro \
missingflex

CLEANFILES = \
*~ \
core \
*.tar.gz \
*.tmp

DISTCLEANFILES = @INCLUDE_MK_FILE@

@INCLUDE_MK@
