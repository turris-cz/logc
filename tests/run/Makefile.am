BATS_TESTS = \
	%reldir%/foo.bats

EXTRA_DIST += ${BATS_TESTS}
TESTS += ${BATS_TESTS}
check_PROGRAMS += foo


BATS_LOG_DRIVER = \
	env AM_TAP_AWK='$(AWK)' '$(SHELL)' '$(srcdir)/.aux/tap-driver.sh'
BATS_LOG_COMPILER = '$(BATS)' --tap
