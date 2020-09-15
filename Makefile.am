ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = liblogc.la liblogc_argp.la

liblogc_la_SOURCES = logc.c
liblogc_la_CFLAGS = \
	-isystem %reldir% \
	$(CODE_COVERAGE_CFLAGS)
liblogc_la_LIBS = $(CODE_COVERAGE_LIBS)

liblogc_argp_la_SOURCES = logc_argp.c
liblogc_argp_la_CFLAGS = \
	-isystem %reldir% \
	$(CODE_COVERAGE_CFLAGS)
liblogc_argp_la_LIBS = $(CODE_COVERAGE_LIBS)

include_HEADERS = logc.h logc_util.h logc_assert.h logc_argp.h
pkgconfig_DATA = logc.pc logc_argp.pc


gperf_files = envlog.gperf.c format.gperf.c
$(liblogc_la_OBJECTS): $(gperf_files)
include $(srcdir)/gperf.am


DIST_TARGETS := dist-gzip dist-zip


linted_files = $(liblogc_la_SOURCES) $(include_HEADERS)
include $(srcdir)/lint.am

include $(srcdir)/tests/Makefile.am
