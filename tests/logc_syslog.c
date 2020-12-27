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
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <syslog.h>
#include "fakesyslog.h"

static FILE *orig_stderr;
static char *stderr_data;
static size_t stderr_len;
static log_t tlog;

static void setup_tlog() {
	fakesyslog_reset();

	errno = 0; // Set to 0 to reset any previous possible error
	orig_stderr = stderr;
	stderr = open_memstream(&stderr_data, &stderr_len);

	tlog = malloc(sizeof *tlog);
	*tlog = (struct log) {
		.name = "tlog",
		.syslog = true,
	};
	log_stderr_fallback(tlog, false);
}

static void teardown_tlog() {
	fakesyslog_free();

	fclose(stderr);
	stderr = orig_stderr;
	ck_assert_int_eq(0, stderr_len);

	log_free(tlog);
	free(tlog);
}


START_TEST(simple_warning) {
	WARNING("This is warning!");
	ck_assert_int_eq(1, fakesyslog_cnt);
	ck_assert_str_eq("WARNING:tlog: This is warning!\n", fakesyslog[0].msg);
}
END_TEST


void logc_syslog_tests(Suite *suite) {
	TCase *tc_syslog = tcase_create("syslog");
	tcase_add_test(tc_syslog, simple_warning);
	tcase_add_checked_fixture(tc_syslog, setup_tlog, teardown_tlog);
	suite_add_tcase(suite, tc_syslog);
}
