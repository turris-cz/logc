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
#define DEFLOG tlog
#include <check.h>
#include <signal.h>
#include <logc_asserts.h>
#include "logc_fixtures.h"

void unittests_add_suite(Suite*);


void (*postabort_check)();

void abort_handler(int signum){
	ck_assert_int_eq(signum, SIGABRT); // This is only for abort
	ck_assert(postabort_check);
	postabort_check();
	teardown();
	exit(42);
}

void setup_mask_abort() {
	setup();
	postabort_check = NULL;
	struct sigaction sigact;

	sigact.sa_handler = abort_handler;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_flags = 0;
	ck_assert_int_eq(sigaction(SIGABRT, &sigact, NULL), 0);
}

void teardown_fail() {
	ck_abort();
}

void teardown_success() {
	ck_assert_int_eq(stderr_len, 0);
	teardown();
}

void basic_assert_postabort() {
	ck_assert_str_eq(stderr_data, "CRITICAL:tlog: Assertion 'a == b' failed\n");
}
START_TEST(basic_assert) {
	bool a = true, b = false;
	postabort_check = basic_assert_postabort;
	assert(a == b);
}
END_TEST

START_TEST(basic_assert_valid) {
	assert(true == true);
}
END_TEST

void int_assert_postabort() {
	ck_assert_str_eq(stderr_data, "CRITICAL:tlog: Assertion 'a == b' failed: 1 == 2\n");
}
START_TEST(int_assert) {
	int a = 1, b = 2;
	postabort_check = int_assert_postabort;
	assert_eq(a, b);
}
END_TEST

START_TEST(int_assert_valid) {
	int a = 2, b = 2;
	assert_eq(a, b);
}
END_TEST

START_TEST(unsigned_assert) {
	unsigned a = 1, b = 2;
	postabort_check = int_assert_postabort;
	assert_eq(a, b);
}
END_TEST

START_TEST(unsigned_assert_valid) {
	unsigned a = 2, b = 2;
	assert_eq(a, b);
}
END_TEST

START_TEST(short_assert) {
	short a = 1, b = 2;
	postabort_check = int_assert_postabort;
	assert_eq(a, b);
}
END_TEST

START_TEST(short_assert_valid) {
	short a = 2, b = 2;
	assert_eq(a, b);
}
END_TEST

void float_assert_postabort() {
	ck_assert_str_eq(stderr_data, "CRITICAL:tlog: Assertion 'a == b' failed: 1.000000E+00 == 2.000000E+00\n");
}
START_TEST(float_assert) {
	float a = 1, b = 2;
	postabort_check = float_assert_postabort;
	assert_eq(a, b);
}
END_TEST

START_TEST(float_assert_valid) {
	float a = 2, b = 2;
	assert_eq(a, b);
}
END_TEST

START_TEST(double_assert) {
	double a = 1, b = 2;
	postabort_check = float_assert_postabort;
	assert_eq(a, b);
}
END_TEST

START_TEST(double_assert_valid) {
	double a = 2, b = 2;
	assert_eq(a, b);
}
END_TEST

START_TEST(long_double_assert) {
	long double a = 1, b = 2;
	postabort_check = float_assert_postabort;
	assert_eq(a, b);
}
END_TEST

START_TEST(long_double_assert_valid) {
	long double a = 2, b = 2;
	assert_eq(a, b);
}
END_TEST

void str_assert_postabort() {
	ck_assert_str_eq(stderr_data, "CRITICAL:tlog: Assertion 'a == b' failed: 'foo' == 'fee'\n");
}
START_TEST(str_assert) {
	char *a = "foo", *b = "fee";
	postabort_check = str_assert_postabort;
	assert_eq(a, b);
}
END_TEST

START_TEST(str_assert_valid) {
	char *a = "fee", *b = "fee";
	assert_eq(a, b);
}
END_TEST

void strl_assert_postabort() {
	ck_assert_str_eq(stderr_data, "CRITICAL:tlog: Assertion 'a == b' failed: 'fo' == 'fe'\n");
}
START_TEST(strl_assert) {
	char *a = "foo", *b = "fee";
	postabort_check = strl_assert_postabort;
	assert_eq(a, b, 2);
}
END_TEST

START_TEST(strl_assert_valid) {
	char *a = "feo", *b = "fee";
	assert_eq(a, b, 2);
}
END_TEST

void floatl_assert_postabort() {
	ck_assert_str_eq(stderr_data, "CRITICAL:tlog: Assertion 'a == b' failed: 2.070000E+00 == 1.940000E+00\n");
}
START_TEST(floatl_assert) {
	float a = 2.07, b = 1.94;
	postabort_check = floatl_assert_postabort;
	assert_eq(a, b, 0.08);
}
END_TEST

START_TEST(floatl_assert_valid) {
	float a = 2.07, b = 1.94;
	assert_eq(a, b, 0.2);
}
END_TEST

void int_assert_ne_postabort() {
	ck_assert_str_eq(stderr_data, "CRITICAL:tlog: Assertion 'a != b' failed: 1 != 1\n");
}
START_TEST(int_assert_ne) {
	int a = 1, b = 1;
	postabort_check = int_assert_ne_postabort;
	assert_ne(a, b);
}
END_TEST
START_TEST(int_assert_ne_valid) {
	int a = 2, b = 1;
	assert_ne(a, b);
}
END_TEST

void int_assert_gt_postabort() {
	ck_assert_str_eq(stderr_data, "CRITICAL:tlog: Assertion 'a > b' failed: 1 > 2\n");
}
START_TEST(int_assert_gt) {
	int a = 1, b = 2;
	postabort_check = int_assert_gt_postabort;
	assert_gt(a, b);
}
END_TEST
START_TEST(int_assert_gt_valid) {
	int a = 2, b = 1;
	assert_gt(a, b);
}
END_TEST

void int_assert_ge_postabort() {
	ck_assert_str_eq(stderr_data, "CRITICAL:tlog: Assertion 'a >= b' failed: 1 >= 2\n");
}
START_TEST(int_assert_ge) {
	int a = 1, b = 2;
	postabort_check = int_assert_ge_postabort;
	assert_ge(a, b);
}
END_TEST
START_TEST(int_assert_ge_valid1) {
	int a = 2, b = 1;
	assert_ge(a, b);
}
END_TEST
START_TEST(int_assert_ge_valid2) {
	int a = 1, b = 1;
	assert_ge(a, b);
}
END_TEST

void int_assert_lt_postabort() {
	ck_assert_str_eq(stderr_data, "CRITICAL:tlog: Assertion 'a < b' failed: 2 < 1\n");
}
START_TEST(int_assert_lt) {
	int a = 2, b = 1;
	postabort_check = int_assert_lt_postabort;
	assert_lt(a, b);
}
END_TEST
START_TEST(int_assert_lt_valid) {
	int a = 1, b = 2;
	assert_lt(a, b);
}
END_TEST

void int_assert_le_postabort() {
	ck_assert_str_eq(stderr_data, "CRITICAL:tlog: Assertion 'a <= b' failed: 2 <= 1\n");
}
START_TEST(int_assert_le) {
	int a = 2, b = 1;
	postabort_check = int_assert_le_postabort;
	assert_le(a, b);
}
END_TEST
START_TEST(int_assert_le_valid1) {
	int a = 1, b = 2;
	assert_le(a, b);
}
END_TEST
START_TEST(int_assert_le_valid2) {
	int a = 1, b = 1;
	assert_le(a, b);
}
END_TEST

void floatl_assert_ne_postabort() {
	ck_assert_str_eq(stderr_data, "CRITICAL:tlog: Assertion 'a != b' failed: 2.070000E+00 != 1.940000E+00\n");
}
START_TEST(floatl_assert_ne) {
	float a = 2.07, b = 1.94;
	postabort_check = floatl_assert_ne_postabort;
	assert_ne(a, b, 0.2);
}
END_TEST
START_TEST(floatl_assert_ne_valid) {
	float a = 2.07, b = 1.94;
	assert_ne(a, b, 0.08);
}
END_TEST

void floatl_assert_gt_postabort() {
	ck_assert_str_eq(stderr_data, "CRITICAL:tlog: Assertion 'a > b' failed: 1.940000E+00 > 2.070000E+00\n");
}
START_TEST(floatl_assert_gt) {
	float a = 1.94, b = 2.07;
	postabort_check = floatl_assert_gt_postabort;
	assert_gt(a, b, 0.08);
}
END_TEST
START_TEST(floatl_assert_gt_valid1) {
	float a = 1.94, b = 2.07;
	assert_gt(a, b, 0.2);
}
END_TEST
START_TEST(floatl_assert_gt_valid2) {
	float a = 2.07, b = 1.94;
	assert_gt(a, b, 0.08);
}
END_TEST
START_TEST(floatl_assert_gt_valid3) {
	float a = 2.07, b = 0.94;
	assert_gt(a, b, 0.08);
}
END_TEST

void floatl_assert_lt_postabort() {
	ck_assert_str_eq(stderr_data, "CRITICAL:tlog: Assertion 'a < b' failed: 2.070000E+00 < 1.940000E+00\n");
}
START_TEST(floatl_assert_lt) {
	float a = 2.07, b = 1.94;
	postabort_check = floatl_assert_lt_postabort;
	assert_lt(a, b, 0.08);
}
END_TEST
START_TEST(floatl_assert_lt_valid1) {
	float a = 2.07, b = 1.94;
	assert_lt(a, b, 0.2);
}
END_TEST
START_TEST(floatl_assert_lt_valid2) {
	float a = 1.94, b = 2.07;
	assert_lt(a, b, 0.08);
}
END_TEST
START_TEST(floatl_assert_lt_valid3) {
	float a = 0.94, b = 2.07;
	assert_lt(a, b, 0.08);
}
END_TEST

__attribute__((constructor))
static void suite() {
	Suite *suite = suite_create("logc_asserts");

	TCase *asserts_f = tcase_create("asserts fail");
	tcase_add_checked_fixture(asserts_f, setup_mask_abort, teardown_fail);
	tcase_add_exit_test(asserts_f, basic_assert, 42);
	tcase_add_exit_test(asserts_f, int_assert, 42);
	tcase_add_exit_test(asserts_f, unsigned_assert, 42);
	tcase_add_exit_test(asserts_f, short_assert, 42);
	tcase_add_exit_test(asserts_f, float_assert, 42);
	tcase_add_exit_test(asserts_f, double_assert, 42);
	tcase_add_exit_test(asserts_f, long_double_assert, 42);
	tcase_add_exit_test(asserts_f, str_assert, 42);
	tcase_add_exit_test(asserts_f, strl_assert, 42);
	tcase_add_exit_test(asserts_f, floatl_assert, 42);
	tcase_add_exit_test(asserts_f, int_assert_ne, 42);
	tcase_add_exit_test(asserts_f, int_assert_gt, 42);
	tcase_add_exit_test(asserts_f, int_assert_ge, 42);
	tcase_add_exit_test(asserts_f, int_assert_lt, 42);
	tcase_add_exit_test(asserts_f, int_assert_le, 42);
	tcase_add_exit_test(asserts_f, floatl_assert_ne, 42);
	tcase_add_exit_test(asserts_f, floatl_assert_gt, 42);
	tcase_add_exit_test(asserts_f, floatl_assert_lt, 42);
	suite_add_tcase(suite, asserts_f);

	TCase *asserts_s = tcase_create("asserts success");
	tcase_add_checked_fixture(asserts_s, setup_mask_abort, teardown_success);
	tcase_add_exit_test(asserts_s, basic_assert_valid, 0);
	tcase_add_exit_test(asserts_s, unsigned_assert_valid, 0);
	tcase_add_exit_test(asserts_s, short_assert_valid, 0);
	tcase_add_exit_test(asserts_s, float_assert_valid, 0);
	tcase_add_exit_test(asserts_s, double_assert_valid, 0);
	tcase_add_exit_test(asserts_s, long_double_assert_valid, 0);
	tcase_add_exit_test(asserts_s, str_assert_valid, 0);
	tcase_add_exit_test(asserts_s, strl_assert_valid, 0);
	tcase_add_exit_test(asserts_s, floatl_assert_valid, 0);
	tcase_add_exit_test(asserts_s, int_assert_ne_valid, 0);
	tcase_add_exit_test(asserts_s, int_assert_gt_valid, 0);
	tcase_add_exit_test(asserts_s, int_assert_ge_valid1, 0);
	tcase_add_exit_test(asserts_s, int_assert_ge_valid2, 0);
	tcase_add_exit_test(asserts_s, int_assert_lt_valid, 0);
	tcase_add_exit_test(asserts_s, int_assert_le_valid1, 0);
	tcase_add_exit_test(asserts_s, int_assert_le_valid2, 0);
	tcase_add_exit_test(asserts_s, floatl_assert_ne_valid, 0);
	tcase_add_exit_test(asserts_s, floatl_assert_gt_valid1, 0);
	tcase_add_exit_test(asserts_s, floatl_assert_gt_valid2, 0);
	tcase_add_exit_test(asserts_s, floatl_assert_gt_valid3, 0);
	tcase_add_exit_test(asserts_s, floatl_assert_lt_valid1, 0);
	tcase_add_exit_test(asserts_s, floatl_assert_lt_valid2, 0);
	tcase_add_exit_test(asserts_s, floatl_assert_lt_valid3, 0);
	suite_add_tcase(suite, asserts_s);

	unittests_add_suite(suite);
}
