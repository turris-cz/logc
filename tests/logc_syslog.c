// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020-2022, CZ.NIC z.s.p.o. (http://www.nic.cz/)
#define SUITE "syslog"
#define DEFAULT_SETUP syslog_setup
#define DEFAULT_TEARDOWN syslog_teardown
#include "unittests.h"
#include "fakesyslog.h"


static void syslog_setup() {
	fakesyslog_reset();
	basic_setup();
	tlog->daemon = true;
	log_stderr_fallback(tlog, false);
}

static void syslog_teardown() {
	fakesyslog_free();
	basic_teardown();
	ck_assert_int_eq(0, stderr_len);
}


TEST_CASE(syslog) {}

TEST(syslog, simple_warning) {
	warning("This is warning!");
	ck_assert_int_eq(1, fakesyslog_cnt);
	ck_assert_str_eq("WARNING:tlog: This is warning!\n", fakesyslog[0].msg);
}
END_TEST


TEST_CASE(format) {}

TEST(format, syslog_format) {
	log_syslog_format(tlog, LOG_FORMAT_PLAIN);
	warning("This is warning!");
	ck_assert_int_eq(1, fakesyslog_cnt);
	ck_assert_str_eq("tlog: This is warning!\n", fakesyslog[0].msg);
}
END_TEST
