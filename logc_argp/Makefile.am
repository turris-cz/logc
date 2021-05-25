lib_LTLIBRARIES += liblogc_argp.la
pkgconfig_DATA += %reldir%/logc_argp.pc
EXTRA_DIST += %reldir%/liblogc_argp.version

include_HEADERS += \
	include/logc_argp.h
liblogc_argp_la_SOURCES = \
	%reldir%/logc_argp.c
liblogc_argp_la_CFLAGS = \
	-isystem '$(srcdir)/include' \
	${CODE_COVERAGE_CFLAGS}
liblogc_argp_la_LIBADD = \
	${CODE_COVERAGE_LIBS}
liblogc_argp_la_LDFLAGS = \
	--version-script='$(srcdir)/%reldir%/liblogc_argp.version' \
	-version-info 0:0:0


linted_files += $(liblogc_argp_la_SOURCES)
