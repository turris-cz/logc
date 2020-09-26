AC_INIT([logc], [0.1.0], [cynerd@email.cz])
AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_AUX_DIR([aux])

AM_INIT_AUTOMAKE([foreign silent-rules subdir-objects -Wall -Wno-portability])
AM_SILENT_RULES([yes])

AC_PROG_CC
AC_GNU_SOURCE
AM_PROG_AR
LT_INIT
PKG_INSTALLDIR

AC_PATH_PROG([GPERF], [gperf])
AS_IF([test -z "$GPERF"], [AC_MSG_ERROR([Missing gperf generator])])


dnl Tests
AC_ARG_ENABLE([tests], AC_HELP_STRING([--enable-tests], [Disable tests]))
AM_CONDITIONAL([ENABLE_TESTS], [test "x$enable_tests" = "xyes"])
AS_IF([test "x$enable_tests" = "xyes"], [
  PKG_CHECK_MODULES([CHECK], [check >= 0.11])
  AC_REQUIRE_AUX_FILE([tap-driver.sh])
])
AX_VALGRIND_CHECK

dnl Linters
AC_ARG_ENABLE([linters], AC_HELP_STRING([--enable-linters], [Disable linters (cppcheck and luacheck)]))
AM_CONDITIONAL([ENABLE_TESTS], [test "x$enable_linters" = "xyes"])
AS_IF([test "x$enable_linters" = "xyes"], [
  AC_PATH_PROG([CPPCHECK], [cppcheck])
])

dnl Coverage
AX_CODE_COVERAGE


AC_CONFIG_FILES([Makefile logc.pc logc_argp.pc])

AC_OUTPUT
