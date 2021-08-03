#!/usr/bin/env bats
set -eu

setup() {
	if [ -z "${BATS_TEST_TMPDIR:-}" ]; then
		BATS_TEST_TMPDIR="$(mktemp -d)"
		export BATS_TEST_TMPDIR
		export BATS_TEST_TMPDIR_CUSTOM="y"
	fi
}

teardown() {
	if [ "${BATS_TEST_TMPDIR_CUSTOM:-}" = "y" ]; then
		rm -rf "$BATS_TEST_TMPDIR"
	fi
}

foo() {
	${LOG_COMPILER:-} ${LOG_COMPILER:+ --log-fd=3} \
		"${BUILD_DIR:-.}/foo" "$@"
}

@test "help" {
	foo --help
}

@test "null" {
	[ "$(foo -f /dev/null </dev/null)" == "0" ]
}

here_foo() {
	foo -f /dev/null <<-EOF
		fee: ignored
		foo: counted once
		foo: coutned twice
		fee: again ignored
	EOF
}
@test "here" {
	[ "$(here_foo)" == "2" ]
}

@test "file" {
	file="$BATS_TEST_TMPDIR/file"
	cat >"$file" <<-EOF
		foo: once
		fee: none
		foo: two
		foo: three
	EOF
	[ "$(foo -f /dev/null "$file")" == "3" ]
}

@test "missing" {
	local ec=0
	local error
	error="$(foo -f /dev/null /dev/missing 2>&1)" || ec=$?
	[ "$ec" -eq 1 ]
	[ "$error" == "ERROR: Can't open input file '/dev/missing': No such file or directory" ]
}
