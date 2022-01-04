AC_INIT([logc], [0.4.0], [cynerd@email.cz])
AC_CONFIG_MACRO_DIRS([.m4])
AC_CONFIG_AUX_DIR([.aux])

AM_INIT_AUTOMAKE([
  foreign silent-rules subdir-objects
  -Wall -Wno-portability
  dist-zip dist-xz
])
AM_SILENT_RULES([yes])

AC_PROG_CC
AC_GNU_SOURCE
AM_PROG_AR
LT_INIT
PKG_INSTALLDIR

AX_CHECK_COMPILE_FLAG([-std=c11], , AC_MSG_ERROR([Compiler with C11 standard support is required]))
AX_APPEND_FLAG([-std=c11])

AS_IF([test -x "${0%/*}/bootstrap" ],[
  AC_PATH_PROG([GPERF], [gperf])
  AS_IF([test -z "$GPERF"], [AC_MSG_ERROR([Missing gperf generator])])
])
AM_CONDITIONAL([GPERF_GENERATE], [test -n "$GPERF"])

AC_ARG_WITH([libconfig],
  [AS_HELP_STRING([--with-libconfig], [build logc_config integration library @<:@default=check@:>@])],
  [], [with_libconfig=check]
)
AS_IF([test "x$with_libconfig" != xno],
  [PKG_CHECK_MODULES([libconfig], [libconfig],
    [with_libconfig=yes],
    [AS_IF([test "x$with_libconfig" != xcheck], [AC_MSG_FAILURE([Unable to find libconfig, logc_config won't be build])])]
  )]
)
AM_CONDITIONAL(LIBCONFIG, [test "x$with_libconfig" = "xyes"])


dnl Tests
AC_ARG_ENABLE([tests], AC_HELP_STRING([--enable-tests], [Whether to enable unit tests]))
AM_CONDITIONAL([ENABLE_TESTS], [test "x$enable_tests" = "xyes"])
AS_IF([test "x$enable_tests" = "xyes"], [
  PKG_CHECK_MODULES([CHECK], [check >= 0.11])
  AC_REQUIRE_AUX_FILE([tap-driver.sh])
])
AX_VALGRIND_CHECK

dnl Linters
AC_ARG_ENABLE([linters], AC_HELP_STRING([--enable-linters], [Whether to enable code linting support (cppcheck)]))
AM_CONDITIONAL([ENABLE_LINTERS], [test "x$enable_linters" = "xyes"])
AS_IF([test "x$enable_linters" = "xyes"], [
  AC_PATH_PROG([CPPCHECK], [cppcheck])
  AS_IF([test -z "$CPPCHECK" ], AC_MSG_ERROR([Missing linter cppcheck]))
])

dnl Coverage
AX_CODE_COVERAGE


AC_CONFIG_FILES([
  Makefile
  logc/logc.pc
  logc_argp/logc_argp.pc
  logc_config/logc_config.pc
 ])

AC_OUTPUT
