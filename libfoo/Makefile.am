lib_LTLIBRARIES += libfoo.la
pkgconfig_DATA += %reldir%/foo.pc
EXTRA_DIST += %reldir%/libfoo.version

include_HEADERS += \
	include/foo.h
libfoo_la_SOURCES = \
	%reldir%/log.h \
	%reldir%/log.c \
	%reldir%/count.c
libfoo_la_CFLAGS = \
	-iquote '$(builddir)/%reldir%' \
	-isystem '$(srcdir)/include' \
	${logc_CFLAGS} \
	${CODE_COVERAGE_CFLAGS}
libfoo_la_LIBADD = \
	${logc_LIBS} \
	${CODE_COVERAGE_LIBS}
libfoo_la_LDFLAGS = \
	-Wl,--version-script='$(srcdir)/%reldir%/libfoo.version' \
	-version-info 0:0:0

libfoo_gperf = %reldir%/match.gperf.c
$(libfoo_la_OBJECTS): $(libfoo_gperf)
gperf_files += ${libfoo_gperf}


source_files += $(libfoo_la_SOURCES)
