// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020-2022, CZ.NIC z.s.p.o. (http://www.nic.cz/)
#include <signal.h>

#define SUITE "formats"
#include "unittests.h"


static void setup_color() {
	basic_setup();
	log_add_output(tlog, stderr, LOG_F_COLORS, LL_INFO,
			LOG_FP_COLOR "%m" LOG_FP_COLOR_CLEAR);
}

TEST_CASE(color, setup_color) {}

static const struct {
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

ARRAY_TEST(color, check_color_format, color_format_tests) {
	log_set_level(tlog, LL_TRACE);
	logc(tlog, _d.level, "foo");

	fflush(stderr);
	ck_assert_str_eq(stderr_data, _d.out);
}
END_TEST

static void setup_level_name() {
	basic_setup();
	log_add_output(tlog, stderr, 0, LL_INFO, LOG_FP_LEVEL_NAME);
}

TEST_CASE(level, setup_level_name) {}

static const struct {
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

ARRAY_TEST(level, check_level_name_format, level_name_format_tests) {
	log_set_level(tlog, LL_TRACE);
	logc(tlog, _d.level, "foo");

	fflush(stderr);
	ck_assert_str_eq(stderr_data, _d.out);
}
END_TEST


TEST_CASE(origin) {}

TEST(origin, check_origin_disabled_format) {
	log_add_output(tlog, stderr, 0, LL_INFO, LOG_FP_ORIGIN " %m");

	log_notice(tlog, "foo");

	fflush(stderr);
	ck_assert_str_eq(stderr_data, "tlog: foo\n");
}
END_TEST

TEST(origin, check_origin_format) {
	log_add_output(tlog, stderr, 0, LL_INFO, LOG_FP_ORIGIN " %m");
	log_set_use_origin(tlog, true);

	unsigned line = __LINE__ + 1;
	log_notice(tlog, "foo");

	fflush(stderr);
	char *expected;
	size_t len = asprintf(&expected, "tlog(%s:%d,%s): foo\n", __FILE__, line, __func__);
	ck_assert_int_eq(stderr_len, len);
	ck_assert_str_eq(stderr_data, expected);
	free(expected);
}
END_TEST


TEST_CASE(predefined) {}

TEST(predefined, plain_format) {
	log_add_output(tlog, stderr, 0, 0, LOG_FORMAT_PLAIN);

	log_notice(tlog, "foo");

	const char *expected = "tlog: foo\n";
	ck_assert_str_eq(stderr_data, expected);
	ck_assert_int_eq(stderr_len, strlen(expected));
}
END_TEST

TEST(predefined, full_format) {
	log_add_output(tlog, stderr, 0, 0, LOG_FORMAT_FULL);

	log_notice(tlog, "foo");

	const char *expected = "NOTICE:tlog: foo\n";
	ck_assert_str_eq(stderr_data, expected);
	ck_assert_int_eq(stderr_len, strlen(expected));
}
END_TEST
