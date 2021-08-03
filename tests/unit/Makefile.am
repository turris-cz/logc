unittests_common = \
	%reldir%/unittests.h \
	%reldir%/unittests.c \
	%reldir%/fixtures.h \
	%reldir%/fixtures.c

TESTS += %reldir%/unittests-libfoo
check_PROGRAMS += %reldir%/unittests-libfoo
%canon_reldir%_unittests_libfoo_SOURCES = \
	${unittests_common} \
	%reldir%/count.c
%canon_reldir%_unittests_libfoo_CFLAGS = \
	-iquote '$(srcdir)/libfoo' \
	-isystem '$(srcdir)/include' \
	$(CHECK_FLAGS)
%canon_reldir%_unittests_libfoo_LDADD = \
	$(builddir)/libfoo.la \
	$(CHECK_LIBS)
source_files += $(%canon_reldir%_unittests_libfoo_SOURCES)

TESTS += %reldir%/unittests-foo
check_PROGRAMS += %reldir%/unittests-foo
%canon_reldir%_unittests_foo_SOURCES = \
	${unittests_common} \
	${foo_sources} \
	%reldir%/config.c
%canon_reldir%_unittests_foo_CFLAGS = \
	-iquote '$(srcdir)/src' \
	${foo_CFLAGS} \
	$(CHECK_FLAGS)
%canon_reldir%_unittests_foo_LDADD = \
	${foo_LDADD} \
	$(CHECK_LIBS)
source_files += $(%canon_reldir%_unittests_foo_SOURCES)
