// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020-2022, CZ.NIC z.s.p.o. (http://www.nic.cz/)
#include <signal.h>
#include <errno.h>
#include <signal.h>

#define SUITE "logc"
#include "unittests.h"


TEST_CASE(def_output) {}

TEST(def_output, simple_warning) {
	warning("This is warning!");

	fflush(stderr);
	ck_assert_str_eq(stderr_data, "WARNING:tlog: This is warning!\n");
}
END_TEST

#define ILL(LEVEL) (LL_##LEVEL + 3)
static const char *level_name[] = {
	[ILL(CRITICAL)] = "CRITICAL",
	[ILL(ERROR)] = "ERROR",
	[ILL(WARNING)] = "WARNING",
	[ILL(NOTICE)] = "NOTICE",
	[ILL(INFO)] = "INFO",
	[ILL(DEBUG)] = "DEBUG",
	[ILL(TRACE)] = "TRACE"
};
#undef ILL
#define ILL(LEVEL) (LEVEL + 3)

LOOP_TEST(def_output, check_default_level, LL_TRACE, LL_CRITICAL) {
	logc(tlog, _i, "This is message!");

	fflush(stderr);
	if (_i >= LL_NOTICE) {
		char *expected;
		int len = asprintf(&expected, "%s:tlog: This is message!\n", level_name[ILL(_i)]);
		ck_assert_str_eq(stderr_data, expected);
		ck_assert_int_eq(stderr_len, len);
		free(expected);
	} else
		ck_assert_str_eq(stderr_data, "");
}
END_TEST

LOOP_TEST(def_output, check_all_levels, LL_TRACE, LL_CRITICAL) {
	log_set_level(tlog, LL_TRACE);
	logc(tlog, _i, "This is message!");

	fflush(stderr);
	char *expected;
	int len = asprintf(&expected, "%s:tlog: This is message!\n", level_name[ILL(_i)]);
	ck_assert_str_eq(stderr_data, expected);
	ck_assert_int_eq(stderr_len, len);
	free(expected);
}
END_TEST

#undef ILL

TEST(def_output, app_log) {
	APP_LOG(tlog);
	log_error(log_tlog, "This is error!");

	fflush(stderr);
	ck_assert_str_eq(stderr_data, "ERROR: This is error!\n");
}
END_TEST

TEST(def_output, standard_error) {
	errno = ENOENT;
	error("This is error");
	ck_assert_int_eq(errno, 0); // call should reset errno

	fflush(stderr);
	char *expected;
	int len = asprintf(&expected, "ERROR:tlog: This is error: %s\n", strerror(ENOENT));
	ck_assert_str_eq(stderr_data, expected);
	ck_assert_int_eq(stderr_len, len);
	free(expected);
}
END_TEST

TEST(def_output, call_verbose) {
	log_verbose(tlog);
	log_verbose(tlog); // increase verbosity twice

	ck_assert_int_eq(log_level(tlog), LL_DEBUG);

	debug("This is debug!");
	trace("This is trace!"); // should not print

	fflush(stderr);
	ck_assert_str_eq(stderr_data, "DEBUG:tlog: This is debug!\n");
}
END_TEST

TEST(def_output, call_quiet) {
	log_quiet(tlog);
	log_quiet(tlog); // decrease verbosity twice

	ck_assert_int_eq(log_level(tlog), LL_ERROR);

	error("This is error!");
	warning("This is warning!"); // should not print

	fflush(stderr);
	ck_assert_str_eq(stderr_data, "ERROR:tlog: This is error!\n");
}
END_TEST

TEST(def_output, call_offset_level) {
	log_offset_level(tlog, -2);

	ck_assert_int_eq(log_level(tlog), LL_DEBUG);
}
END_TEST

TEST(def_output, disabled_def_output) {
	log_stderr_fallback(tlog, false);
	logc(tlog, _i, "This is message!");

	fflush(stderr);
	ck_assert_str_eq(stderr_data, "");
}
END_TEST

TEST(def_output, message_origin) {
	ck_assert(!log_use_origin(tlog));
	log_set_use_origin(tlog, true);
	ck_assert(log_use_origin(tlog));

	const int line = __LINE__ + 1;
	notice("foo");

	fflush(stderr);
	char *expected;
	int len = asprintf(&expected, "NOTICE:tlog(%s:%d,%s): foo\n", __FILE__, line, __func__);
	ck_assert_str_eq(stderr_data, expected);
	ck_assert_int_eq(stderr_len, len);
	free(expected);
}
END_TEST


TEST_CASE(would_log) {}

LOOP_TEST(would_log, check_would_log, LL_TRACE, LL_CRITICAL) {
	log_set_level(tlog, _i);

	for (enum log_message_level level = LL_TRACE; level < LL_CRITICAL; level++)
		ck_assert(log_would_log(tlog, level) == (level >= _i));
}
END_TEST


TEST_CASE(custom_format) {}

static const struct {
	const char *format;
	const char *out;
} custom_output_tests[] = {
	{"%m", "Message"},
	{"%n", "tlog"},
	{"%f", "tests/logc.c"},
	{"%i", "42"},
	{"%c", "function_name"},
	{"%e", ""},
	{"%(_empty%)", ""},
	{"%(_%e empty%)", ""},
	{"%(_ %m %)", " Message "},
	{"%(ctext%)", "text"},
	{"%(Ctext%)", ""},
	{"%(etext%)", "text"},
	{"%(Etext%)", ""},
	{"%(wtext%)", "text"},
	{"%(Wtext%)", ""},
	{"%(ntext%)", ""},
	{"%(Ntext%)", "text"},
	{"%(itext%)", ""},
	{"%(Itext%)", "text"},
	{"%(dtext%)", ""},
	{"%(Dtext%)", "text"},
	{"%(c%(e%(wtext%)%)%)", "text"},
	{"%(c%(e%(wtext", "text"}, // Verify that unterminated ifs are not an issue
	// Note: Output is to memstream not to terminal
	{"%(ttext%)", "text"},
	{"%(Ttext%)", ""},
	// Note: In default there should be no colors unless on terminal
	{"%(ptext%)", "text"},
	{"%(Ptext%)", ""},
	{"%%", "%"},
	{" %% ", " % "},
	{" %%%% ", " %% "},
	{" %x ", " x "}, // Any alone % is eaten up
};

ARRAY_TEST(custom_format, check_custom_outputs, custom_output_tests) {
	char *buf;
	size_t bufsiz;
	FILE *f = open_memstream(&buf, &bufsiz);
	log_add_output(tlog, f, 0, 0, _d.format);

	_logc(tlog, LL_NOTICE, "tests/logc.c", 42, "function_name", "Message");

	fclose(f);
	// Last character is always new line so we skip it
	ck_assert_mem_eq(buf, _d.out, bufsiz - 1);
	free(buf);

	ck_assert_int_eq(stderr_len, 0); // No output to stderr
}
END_TEST

static const struct {
	const char *format;
	int flags;
	const char *out;
} custom_output_flag_tests[] = {
	{"%(p%m%)", 0, "Message"}, // Output is memstream so in default no colors
	{"%(P%m%)", 0, ""},
	{"%(p%m%)", LOG_F_NO_COLORS, "Message"},
	{"%(P%m%)", LOG_F_NO_COLORS, ""},
	{"%(p%m%)", LOG_F_COLORS, ""},
	{"%(P%m%)", LOG_F_COLORS, "Message"},
	{"%(p%m%)", LOG_F_NO_COLORS & LOG_F_COLORS, "Message"}, // no colors has precedence over colors
	{"%(P%m%)", LOG_F_NO_COLORS & LOG_F_COLORS, ""},
};

ARRAY_TEST(custom_format, check_custom_outputs_flags, custom_output_flag_tests) {
	char *buf;
	size_t bufsiz;
	FILE *f = open_memstream(&buf, &bufsiz);
	log_add_output(tlog, f, _d.flags, 0, _d.format);

	notice("Message");

	fclose(f);
	// Last character is always new line so we skip it
	ck_assert_mem_eq(buf, _d.out, bufsiz - 1);
	free(buf);

	ck_assert_int_eq(stderr_len, 0); // No output to stderr
}
END_TEST

TEST(custom_format, check_custom_output_remove) {
	char *buf;
	size_t bufsiz;
	FILE *f = open_memstream(&buf, &bufsiz);

	log_add_output(tlog, f, LOG_F_AUTOCLOSE, 0, LOG_FORMAT_PLAIN);
	notice("output");
	ck_assert(log_rm_output(tlog, f));
	// No need to flush here as f is closed on log_rm_output and so flushed
	ck_assert_str_eq(buf, "tlog: output\n");
	free(buf);

	// Test default output as well to verify that we stopped using custom one
	notice("stderr");
	fflush(stderr);
	ck_assert_str_eq(stderr_data, "NOTICE:tlog: stderr\n");
}
END_TEST

TEST(custom_format, check_custom_output_twice) {
	char *buf;
	size_t bufsiz;
	FILE *f = open_memstream(&buf, &bufsiz);

	log_add_output(tlog, f, 0, 0, LOG_FORMAT_PLAIN);
	notice("plain");
	fflush(f);
	ck_assert_str_eq(buf, "tlog: plain\n");

	log_add_output(tlog, f, 0, 0, LOG_FORMAT_DEFAULT);
	notice("default");
	fflush(f);
	ck_assert_str_eq(buf, "tlog: plain\nNOTICE:tlog: default\n");

	fclose(f);
	free(buf);
}
END_TEST

TEST(custom_format, check_custom_output_wipe) {
	char *buf1, *buf2;
	size_t bufsiz1, bufsiz2;
	FILE *f1 = open_memstream(&buf1, &bufsiz1);
	FILE *f2 = open_memstream(&buf2, &bufsiz2);

	log_add_output(tlog, f1, LOG_F_AUTOCLOSE, 0, LOG_FORMAT_PLAIN);
	log_add_output(tlog, f2, LOG_F_AUTOCLOSE, 0, LOG_FORMAT_PLAIN);

	notice("Message");

	log_wipe_outputs(tlog);
	fflush(stderr);

	ck_assert_str_eq(buf1, "tlog: Message\n");
	ck_assert_str_eq(buf2, "tlog: Message\n");
	ck_assert_int_eq(stderr_len, 0);
	free(buf1);
	free(buf2);

	notice("Message");
	fflush(stderr);
	ck_assert_str_eq(stderr_data, "NOTICE:tlog: Message\n");
}
END_TEST

// We test here once with real file as memstream does not have fileno while real
// tmpfile does.
TEST(custom_format, check_custom_file_output) {
	FILE *f = tmpfile();

	log_add_output(tlog, f, 0, 0, LOG_FORMAT_PLAIN);
	notice("Message");
	log_wipe_outputs(tlog);

	char *line = NULL;
	size_t line_size = 0;
	rewind(f);
	ck_assert_int_ge(getline(&line, &line_size, f), 0);
	ck_assert_str_eq(line, "tlog: Message\n");
	ck_assert_int_eq(getline(&line, &line_size, f), -1);

	free(line);
}
END_TEST


void abort_action(int sig) {
	exit(42);
}
struct sigaction *sig_oldact;
void abort_setup() {
	struct sigaction action = {
		.sa_handler = abort_action,
		.sa_flags = 0,
	};
	ck_assert_int_eq(sigemptyset(&action.sa_mask), 0);
	ck_assert_int_eq(sigaction(SIGABRT, &action, sig_oldact), 0);
	basic_setup();
}

TEST_CASE(cause_exit, abort_setup) {}

TEST_EXIT(cause_exit, cause_exit_critical, 42) {
	critical("Cause abort");
}

TEST_EXIT(cause_exit, cause_exit_fatal, 24) {
	fatal(24, "Cause exit");
}
