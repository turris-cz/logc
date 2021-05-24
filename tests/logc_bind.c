/* Copyright (c) 2021 CZ.NIC z.s.p.o. (http://www.nic.cz/)
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
#include <errno.h>
#include "fakesyslog.h"
#include "logc_fixtures.h"

void unittests_add_suite(Suite*);

LOG(sub);
LOG(subsub);


static void bind_setup() {
	setup();
	log_bind(tlog, log_sub);
	log_bind(log_sub, log_subsub);
}

static void bind_teardown() {
	teardown();
	log_free(log_sub);
	log_free(log_subsub);
	ck_assert_int_eq(errno, 0);
}

START_TEST(bound) {
	ck_assert_ptr_null(log_bound(tlog));
	ck_assert_ptr_eq(log_bound(log_sub), tlog);
	ck_assert_ptr_eq(log_bound(log_subsub), log_sub);
}
END_TEST

START_TEST(unbind) {
	ck_assert_ptr_eq(log_bound(log_sub), tlog);
	log_unbind(log_sub);
	ck_assert_ptr_null(log_bound(log_sub));
}
END_TEST

START_TEST(sub_simple_warning) {
	log_warning(log_sub, "This is warning!");
	const char *res = "WARNING:sub: This is warning!\n";
	ck_assert_int_eq(stderr_len, strlen(res));
	ck_assert_str_eq(stderr_data, res);
}
END_TEST

START_TEST(subsub_simple_warning) {
	log_warning(log_subsub, "This is warning!");
	const char *res = "WARNING:subsub: This is warning!\n";
	ck_assert_int_eq(stderr_len, strlen(res));
	ck_assert_str_eq(stderr_data, res);
}
END_TEST

START_TEST(sub_level) {
	log_set_level(log_sub, LL_WARNING);
	log_warning(log_sub, "This is warning!");
	log_notice(log_sub, "This is notice.");
	const char *res = "WARNING:sub: This is warning!\n";
	ck_assert_str_eq(stderr_data, res);
	ck_assert_int_eq(stderr_len, strlen(res));
}
END_TEST

START_TEST(top_level) {
	log_set_level(tlog, LL_WARNING);
	log_warning(log_sub, "This is warning!");
	log_notice(log_sub, "This is notice.");
	const char *res = "WARNING:sub: This is warning!\n";
	ck_assert_str_eq(stderr_data, res);
	ck_assert_int_eq(stderr_len, strlen(res));
}
END_TEST

START_TEST(both_level) {
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
START_TEST(sub_output_ignored) {
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
	tlog->syslog = true;
	log_stderr_fallback(tlog, false);
}

static void syslog_teardown() {
	fakesyslog_free();
	tlog->syslog = false;
	bind_teardown();
	ck_assert_int_eq(stderr_len, 0);
}

START_TEST(sub_syslog_warning) {
	log_warning(log_sub, "This is warning!");
	ck_assert_int_eq(1, fakesyslog_cnt);
	ck_assert_str_eq("WARNING:sub: This is warning!\n", fakesyslog[0].msg);
}
END_TEST

START_TEST(subsub_syslog_warning) {
	log_warning(log_subsub, "This is warning!");
	ck_assert_int_eq(1, fakesyslog_cnt);
	ck_assert_str_eq("WARNING:subsub: This is warning!\n", fakesyslog[0].msg);
}
END_TEST

START_TEST(sub_syslog_ignored) {
	log_sub->syslog = true;
	log_subsub->syslog = true;
	log_warning(log_subsub, "This is warning!");
	ck_assert_int_eq(1, fakesyslog_cnt);
	ck_assert_str_eq("WARNING:subsub: This is warning!\n", fakesyslog[0].msg);
}
END_TEST

START_TEST(sub_syslog_level) {
	log_set_level(log_sub, LL_WARNING);
	log_warning(log_sub, "This is warning!");
	log_notice(log_sub, "This is notice.");
	const char *res = "WARNING:sub: This is warning!\n";
	ck_assert_int_eq(1, fakesyslog_cnt);
	ck_assert_str_eq("WARNING:sub: This is warning!\n", fakesyslog[0].msg);
}
END_TEST


__attribute__((constructor))
static void suite() {
	Suite *suite = suite_create("bind");

	TCase *tc_generic = tcase_create("generic");
	tcase_add_checked_fixture(tc_generic, bind_setup, bind_teardown);
	tcase_add_test(tc_generic, bound);
	tcase_add_test(tc_generic, unbind);
	suite_add_tcase(suite, tc_generic);

	TCase *tc_stderr = tcase_create("stderr");
	tcase_add_checked_fixture(tc_stderr, bind_setup, bind_teardown);
	tcase_add_test(tc_stderr, sub_simple_warning);
	tcase_add_test(tc_stderr, subsub_simple_warning);
	tcase_add_test(tc_stderr, sub_level);
	tcase_add_test(tc_stderr, top_level);
	tcase_add_test(tc_stderr, both_level);
	tcase_add_test(tc_stderr, sub_output_ignored);
	suite_add_tcase(suite, tc_stderr);

	TCase *tc_syslog = tcase_create("syslog");
	tcase_add_checked_fixture(tc_syslog, syslog_setup, syslog_teardown);
	tcase_add_test(tc_syslog, sub_syslog_warning);
	tcase_add_test(tc_syslog, subsub_syslog_warning);
	tcase_add_test(tc_syslog, sub_syslog_ignored);
	tcase_add_test(tc_syslog, sub_syslog_level);
	suite_add_tcase(suite, tc_syslog);

	unittests_add_suite(suite);
}
