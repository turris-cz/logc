ACLOCAL_AMFLAGS = -I .m4

lib_LTLIBRARIES =
include_HEADERS =
pkgconfig_DATA =
EXTRA_DIST =
gperf_files =


include $(srcdir)/logc/Makefile.am
include $(srcdir)/logc_argp/Makefile.am

include $(srcdir)/utils/gperf.am


linted_files = $(liblogc_la_SOURCES) $(include_HEADERS)
if ENABLE_LINTERS
include $(srcdir)/utils/lint.am
endif

if ENABLE_TESTS
include $(srcdir)/tests/Makefile.am
endif
