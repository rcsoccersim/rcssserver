ACLOCAL_AMFLAGS=-I m4

LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool:	$(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

SUBDIRS = \
	rcssbase \
	src \
	.

EXTRA_DIST = 

CLEANFILES = \
	*~ \
	core \
	*.tar.gz \
	*.tmp

#DISTCLEANFILES =
