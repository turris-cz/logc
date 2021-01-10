lib_LTLIBRARIES += liblogc.la
include_HEADERS += \
	include/logc.h \
	include/logc_util.h \
	include/logc_assert.h
pkgconfig_DATA += %reldir%/logc.pc

liblogc_la_SOURCES = \
	%reldir%/log.c \
	%reldir%/level.c \
	%reldir%/origin.c \
	%reldir%/output.c \
	%reldir%/format.c
liblogc_la_CFLAGS = \
	-isystem %reldir%/../include \
	${CODE_COVERAGE_CFLAGS}
liblogc_la_LDFLAGS = --version-script=%reldir%/liblogc.version
liblogc_la_LIDADD = ${CODE_COVERAGE_LIBS}

logc_gperf = %reldir%/format.gperf.c
$(liblogc_la_OBJECTS): $(logc_gperf)
gperf_files += ${logc_gperf}
