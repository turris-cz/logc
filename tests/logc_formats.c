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
#include "../logc.h"
#include <signal.h>
#include <errno.h>

static FILE *orig_stderr;
static char *stderr_data;
static size_t stderr_len;
static log_t tlog;

static void setup_tlog() {
	errno = 0; // Set to 0 to reset any previous possible error
	orig_stderr = stderr;
	stderr = open_memstream(&stderr_data, &stderr_len);

	tlog = malloc(sizeof *tlog);
	*tlog = (struct log) {
		.name = "tlog"
	};
}

static void setup_color() {
	setup_tlog();
	log_add_output(tlog, stderr, LOG_F_COLORS, LL_INFO,
			LOG_FP_COLOR "%m" LOG_FP_COLOR_CLEAR);
}

static void teardown_tlog() {
	log_free(tlog);
	free(tlog);

	fclose(stderr);
	stderr = orig_stderr;
}

struct color_format_tests {
	enum log_level level;
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
	log(tlog, color_format_tests[_i].level, "foo");

	fflush(stderr);
	ck_assert_str_eq(stderr_data, color_format_tests[_i].out);
}
END_TEST


void logc_formats_tests(Suite *suite) {
	TCase *color = tcase_create("color format");
	tcase_add_checked_fixture(color, setup_color, teardown_tlog);
	tcase_add_loop_test(color, check_color_format, 0,
			sizeof(color_format_tests) / sizeof(struct color_format_tests));
	suite_add_tcase(suite, color);
}
