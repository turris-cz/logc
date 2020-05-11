ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = liblogc.la liblogc_argp.la

liblogc_la_SOURCES = logc.c
liblogc_la_CFLAGS = -isystem %reldir%

liblogc_argp_la_SOURCES = logc_argp.c
liblogc_argp_la_CFLAGS = -isystem %reldir%

include_HEADERS = logc.h logc_util.h logc_argp.h
pkgconfig_DATA = logc.pc logc_argp.pc


gperf_files = envlog.gperf.c

liblogc_la-logc.$(OBJEXT): envlog.gperf.c

gperf_V = $(gperf_V_@AM_V@)
gperf_V_ = $(gperf_V_@AM_DEFAULT_V@)
gperf_V_0 = @echo GPERF $@;

%.gperf.c: %.gperf
	$(gperf_V)$(GPERF) --output-file=$@ $<

CLEANFILES = $(gperf_files)

.PHONY: gen-gperf
gen-gperf: $(gperf_files)


include $(srcdir)/tests/Makefile.am

## Linting
lint_V = $(lint_V_@AM_V@)
lint_V_ = $(lint_V_@AM_DEFAULT_V@)
lint_V_0 = @echo LINT $@;

lint: $(liblogc_la_SOURCES) $(include_HEADERS)
	$(lint_V)$(CPPCHECK) --error-exitcode=1 \
		--enable=warning \
		--std=c11 \
		--template='{file}:{line},{severity},{id},{message}' \
		--inline-suppr \
		 $^
