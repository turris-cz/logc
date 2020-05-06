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
#include "logc.h"
#include "../logc.h"
#include <signal.h>
#include <errno.h>

log_t tlog;

static void setup_tlog() {
	tlog = malloc(sizeof *tlog);
	*tlog = (struct log) {
		.name = "tlog"
	};
	errno = 0;
}

static void teardown_tlog() {
	log_free(tlog);
	free(tlog);
}

char *stderr_data;
size_t stderr_len;

static void stderr_setup() {
	stderr = open_memstream(&stderr_data, &stderr_len);
	setup_tlog();
}

static void stderr_teardown() {
	fclose(stderr);
	free(stderr_data);
	teardown_tlog();
}

START_TEST(simple_warning) {
	WARNING("This is warning!");

	fflush(stderr);
	ck_assert_str_eq("tlog: This is warning!\n", stderr_data);
}
END_TEST

START_TEST(check_default_level) {
	log(tlog, _i, "This is message!");

	fflush(stderr);
	if (_i >= LL_NOTICE)
		ck_assert_str_eq("tlog: This is message!\n", stderr_data);
	else
		ck_assert_str_eq("", stderr_data);
}
END_TEST

START_TEST(check_all_levels) {
	log_set_level(tlog, LL_TRACE);
	const int line = __LINE__ + 1;
	log(tlog, _i, "This is message!");

	fflush(stderr);
	char *expected;
	int len = asprintf(&expected, "tlog(%s:%d,%s): This is message!\n",
			__FILE__, line, __func__);
	ck_assert_str_eq(expected, stderr_data);
	ck_assert_int_eq(len, stderr_len);
	free(expected);
}
END_TEST

START_TEST(app_log) {
	APP_LOG(tlog);
	error(log_tlog, "This is error!");

	fflush(stderr);
	ck_assert_str_eq("This is error!\n", stderr_data);
}
END_TEST

START_TEST(standard_error) {
	errno = ENOENT;
	ERROR("This is error");
	ck_assert_int_eq(0, errno); // call should reset errno

	fflush(stderr);
	char *expected;
	int len = asprintf(&expected, "tlog: This is error: %s\n", strerror(ENOENT));
	ck_assert_str_eq(expected, stderr_data);
	ck_assert_int_eq(len, stderr_len);
	free(expected);
}
END_TEST

START_TEST(call_verbose) {
	log_verbose(tlog);
	log_verbose(tlog); // increase verbosity twice

	ck_assert_int_eq(LL_DEBUG, log_level(tlog));

	DEBUG("This is debug!");
	TRACE("This is trace!"); // should not print

	fflush(stderr);
	ck_assert_str_eq("tlog: This is debug!\n", stderr_data);
}
END_TEST

START_TEST(call_quiet) {
	log_quiet(tlog);
	log_quiet(tlog); // decrease verbosity twice

	ck_assert_int_eq(LL_ERROR, log_level(tlog));

	ERROR("This is error!");
	WARNING("This is warning!"); // should not print

	fflush(stderr);
	ck_assert_str_eq("tlog: This is error!\n", stderr_data);
}
END_TEST


Suite *logc_test_suite(void) {
	Suite *suite = suite_create("logc");

	TCase *def_output = tcase_create("default output");
	tcase_add_checked_fixture(def_output, stderr_setup, stderr_teardown);
	tcase_add_test(def_output, simple_warning);
	tcase_add_loop_test(def_output, check_default_level, LL_TRACE, LL_CRITICAL);
	tcase_add_loop_test(def_output, check_all_levels, LL_TRACE, LL_CRITICAL);
	tcase_add_test(def_output, app_log);
	tcase_add_test(def_output, standard_error);
	tcase_add_test(def_output, call_verbose);
	tcase_add_test(def_output, call_quiet);
	suite_add_tcase(suite, def_output);

	return suite;
}
