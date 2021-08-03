AC_INIT([c-template],[0.1.0],[packaging@turris.cz])
AC_CONFIG_MACRO_DIRS([.m4])
AC_CONFIG_AUX_DIR([.aux])

AM_INIT_AUTOMAKE([
  foreign silent-rules subdir-objects
  -Wall -Wno-portability
  dist-zip dist-xz
])
AM_SILENT_RULES([yes])

AC_PROG_CC
AC_USE_SYSTEM_EXTENSIONS
AM_PROG_AR
LT_INIT
PKG_INSTALLDIR

AX_CHECK_COMPILE_FLAG([-std=c11], , AC_MSG_ERROR([Compiler with C11 standard support is required]))
AX_APPEND_FLAG([-std=c11])

dnl gperf
AC_CHECK_FILE([${0%/*}/bootstrap],[
  AC_PATH_PROG([GPERF], [gperf])
  AS_IF([test -z "$GPERF"], [AC_MSG_ERROR([Missing gperf generator])])
])
AM_CONDITIONAL([GPERF_GENERATE], [test -n "$GPERF"])


dnl Library dependencies
PKG_CHECK_MODULES([logc], [logc >= 0.4.0 logc_argp logc_config])
PKG_CHECK_MODULES([libconfig], [libconfig])
AC_LINK_IFELSE(
  [AC_LANG_PROGRAM([#include <argp.h>],[argp_parse(0,1,NULL,0,0,0);])],,
  [argp_standalone_required="yes"]
)
AS_IF([test "$argp_standalone_required" = "yes"],
  AC_CHECK_LIB([argp], [argp_parse], , AC_MSG_ERROR([Unable to find libargp])))


dnl Options from environment
AC_ARG_VAR([defconfig], [Default path to the configuration file])
AS_IF([test -z "${defconfig}"], [defconfig="/etc/turris/template.cfg"])
AC_DEFINE_UNQUOTED([DEFAULT_CONFIG_FILE], ["${defconfig}"],
  [Default path to the configuration file])



dnl Tests
AC_ARG_ENABLE([tests], AS_HELP_STRING([--enable-tests],[Whether to enable unit tests]))
AM_CONDITIONAL([ENABLE_TESTS], [test "x$enable_tests" = "xyes"])
AS_IF([test "x$enable_tests" = "xyes"], [
  AC_REQUIRE_AUX_FILE([tap-driver.sh])

  PKG_CHECK_MODULES([CHECK], [check >= 0.11])
  AC_LINK_IFELSE(
    [AC_LANG_PROGRAM([#include <obstack.h>],[obstack_free(NULL, NULL);])],,
    [missing_obstack="yes"]
  )
  AS_IF([test "$missing_obstack" = "yes"],
    AC_CHECK_LIB([obstack], [obstack_printf], , AC_MSG_ERROR([Unable to find obstack])))

  AC_PATH_PROG([BATS], [bats])
  AS_IF([test -z "$BATS"], [AC_MSG_ERROR([Missing bats testing system])])
],)
AX_VALGRIND_CHECK

dnl Linters
AC_ARG_ENABLE([linters], AS_HELP_STRING([--enable-linters],[Whether to enable code linting support]))
AM_CONDITIONAL([ENABLE_LINTERS], [test "x$enable_linters" = "xyes"])
AS_IF([test "x$enable_linters" = "xyes"], [
  AC_PATH_PROG([CPPCHECK], [cppcheck])
  AS_IF([test -z "$CPPCHECK" ], AC_MSG_ERROR([Missing linter cppcheck]))
  AC_PATH_PROG([FLAWFINDER], [flawfinder])
  AS_IF([test -z "$FLAWFINDER" ], AC_MSG_ERROR([Missing linter flawfinder]))
])

dnl Coverage
AX_CODE_COVERAGE


AC_CONFIG_FILES([Makefile libfoo/foo.pc])

AC_OUTPUT
