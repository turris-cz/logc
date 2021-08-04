TESTS =
check_PROGRAMS =

unittests_common = \
	%reldir%/unittests.c \
	%reldir%/logc_fixtures.h \
	%reldir%/logc_fixtures.c
unittests_common_cflags = \
	-isystem '$(srcdir)/include' \
	$(CHECK_CFLAGS) \
	${CODE_COVERAGE_CFLAGS}
unittests_common_ldadd = \
	$(builddir)/liblogc.la \
	$(CHECK_LIBS) \
	${CODE_COVERAGE_LIBS} \
	%reldir%/libfakesyslog.la

TESTS += %reldir%/unittests-logc
check_PROGRAMS += %reldir%/unittests-logc
%canon_reldir%_unittests_logc_SOURCES = \
	${unittests_common} \
	%reldir%/logc.c \
	%reldir%/logc_bind.c \
	%reldir%/logc_asserts.c \
	%reldir%/logc_formats.c \
	%reldir%/logc_syslog.c
%canon_reldir%_unittests_logc_CFLAGS = \
	${unittests_common_cflags}
%canon_reldir%_unittests_logc_LDADD = \
	${unittests_common_ldadd}
linted_files += $(%canon_reldir%_unittests_logc_SOURCES)

TESTS += %reldir%/unittests-logc_argp
check_PROGRAMS += %reldir%/unittests-logc_argp
%canon_reldir%_unittests_logc_argp_SOURCES = \
	${unittests_common} \
	%reldir%/logc_argp.c
%canon_reldir%_unittests_logc_argp_CFLAGS = \
	${unittests_common_cflags}
%canon_reldir%_unittests_logc_argp_LDADD = \
	${unittests_common_ldadd} \
	$(builddir)/liblogc_argp.la
linted_files += $(%canon_reldir%_unittests_logc_argp_SOURCES)

TESTS += %reldir%/unittests-logc_config
check_PROGRAMS += %reldir%/unittests-logc_config
%canon_reldir%_unittests_logc_config_SOURCES = \
	${unittests_common} \
	%reldir%/logc_config.c
%canon_reldir%_unittests_logc_config_CFLAGS = \
	${unittests_common_cflags} \
	$(libconfig_CFLAGS)
%canon_reldir%_unittests_logc_config_LDADD = \
	${unittests_common_ldadd} \
	$(libconfig_LIBS) \
	$(builddir)/liblogc_config.la
linted_files += $(%canon_reldir%_unittests_logc_config_SOURCES)


lib_LTLIBRARIES += %reldir%/libfakesyslog.la
%canon_reldir%_libfakesyslog_la_SOURCES = \
	%reldir%/fakesyslog.h \
	%reldir%/fakesyslog.c
%canon_reldir%_libfakesyslog_la_CFLAGS = $(CHECK_CFLAGS)
%canon_reldir%_libfakesyslog_la_LIBADD = $(CHECK_LIBS)
linted_files += $(%canon_reldir%_libfakesyslog_la_SOURCES)
 


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
