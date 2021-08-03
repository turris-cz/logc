bin_PROGRAMS += foo

foo_sources = \
	%reldir%/config.h \
	%reldir%/config.c \
	%reldir%/log.h \
	%reldir%/log.c

foo_SOURCES = \
	%reldir%/foo.c \
	${foo_sources}

foo_CFLAGS = \
	-isystem '$(srcdir)/include' \
	${logc_CFLAGS} \
	${libconfig_CFLAGS} \
	${CODE_COVERAGE_CFLAGS}
foo_LDADD = \
	${logc_LIBS} \
	${libconfig_LIBS} \
	libfoo.la \
	${CODE_COVERAGE_LIBS}

source_files += $(foo_SOURCES)
