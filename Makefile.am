ACLOCAL_AMFLAGS=-I m4

LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool:	$(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

SUBDIRS = \
	rcss \
	src \
	.

EXTRA_DIST = \
	README.md \
	CMakeLists.txt \
	config.h.cmake

CLEANFILES = \
	*~ \
	core \
	*.tar.gz \
	*.tmp

#DISTCLEANFILES =
