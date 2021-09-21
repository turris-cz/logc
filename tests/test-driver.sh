#!/bin/bash
# The support provided by Meson for running tests in valgrind consist of
# wrapping test calls with string provided by user. This has few issues:
# * There is not standard way to set default arguments for Valgrind
# * The valgrind is used on suite code that is not ours at all. The issue here
#   is with test suites based on shell.
# The solution this script presents is to simply ignore wrap functionality and
# provide a way for user to enable valgrind wrapping in the correct level trough
# environment variable.
set -eu
################################################################################
declare -A default_valgrind_args
declare default_valgrind_args_common
default_valgrind_args_common+='--track-fds=yes '
default_valgrind_args_common+='--trace-children=yes '
default_valgrind_args_common+='--child-silent-after-fork=no '

default_valgrind_args[memcheck]="$default_valgrind_args_common"
default_valgrind_args[memcheck]+="--leak-check=full "
default_valgrind_args[memcheck]+="--show-leak-kinds=definite,indirect,possible "
default_valgrind_args[memcheck]+="--track-origins=yes "

default_valgrind_args[helgrind]="$default_valgrind_args_common"

default_valgrind_args[drd]="$default_valgrind_args_common"
################################################################################


usage() {
	echo "Usage: $0 [OPTION].. BINARY [ARG].." >&2
}
help() {
	usage
	{
	echo "This script runs given binary either directly or trough valgrind"
	echo "depending on its configuration."
	echo
	echo "Options:"
	echo "  -h     Print this help text"
	echo "  -n     Do not wrap with valgrind but rather pass it as environment variable"
	echo "  -v BIN Valgrind binary to use"
	echo "  -a ARG Add given argument as valgrind argument"
	echo "Environment variables:"
	echo "  VALGRIND=TOOL: run tests with valgrind with given tool"
	} >&2
}

declare -a valgrind_args
valgrind="valgrind"
no_wrap="n"
while getopts "hna:" opt; do
	case "$opt" in
		h)
			help
			exit 0
			;;
		n)
			no_wrap="y"
			;;
		a)
			valgrind_args+=("$OPTARG")
			;;
		*)
			usage
			exit 1
			;;
	esac
done
shift $((OPTIND - 1))
[ "$#" -gt 0 ] || {
	usage
	exit 1
}

if [[ -v VALGRIND ]]; then
	if [[ "$no_wrap" == "y" ]]; then
		export VALGRIND="$valgrind ${default_valgrind_args[$VALGRIND]} ${valgrind_args[*]} --tool=$VALGRIND"
		exec "$@"
	else
		exec "$valgrind" ${default_valgrind_args[$VALGRIND]} "${valgrind_args[@]}" --tool="$VALGRIND" -- "$@"
	fi
else
	exec "$@"	
fi
