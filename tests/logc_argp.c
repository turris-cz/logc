// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020-2022, CZ.NIC z.s.p.o. (http://www.nic.cz/)
#define SUITE "logc_argp"
#include "unittests.h"
#include "fakesyslog.h"
#include <logc_argp.h>


struct argp_levels {
	enum log_message_level level;
	int argc;
	char **argv;
};

static void argp_setup_tlog() {
	basic_setup();
	logc_argp_log = tlog;
}

TEST_CASE(argp, argp_setup_tlog) {}

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

ARRAY_TEST(argp, argp_q_v, argp_q_v_values) {
	ck_assert_int_eq(0,
			argp_parse(&argp_tlog_parser, _d.argc, _d.argv, 0, NULL, NULL));
	ck_assert_int_eq(_d.level, log_level(logc_argp_log));
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

ARRAY_TEST(argp, argp_log_level, argp_log_level_values) {
	ck_assert_int_eq(0,
			argp_parse(&argp_tlog_parser, _d.argc, _d.argv, 0, NULL, NULL));
	ck_assert_int_eq(_d.level, log_level(logc_argp_log));
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

ARRAY_TEST_EXIT(argp, argp_log_level_invalid, argp_err_exit_status, argp_log_level_invalid_values) {
	argp_parse(&argp_tlog_parser, _d.argc, _d.argv, 0, NULL, NULL);
}
END_TEST

TEST(argp, argp_log_file) {
	char tmpfile_path[] = "/tmp/logc_argp_log_file_XXXXXX";
	ck_assert(mktemp(tmpfile_path));

	ck_assert_int_eq(0, argp_parse(&argp_tlog_parser, 3,
		(char*[]){"t", "--log-file", tmpfile_path}, 0, NULL, NULL));

	log_warning(logc_argp_log, "foo");
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

TEST(argp, argp_syslog) {
	fakesyslog_reset();

	ck_assert_int_eq(0, argp_parse(&argp_tlog_parser, 2,
		(char*[]){"t", "--syslog"}, 0, NULL, NULL));

	log_warning(logc_argp_log, "foo");

	ck_assert_int_eq(1, fakesyslog_cnt);
	ck_assert_str_eq("WARNING:tlog: foo\n", fakesyslog[0].msg);
}
END_TEST
