TESTS = %reldir%/unittests


check_PROGRAMS = %reldir%/unittests
%canon_reldir%_unittests_SOURCES = \
	%reldir%/unittests.c \
	%reldir%/logc_fixtures.h \
	%reldir%/logc_fixtures.c \
	%reldir%/logc.c \
	%reldir%/logc_bind.c \
	%reldir%/logc_config.c \
	%reldir%/logc_argp.c \
	%reldir%/logc_asserts.c \
	%reldir%/logc_formats.c \
	%reldir%/logc_syslog.c
%canon_reldir%_unittests_CFLAGS = \
	-isystem '$(srcdir)/include' \
	$(libconfig_CFLAGS) \
	$(CHECK_CFLAGS) \
	${CODE_COVERAGE_CFLAGS}
%canon_reldir%_unittests_LDADD = \
	$(builddir)/liblogc.la \
	$(builddir)/liblogc_argp.la \
	$(builddir)/liblogc_config.la \
	$(libconfig_LIBS) \
	$(CHECK_LIBS) \
	${CODE_COVERAGE_LIBS} \
	%reldir%/libfakesyslog.la

lib_LTLIBRARIES += %reldir%/libfakesyslog.la
%canon_reldir%_libfakesyslog_la_SOURCES = \
	%reldir%/fakesyslog.h \
	%reldir%/fakesyslog.c
%canon_reldir%_libfakesyslog_la_CFLAGS = $(CHECK_CFLAGS)
%canon_reldir%_libfakesyslog_la_LIBADD = $(CHECK_LIBS)


linted_files += $(%canon_reldir%_unittests_SOURCES)


LOG_DRIVER = \
	env AM_TAP_AWK='$(AWK)' TEST_OUTPUT_TAP="/dev/stdout" \
	'$(SHELL)' '$(srcdir)/.aux/tap-driver.sh'

## Valgrind
VALGRIND_memcheck_FLAGS = \
	--leak-check=full \
	--show-leak-kinds=definite,indirect,possible \
	--track-fds=yes \
	--track-origins=yes \
	--trace-children=yes \
	--child-silent-after-fork=no
@VALGRIND_CHECK_RULES@

# Rules generated for valgrind are for some reason called *-am. This is just an alias
check-valgrind: check-valgrind-am
define check_valgrind_rule
check-valgrind-$(1): check-valgrind-$(1)-am
endef
$(foreach tool,$(valgrind_tools),$(eval $(call check_valgrind_rule,$(tool))))

## Coverage
CODE_COVERAGE_LCOV_OPTIONS := --no-external
CODE_COVERAGE_GENHTML_OPTIONS := --prefix $(dir $(abs_top_builddir))

include $(srcdir)/aminclude_static.am

clean-local: code-coverage-clean
distclean-local: code-coverage-dist-clean
