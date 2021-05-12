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
#include <signal.h>
#include "logc_fixtures.h"

void unittests_add_suite(Suite*);


static void setup_color() {
	setup();
	log_add_output(tlog, stderr, LOG_F_COLORS, LL_INFO,
			LOG_FP_COLOR "%m" LOG_FP_COLOR_CLEAR);
}

struct color_format_tests {
	enum log_message_level level;
	const char *out;
} color_format_tests[] = {
	{LL_TRACE, "\033[37mfoo\033[0m\n"},
	{LL_DEBUG, "\033[37mfoo\033[0m\n"},
	{LL_INFO, "foo\033[0m\n"}, // Note: reset of color is not an issue so we can do it unconditionally
	{LL_NOTICE, "\033[37;1mfoo\033[0m\n"},
	{LL_WARNING, "\033[33;1mfoo\033[0m\n"},
	{LL_ERROR, "\033[31;1mfoo\033[0m\n"},
	{LL_CRITICAL, "\033[31mfoo\033[0m\n"},
};

START_TEST(check_color_format) {
	log_set_level(tlog, LL_TRACE);
	logc(tlog, color_format_tests[_i].level, "foo");

	fflush(stderr);
	ck_assert_str_eq(stderr_data, color_format_tests[_i].out);
}
END_TEST

static void setup_level_name() {
	setup();
	log_add_output(tlog, stderr, 0, LL_INFO, LOG_FP_LEVEL_NAME);
}

struct level_name_format_tests {
	enum log_message_level level;
	const char *out;
} level_name_format_tests[] = {
	{LL_TRACE, "TRACE\n"},
	{LL_DEBUG, "DEBUG\n"},
	{LL_INFO, "INFO\n"},
	{LL_NOTICE, "NOTICE\n"},
	{LL_WARNING, "WARNING\n"},
	{LL_ERROR, "ERROR\n"},
	{LL_CRITICAL, "CRITICAL\n"},
};

START_TEST(check_level_name_format) {
	log_set_level(tlog, LL_TRACE);
	logc(tlog, level_name_format_tests[_i].level, "foo");

	fflush(stderr);
	ck_assert_str_eq(stderr_data, level_name_format_tests[_i].out);
}
END_TEST

START_TEST(check_origin_disabled_format) {
	log_add_output(tlog, stderr, 0, LL_INFO, LOG_FP_ORIGIN " %m");

	logc(tlog, LL_NOTICE, "foo");

	fflush(stderr);
	ck_assert_str_eq(stderr_data, "tlog: foo\n");
}
END_TEST

START_TEST(check_origin_format) {
	log_add_output(tlog, stderr, 0, LL_INFO, LOG_FP_ORIGIN " %m");
	log_set_use_origin(tlog, true);

	unsigned line = __LINE__ + 1;
	logc(tlog, LL_NOTICE, "foo");

	fflush(stderr);
	char *expected;
	size_t len = asprintf(&expected, "tlog(%s:%d,%s): foo\n", __FILE__, line, __func__);
	ck_assert_int_eq(stderr_len, len);
	ck_assert_str_eq(stderr_data, expected);
	free(expected);
}
END_TEST

__attribute__((constructor))
static void suite() {
	Suite *suite = suite_create("logc_formats");

	TCase *color = tcase_create("color format");
	tcase_add_checked_fixture(color, setup_color, teardown);
	tcase_add_loop_test(color, check_color_format, 0,
			sizeof(color_format_tests) / sizeof(struct color_format_tests));
	suite_add_tcase(suite, color);

	TCase *level_name = tcase_create("level name format");
	tcase_add_checked_fixture(level_name, setup_level_name, teardown);
	tcase_add_loop_test(level_name, check_level_name_format, 0,
			sizeof(level_name_format_tests) / sizeof(struct level_name_format_tests));
	suite_add_tcase(suite, level_name);

	TCase *origin = tcase_create("origin format");
	tcase_add_checked_fixture(origin, setup, teardown);
	tcase_add_test(origin, check_origin_disabled_format);
	tcase_add_test(origin, check_origin_format);
	suite_add_tcase(suite, origin);

	unittests_add_suite(suite);
}
