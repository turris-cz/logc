lib_LTLIBRARIES += liblogc.la
pkgconfig_DATA += %reldir%/logc.pc
EXTRA_DIST += %reldir%/liblogc.version

include_HEADERS += \
	include/logc.h \
	include/logc_util.h \
	include/logc_assert.h \
	include/logc_asserts.h
liblogc_la_SOURCES = \
	%reldir%/bind.c \
	%reldir%/log.h \
	%reldir%/log.c \
	%reldir%/level.h \
	%reldir%/level.c \
	%reldir%/origin.c \
	%reldir%/output.h \
	%reldir%/output.c \
	%reldir%/format.h \
	%reldir%/format.c \
	%reldir%/syslog.c \
	%reldir%/util.h
liblogc_la_CFLAGS = \
	-isystem '$(srcdir)/include' \
	${CODE_COVERAGE_CFLAGS}
liblogc_la_LIBADD = \
	${CODE_COVERAGE_LIBS}
liblogc_la_LDFLAGS = \
	-Wl,--version-script='$(srcdir)/%reldir%/liblogc.version' \
	-version-info 0:0:0

logc_gperf = %reldir%/format.gperf.c
$(liblogc_la_OBJECTS): $(logc_gperf)
gperf_files += ${logc_gperf}


linted_files += $(liblogc_la_SOURCES)
