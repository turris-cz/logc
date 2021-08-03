ACLOCAL_AMFLAGS = -I .m4

bin_PROGRAMS =
lib_LTLIBRARIES =
include_HEADERS =
pkgconfig_DATA =
EXTRA_DIST =
source_files =
gperf_files =

include src/Makefile.am
include libfoo/Makefile.am


include utils/gperf.am

include tests/Makefile.am

source_files += $(include_HEADERS)
if ENABLE_LINTERS
include utils/lint.am
endif
