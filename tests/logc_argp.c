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
#include <check.h>
#include <logc_argp.h>
#include "fakesyslog.h"
#include "logc_fixtures.h"

struct argp_levels {
	enum log_message_level level;
	int argc;
	char **argv;
};

static void argp_setup_tlog() {
	setup();
	logc_argp_log = tlog;
}

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
	return ARGP_ERR_UNKNOWN;
}

const struct argp argp_tlog_parser = {
	.options = (struct argp_option[]){{NULL}},
	.parser = parse_opt,
	.doc = "Some help text",
	.children = (struct argp_child[]){{&logc_argp_parser, 0, "Logging", 2}, {NULL}},
};

const struct argp argp_tlog_daemon_parser = {
	.options = (struct argp_option[]){{NULL}},
	.parser = parse_opt,
	.doc = "Some help text for daemon",
	.children = (struct argp_child[]){{&logc_argp_daemon_parser, 0, "Logging", 2}, {NULL}},
};

static const struct argp_levels argp_q_v_values[] = {
	{LL_NOTICE, 0, (char*[]){NULL}},
	{LL_NOTICE, 1, (char*[]){"t"}},
	{LL_NOTICE, 2, (char*[]){"t", "-vq"}},
	{LL_NOTICE, 3, (char*[]){"t", "--quiet", "--verbose"}},
	{LL_ERROR, 2, (char*[]){"t", "-qq"}},
	{LL_CRITICAL, 2, (char*[]){"t", "-qqq"}},
	{LL_CRITICAL + 1, 2, (char*[]){"t", "-qqqq"}},
	{LL_DEBUG, 2, (char*[]){"t", "-vv"}},
	{LL_TRACE, 2, (char*[]){"t", "-vvv"}},
	{LL_TRACE - 1, 2, (char*[]){"t", "-vvvv"}},
};

START_TEST(argp_q_v) {
	ck_assert_int_eq(0, argp_parse(&argp_tlog_parser,
			argp_q_v_values[_i].argc, argp_q_v_values[_i].argv, 0, NULL, NULL));
	ck_assert_int_eq(argp_q_v_values[_i].level, log_level(logc_argp_log));
}
END_TEST

static const struct argp_levels argp_log_level_values[] = {
	{-87578, 3, (char*[]){"t", "--log-level", "-87578"}},
	{-4, 3, (char*[]){"t", "--log-level", "-4"}},
	{LL_TRACE, 3, (char*[]){"t", "--log-level", "-3"}},
	{LL_DEBUG, 3, (char*[]){"t", "--log-level", "-2"}},
	{LL_INFO, 3, (char*[]){"t", "--log-level", "-1"}},
	{LL_NOTICE, 3, (char*[]){"t", "--log-level", "0"}},
	{LL_WARNING, 3, (char*[]){"t", "--log-level", "1"}},
	{LL_ERROR, 3, (char*[]){"t", "--log-level", "2"}},
	{LL_CRITICAL, 3, (char*[]){"t", "--log-level", "3"}},
	{4, 3, (char*[]){"t", "--log-level", "4"}},
	{78634, 3, (char*[]){"t", "--log-level", "78634"}},
	{LL_NOTICE, 0, (char*[]){NULL}},
};

START_TEST(argp_log_level) {
	ck_assert_int_eq(0, argp_parse(&argp_tlog_parser, argp_log_level_values[_i].argc,
		argp_log_level_values[_i].argv, 0, NULL, NULL));
	ck_assert_int_eq(argp_log_level_values[_i].level, log_level(logc_argp_log));
}
END_TEST

static const struct argp_levels argp_log_level_invalid_values[] = {
	{0, 3, (char*[]){"t", "--log-level", ""}},
	{42, 3, (char*[]){"t", "--log-level", "42number"}},
	{42, 3, (char*[]){"t", "--log-level", "number42"}},
	{0, 3, (char*[]){"t", "--log-level", "not_a_number"}},
	{0, 3, (char*[]){"t", "--log-level", "786341321313234343478678634344"}},
	{0, 3, (char*[]){"t", "--log-level", "-875788987897979797979678687687"}},
};

START_TEST(argp_log_level_invalid) {
	argp_parse(&argp_tlog_parser, argp_log_level_invalid_values[_i].argc,
		argp_log_level_invalid_values[_i].argv, 0, NULL, NULL);
}
END_TEST

START_TEST(argp_log_file) {
	char tmpfile_path[] = "/tmp/logc_argp_log_file_XXXXXX";
	ck_assert(mktemp(tmpfile_path));

	ck_assert_int_eq(0, argp_parse(&argp_tlog_parser, 3,
		(char*[]){"t", "--log-file", tmpfile_path}, 0, NULL, NULL));

	warning(logc_argp_log, "foo");
	log_flush(logc_argp_log);

	FILE *f = fopen(tmpfile_path, "r");
	ck_assert(f);
	char *line = NULL;
	size_t line_size = 0;
	ck_assert_int_ge(getline(&line, &line_size, f), 0);
	ck_assert_str_eq(line, "WARNING:tlog: foo\n");
	free(line);

	unlink(tmpfile_path);
	fflush(stderr);
}
END_TEST

START_TEST(argp_syslog) {
	fakesyslog_reset();

	ck_assert_int_eq(0, argp_parse(&argp_tlog_parser, 2,
		(char*[]){"t", "--syslog"}, 0, NULL, NULL));

	warning(logc_argp_log, "foo");

	ck_assert_int_eq(1, fakesyslog_cnt);
	ck_assert_str_eq("WARNING:tlog: foo\n", fakesyslog[0].msg);
}
END_TEST

void logc_argp_tests(Suite *suite) {
	TCase *argp = tcase_create("argp");
	tcase_add_checked_fixture(argp, argp_setup_tlog, teardown);
	tcase_add_loop_test(argp, argp_q_v,
		0, sizeof(argp_q_v_values) / sizeof(*argp_q_v_values));
	tcase_add_loop_test(argp, argp_log_level,
		0, sizeof(argp_log_level_values) / sizeof(*argp_log_level_values));
	tcase_add_loop_exit_test(argp, argp_log_level_invalid, argp_err_exit_status,
		0, sizeof(argp_log_level_invalid_values) / sizeof(*argp_log_level_invalid_values));
	tcase_add_test(argp, argp_log_file);
	tcase_add_test(argp, argp_syslog);
	suite_add_tcase(suite, argp);
}
