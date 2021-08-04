lib_LTLIBRARIES += liblogc_config.la
pkgconfig_DATA += %reldir%/logc_config.pc
EXTRA_DIST += %reldir%/liblogc_config.version

include_HEADERS += \
	include/logc_config.h
liblogc_config_la_SOURCES = \
	%reldir%/logc_config.c \
	%reldir%/setting_type.c \
	%reldir%/setting_path.c
liblogc_config_la_CFLAGS = \
	-isystem '$(srcdir)/include' \
	${libconfig_CFLAGS} \
	${CODE_COVERAGE_CFLAGS}
liblogc_config_la_LIBADD = \
	${libconfig_LIBS} \
	${CODE_COVERAGE_LIBS}
liblogc_config_la_LDFLAGS = \
	--version-script='$(srcdir)/%reldir%/liblogc_config.version' \
	-version-info 0:0:0


linted_files += $(liblogc_config_la_SOURCES)
