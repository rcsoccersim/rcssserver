ACLOCAL_AMFLAGS=-I m4

LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool:	$(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

SUBDIRS = src \
.

EXTRA_DIST = \
missingflex

CLEANFILES = \
*~ \
core \
*.tar.gz \
*.tmp

#DISTCLEANFILES = @INCLUDE_MK_FILE@

#@INCLUDE_MK@
