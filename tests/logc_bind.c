// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020-2022, CZ.NIC z.s.p.o. (http://www.nic.cz/)
#include <errno.h>

#define SUITE "bind"
#define DEFAULT_SETUP bind_setup
#define DEFAULT_TEARDOWN bind_teardown
#include "unittests.h"
#include "fakesyslog.h"

LOG(sub);
LOG(subsub);


static void bind_setup() {
	basic_setup();
	log_bind(tlog, log_sub);
	log_bind(log_sub, log_subsub);
}

static void bind_teardown() {
	basic_teardown();
	log_free(log_sub);
	log_free(log_subsub);
	ck_assert_int_eq(errno, 0);
}


TEST_CASE(generic) {}

TEST(generic, bound) {
	ck_assert_ptr_null(log_bound(tlog));
	ck_assert_ptr_eq(log_bound(log_sub), tlog);
	ck_assert_ptr_eq(log_bound(log_subsub), log_sub);
}
END_TEST

TEST(generic, unbind) {
	ck_assert_ptr_eq(log_bound(log_sub), tlog);
	log_unbind(log_sub);
	ck_assert_ptr_null(log_bound(log_sub));
}
END_TEST


TEST_CASE(stderr) {}

TEST(stderr, sub_simple_warning) {
	log_warning(log_sub, "This is warning!");
	const char *res = "WARNING:sub: This is warning!\n";
	ck_assert_int_eq(stderr_len, strlen(res));
	ck_assert_str_eq(stderr_data, res);
}
END_TEST

TEST(stderr, subsub_simple_warning) {
	log_warning(log_subsub, "This is warning!");
	const char *res = "WARNING:subsub: This is warning!\n";
	ck_assert_int_eq(stderr_len, strlen(res));
	ck_assert_str_eq(stderr_data, res);
}
END_TEST

TEST(stderr, sub_level) {
	log_set_level(log_sub, LL_WARNING);
	log_warning(log_sub, "This is warning!");
	log_notice(log_sub, "This is notice.");
	const char *res = "WARNING:sub: This is warning!\n";
	ck_assert_str_eq(stderr_data, res);
	ck_assert_int_eq(stderr_len, strlen(res));
}
END_TEST

TEST(stderr, top_level) {
	log_set_level(tlog, LL_WARNING);
	log_warning(log_sub, "This is warning!");
	log_notice(log_sub, "This is notice.");
	const char *res = "WARNING:sub: This is warning!\n";
	ck_assert_str_eq(stderr_data, res);
	ck_assert_int_eq(stderr_len, strlen(res));
}
END_TEST

TEST(stderr, both_level) {
	log_set_level(tlog, LL_INFO);
	log_set_level(log_sub, LL_WARNING);
	log_notice(log_sub, "This is notice.");
	log_info(log_sub, "This is info.");
	const char *res = "NOTICE:sub: This is notice.\n";
	ck_assert_str_eq(stderr_data, res);
	ck_assert_int_eq(stderr_len, strlen(res));
}
END_TEST

// There might be outputs in sub or subsub but those are ignored.
TEST(stderr, sub_output_ignored) {
	log_add_output(log_sub, stderr, 0, 0, LOG_FORMAT_PLAIN);
	log_add_output(log_subsub, stderr, 0, 0, LOG_FORMAT_PLAIN);
	log_warning(log_subsub, "This is warning!");
	const char *res = "WARNING:subsub: This is warning!\n";
	ck_assert_int_eq(stderr_len, strlen(res));
	ck_assert_str_eq(stderr_data, res);
}
END_TEST


static void syslog_setup() {
	bind_setup();
	fakesyslog_reset();
	tlog->daemon = true;
	log_stderr_fallback(tlog, false);
}

static void syslog_teardown() {
	fakesyslog_free();
	tlog->daemon = false;
	bind_teardown();
	ck_assert_int_eq(stderr_len, 0);
}

TEST_CASE(syslog, syslog_setup, syslog_teardown) {}

TEST(syslog, sub_syslog_warning) {
	log_warning(log_sub, "This is warning!");
	ck_assert_int_eq(1, fakesyslog_cnt);
	ck_assert_str_eq("WARNING:sub: This is warning!\n", fakesyslog[0].msg);
}
END_TEST

TEST(syslog, subsub_syslog_warning) {
	log_warning(log_subsub, "This is warning!");
	ck_assert_int_eq(1, fakesyslog_cnt);
	ck_assert_str_eq("WARNING:subsub: This is warning!\n", fakesyslog[0].msg);
}
END_TEST

TEST(syslog, sub_syslog_ignored) {
	log_sub->daemon = true;
	log_subsub->daemon = true;
	log_warning(log_subsub, "This is warning!");
	ck_assert_int_eq(1, fakesyslog_cnt);
	ck_assert_str_eq("WARNING:subsub: This is warning!\n", fakesyslog[0].msg);
}
END_TEST

TEST(syslog, sub_syslog_level) {
	log_set_level(log_sub, LL_WARNING);
	log_warning(log_sub, "This is warning!");
	log_notice(log_sub, "This is notice.");
	ck_assert_int_eq(1, fakesyslog_cnt);
	ck_assert_str_eq("WARNING:sub: This is warning!\n", fakesyslog[0].msg);
}
END_TEST
