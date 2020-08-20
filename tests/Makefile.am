TESTS = %reldir%/unittests


check_PROGRAMS = %reldir%/unittests
%canon_reldir%_unittests_SOURCES = \
	%reldir%/unittests.c \
	%reldir%/logc.c \
	%reldir%/logc_formats.c \
	%reldir%/logc_argp.c
%canon_reldir%_unittests_CFLAGS = \
	$(CHECK_FLAGS)
%canon_reldir%_unittests_LDADD = \
	$(builddir)/liblogc.la \
	$(builddir)/liblogc_argp.la \
	$(CHECK_LIBS)


LOG_DRIVER = \
	env AM_TAP_AWK='$(AWK)' TEST_OUTPUT_TAP=y \
	'$(SHELL)' '$(srcdir)/aux/tap-driver.sh'

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
include $(srcdir)/aminclude_static.am

clean-local: code-coverage-clean
distclean-local: code-coverage-dist-clean
