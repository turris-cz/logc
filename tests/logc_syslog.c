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
#include "fakesyslog.h"
#include "logc_fixtures.h"

void unittests_add_suite(Suite*);


static void syslog_setup() {
	fakesyslog_reset();
	setup();
	tlog->syslog = true;
	log_stderr_fallback(tlog, false);
}

static void syslog_teardown() {
	fakesyslog_free();
	teardown();
	ck_assert_int_eq(0, stderr_len);
}

START_TEST(simple_warning) {
	warning("This is warning!");
	ck_assert_int_eq(1, fakesyslog_cnt);
	ck_assert_str_eq("WARNING:tlog: This is warning!\n", fakesyslog[0].msg);
}
END_TEST


__attribute__((constructor))
static void suite() {
	Suite *suite = suite_create("syslog");

	TCase *tc_syslog = tcase_create("syslog");
	tcase_add_checked_fixture(tc_syslog, syslog_setup, syslog_teardown);
	tcase_add_test(tc_syslog, simple_warning);
	suite_add_tcase(suite, tc_syslog);

	unittests_add_suite(suite);
}
