lib_LTLIBRARIES += liblogc_argp.la
include_HEADERS += include/logc_argp.h
pkgconfig_DATA += %reldir%/logc_argp.pc

liblogc_argp_la_SOURCES = %reldir%/logc_argp.c
liblogc_argp_la_CFLAGS = \
	-isystem include \
	${CODE_COVERAGE_CFLAGS}
liblogc_argp_la_LDFLAGS = --version-script=%reldir%/liblogc_argp.version
liblogc_argp_la_LIDADD = ${CODE_COVERAGE_LIBS}

