/* Copyright (c) 2020 CZ.NIC z.s.p.o. (http://www.nic.cz/)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#define DEFLOG tlog
#include <check.h>
#include <logc.h>
#include <signal.h>
#include <errno.h>

static FILE *orig_stderr;
static char *stderr_data;
static size_t stderr_len;
static log_t tlog;

static void setup_capture() {
	errno = 0; // Set to 0 to reset any previous possible error
	orig_stderr = stderr;
	stderr = open_memstream(&stderr_data, &stderr_len);
}

static void teardown_capture() {
	fclose(stderr);
	stderr = orig_stderr;
}

static void setup_tlog() {
	setup_capture();

	tlog = malloc(sizeof *tlog);
	*tlog = (struct log) {
		.name = "tlog"
	};
}

static void teardown_tlog() {
	log_free(tlog);
	free(tlog);

	teardown_capture();
}

START_TEST(simple_warning) {
	WARNING("This is warning!");

	fflush(stderr);
	ck_assert_str_eq(stderr_data, "WARNING:tlog: This is warning!\n");

	ck_assert_int_eq(errno, 0);
}
END_TEST

#define ILL(LEVEL) (LL_##LEVEL + 3)
const char *level_name[] = {
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

START_TEST(check_default_level) {
	log(tlog, _i, "This is message!");

	fflush(stderr);
	if (_i >= LL_NOTICE) {
		char *expected;
		int len = asprintf(&expected, "%s:tlog: This is message!\n", level_name[ILL(_i)]);
		ck_assert_str_eq(stderr_data, expected);
		free(expected);
	} else
		ck_assert_str_eq(stderr_data, "");

	ck_assert_int_eq(errno, 0);
}
END_TEST

START_TEST(check_all_levels) {
	log_set_level(tlog, LL_TRACE);
	log(tlog, _i, "This is message!");

	fflush(stderr);
	char *expected;
	int len = asprintf(&expected, "%s:tlog: This is message!\n", level_name[ILL(_i)]);
	ck_assert_str_eq(stderr_data, expected);
	ck_assert_int_eq(stderr_len, len);
	free(expected);

	ck_assert_int_eq(errno, 0);
}
END_TEST

#undef ILL

START_TEST(app_log) {
	APP_LOG(tlog);
	error(log_tlog, "This is error!");

	fflush(stderr);
	ck_assert_str_eq(stderr_data, "ERROR: This is error!\n");

	ck_assert_int_eq(errno, 0);
}
END_TEST

START_TEST(standard_error) {
	errno = ENOENT;
	ERROR("This is error");
	ck_assert_int_eq(errno, 0); // call should reset errno

	fflush(stderr);
	char *expected;
	int len = asprintf(&expected, "ERROR:tlog: This is error: %s\n", strerror(ENOENT));
	ck_assert_str_eq(stderr_data, expected);
	ck_assert_int_eq(stderr_len, len);
	free(expected);

	ck_assert_int_eq(errno, 0);
}
END_TEST

START_TEST(call_verbose) {
	log_verbose(tlog);
	log_verbose(tlog); // increase verbosity twice

	ck_assert_int_eq(log_level(tlog), LL_DEBUG);

	DEBUG("This is debug!");
	TRACE("This is trace!"); // should not print

	fflush(stderr);
	ck_assert_str_eq(stderr_data, "DEBUG:tlog: This is debug!\n");

	ck_assert_int_eq(errno, 0);
}
END_TEST

START_TEST(call_quiet) {
	log_quiet(tlog);
	log_quiet(tlog); // decrease verbosity twice

	ck_assert_int_eq(log_level(tlog), LL_ERROR);

	ERROR("This is error!");
	WARNING("This is warning!"); // should not print

	fflush(stderr);
	ck_assert_str_eq(stderr_data, "ERROR:tlog: This is error!\n");

	ck_assert_int_eq(errno, 0);
}
END_TEST

START_TEST(disabled_def_output) {
	log_stderr_fallback(tlog, false);
	log(tlog, _i, "This is message!");

	fflush(stderr);
	ck_assert_str_eq(stderr_data, "");

	ck_assert_int_eq(errno, 0);
}
END_TEST

START_TEST(message_origin) {
	ck_assert(!log_use_origin(tlog));
	log_set_use_origin(tlog, true);
	ck_assert(log_use_origin(tlog));

	const int line = __LINE__ + 1;
	NOTICE("foo");

	fflush(stderr);
	char *expected;
	int len = asprintf(&expected, "NOTICE:tlog(%s:%d,%s): foo\n", __FILE__, line, __func__);
	ck_assert_str_eq(stderr_data, expected);
	ck_assert_int_eq(stderr_len, len);
	free(expected);

	ck_assert_int_eq(errno, 0);
}
END_TEST

START_TEST(check_would_log) {
	log_set_level(tlog, _i);

	for (enum log_message_level level = LL_TRACE; level < LL_CRITICAL; level++)
		ck_assert(log_would_log(tlog, level) == (level >= _i));

	ck_assert_int_eq(errno, 0);
}
END_TEST

struct custom_output_tests {
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

START_TEST(check_custom_outputs) {
	char *buf;
	size_t bufsiz;
	FILE *f = open_memstream(&buf, &bufsiz);
	log_add_output(tlog, f, 0, 0, custom_output_tests[_i].format);

	_log(tlog, LL_NOTICE, "tests/logc.c", 42, "function_name", "Message");

	fclose(f);
	// Last character is always new line so we skip it
	ck_assert_mem_eq(buf, custom_output_tests[_i].out, bufsiz - 1);
	free(buf);

	ck_assert_int_eq(stderr_len, 0); // No output to stderr
}
END_TEST

struct custom_output_flag_tests {
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

START_TEST(check_custom_outputs_flags) {
	char *buf;
	size_t bufsiz;
	FILE *f = open_memstream(&buf, &bufsiz);
	log_add_output(tlog, f, custom_output_flag_tests[_i].flags, 0, custom_output_flag_tests[_i].format);

	NOTICE("Message");

	fclose(f);
	// Last character is always new line so we skip it
	ck_assert_mem_eq(buf, custom_output_flag_tests[_i].out, bufsiz - 1);
	free(buf);

	ck_assert_int_eq(stderr_len, 0); // No output to stderr
	ck_assert_int_eq(errno, 0);
}
END_TEST

START_TEST(check_custom_output_remove) {
	char *buf;
	size_t bufsiz;
	FILE *f = open_memstream(&buf, &bufsiz);

	log_add_output(tlog, f, LOG_F_AUTOCLOSE, 0, LOG_FORMAT_PLAIN);
	NOTICE("output");
	ck_assert(log_rm_output(tlog, f));
	// No need to flush here as f is closed on log_rm_output and so flushed
	ck_assert_str_eq(buf, "tlog: output\n");
	free(buf);

	// Test default output as well to verify that we stopped using custom one
	NOTICE("stderr");
	fflush(stderr);
	ck_assert_str_eq(stderr_data, "NOTICE:tlog: stderr\n");

	ck_assert_int_eq(errno, 0);
}
END_TEST

START_TEST(check_custom_output_twice) {
	char *buf;
	size_t bufsiz;
	FILE *f = open_memstream(&buf, &bufsiz);

	log_add_output(tlog, f, 0, 0, LOG_FORMAT_PLAIN);
	NOTICE("plain");
	fflush(f);
	ck_assert_str_eq(buf, "tlog: plain\n");

	log_add_output(tlog, f, 0, 0, LOG_FORMAT_DEFAULT);
	NOTICE("default");
	fflush(f);
	ck_assert_str_eq(buf, "tlog: plain\nNOTICE:tlog: default\n");

	fclose(f);
	free(buf);

	ck_assert_int_eq(errno, 0);
}
END_TEST

START_TEST(check_custom_output_wipe) {
	char *buf1, *buf2;
	size_t bufsiz1, bufsiz2;
	FILE *f1 = open_memstream(&buf1, &bufsiz1);
	FILE *f2 = open_memstream(&buf2, &bufsiz2);

	log_add_output(tlog, f1, LOG_F_AUTOCLOSE, 0, LOG_FORMAT_PLAIN);
	log_add_output(tlog, f2, LOG_F_AUTOCLOSE, 0, LOG_FORMAT_PLAIN);

	NOTICE("Message");

	log_wipe_outputs(tlog);
	fflush(stderr);

	ck_assert_str_eq(buf1, "tlog: Message\n");
	ck_assert_str_eq(buf2, "tlog: Message\n");
	ck_assert_int_eq(stderr_len, 0);
	free(buf1);
	free(buf2);

	NOTICE("Message");
	fflush(stderr);
	ck_assert_str_eq(stderr_data, "NOTICE:tlog: Message\n");

	ck_assert_int_eq(errno, 0);
}
END_TEST


void logc_tests(Suite *suite) {
	TCase *def_output = tcase_create("default output");
	tcase_add_checked_fixture(def_output, setup_tlog, teardown_tlog);
	tcase_add_test(def_output, simple_warning);
	tcase_add_loop_test(def_output, check_default_level, LL_TRACE, LL_CRITICAL);
	tcase_add_loop_test(def_output, check_all_levels, LL_TRACE, LL_CRITICAL);
	tcase_add_test(def_output, app_log);
	tcase_add_test(def_output, standard_error);
	tcase_add_test(def_output, call_verbose);
	tcase_add_test(def_output, call_quiet);
	tcase_add_loop_test(def_output, disabled_def_output, LL_TRACE, LL_CRITICAL);
	tcase_add_test(def_output, message_origin);
	suite_add_tcase(suite, def_output);

	TCase *would_log = tcase_create("would log check");
	tcase_add_checked_fixture(would_log, setup_tlog, teardown_tlog);
	tcase_add_loop_test(would_log, check_would_log, LL_TRACE, LL_CRITICAL);
	suite_add_tcase(suite, would_log);

	TCase *custom_output = tcase_create("custom output");
	tcase_add_checked_fixture(custom_output, setup_tlog, teardown_tlog);
	tcase_add_loop_test(custom_output, check_custom_outputs, 0,
			sizeof(custom_output_tests) / sizeof(struct custom_output_tests));
	tcase_add_loop_test(custom_output, check_custom_outputs_flags, 0,
			sizeof(custom_output_flag_tests) / sizeof(struct custom_output_flag_tests));
	tcase_add_test(custom_output, check_custom_output_remove);
	tcase_add_test(custom_output, check_custom_output_twice);
	tcase_add_test(custom_output, check_custom_output_wipe);
	suite_add_tcase(suite, custom_output);
}
