// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020-2022, CZ.NIC z.s.p.o. (http://www.nic.cz/)
#include <signal.h>

#define SUITE "logc_asserts"
#include "unittests.h"
#include <logc_asserts.h>


void (*postabort_check)();

void abort_handler(int signum){
	ck_assert_int_eq(signum, SIGABRT); // This is only for abort
	ck_assert(postabort_check);
	postabort_check();
	basic_teardown();
	exit(42);
}

void setup_mask_abort() {
	basic_setup();
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
	basic_teardown();
}

TEST_CASE(assert_fail, setup_mask_abort, teardown_fail) {}
TEST_CASE(assert_success, setup_mask_abort, teardown_success) {}

#define FAIL_TEST(name) TEST_EXIT(assert_fail, name, 42)
#define SUCCESS_TEST(name) TEST_EXIT(assert_success, name, 0)

void basic_assert_postabort() {
	ck_assert_str_eq(stderr_data, "CRITICAL:tlog: Assertion 'a == b' failed\n");
}
FAIL_TEST(basic_assert) {
	bool a = true, b = false;
	postabort_check = basic_assert_postabort;
	assert(a == b);
}
END_TEST

SUCCESS_TEST(basic_assert_valid) {
	assert(true == true);
}
END_TEST

void int_assert_postabort() {
	ck_assert_str_eq(stderr_data, "CRITICAL:tlog: Assertion 'a == b' failed: 1 == 2\n");
}
FAIL_TEST(int_assert) {
	int a = 1, b = 2;
	postabort_check = int_assert_postabort;
	assert_eq(a, b);
}
END_TEST

SUCCESS_TEST(int_assert_valid) {
	int a = 2, b = 2;
	assert_eq(a, b);
}
END_TEST

FAIL_TEST(unsigned_assert) {
	unsigned a = 1, b = 2;
	postabort_check = int_assert_postabort;
	assert_eq(a, b);
}
END_TEST

SUCCESS_TEST(unsigned_assert_valid) {
	unsigned a = 2, b = 2;
	assert_eq(a, b);
}
END_TEST

FAIL_TEST(short_assert) {
	short a = 1, b = 2;
	postabort_check = int_assert_postabort;
	assert_eq(a, b);
}
END_TEST

SUCCESS_TEST(short_assert_valid) {
	short a = 2, b = 2;
	assert_eq(a, b);
}
END_TEST

void float_assert_postabort() {
	ck_assert_str_eq(stderr_data, "CRITICAL:tlog: Assertion 'a == b' failed: 1.000000E+00 == 2.000000E+00\n");
}
FAIL_TEST(float_assert) {
	float a = 1, b = 2;
	postabort_check = float_assert_postabort;
	assert_eq(a, b);
}
END_TEST

SUCCESS_TEST(float_assert_valid) {
	float a = 2, b = 2;
	assert_eq(a, b);
}
END_TEST

FAIL_TEST(double_assert) {
	double a = 1, b = 2;
	postabort_check = float_assert_postabort;
	assert_eq(a, b);
}
END_TEST

SUCCESS_TEST(double_assert_valid) {
	double a = 2, b = 2;
	assert_eq(a, b);
}
END_TEST

FAIL_TEST(long_double_assert) {
	long double a = 1, b = 2;
	postabort_check = float_assert_postabort;
	assert_eq(a, b);
}
END_TEST

SUCCESS_TEST(long_double_assert_valid) {
	long double a = 2, b = 2;
	assert_eq(a, b);
}
END_TEST

void str_assert_postabort() {
	ck_assert_str_eq(stderr_data, "CRITICAL:tlog: Assertion 'a == b' failed: 'foo' == 'fee'\n");
}
FAIL_TEST(str_assert) {
	char *a = "foo", *b = "fee";
	postabort_check = str_assert_postabort;
	assert_eq(a, b);
}
END_TEST

SUCCESS_TEST(str_assert_valid) {
	char *a = "fee", *b = "fee";
	assert_eq(a, b);
}
END_TEST

void strl_assert_postabort() {
	ck_assert_str_eq(stderr_data, "CRITICAL:tlog: Assertion 'a == b' failed: 'fo' == 'fe'\n");
}
FAIL_TEST(strl_assert) {
	char *a = "foo", *b = "fee";
	postabort_check = strl_assert_postabort;
	assert_eq(a, b, 2);
}
END_TEST

SUCCESS_TEST(strl_assert_valid) {
	char *a = "feo", *b = "fee";
	assert_eq(a, b, 2);
}
END_TEST

void floatl_assert_postabort() {
	ck_assert_str_eq(stderr_data, "CRITICAL:tlog: Assertion 'a == b' failed: 2.070000E+00 == 1.940000E+00\n");
}
FAIL_TEST(floatl_assert) {
	float a = 2.07, b = 1.94;
	postabort_check = floatl_assert_postabort;
	assert_eq(a, b, 0.08);
}
END_TEST

SUCCESS_TEST(floatl_assert_valid) {
	float a = 2.07, b = 1.94;
	assert_eq(a, b, 0.2);
}
END_TEST

void int_assert_ne_postabort() {
	ck_assert_str_eq(stderr_data, "CRITICAL:tlog: Assertion 'a != b' failed: 1 != 1\n");
}
FAIL_TEST(int_assert_ne) {
	int a = 1, b = 1;
	postabort_check = int_assert_ne_postabort;
	assert_ne(a, b);
}
END_TEST
SUCCESS_TEST(int_assert_ne_valid) {
	int a = 2, b = 1;
	assert_ne(a, b);
}
END_TEST

void int_assert_gt_postabort() {
	ck_assert_str_eq(stderr_data, "CRITICAL:tlog: Assertion 'a > b' failed: 1 > 2\n");
}
FAIL_TEST(int_assert_gt) {
	int a = 1, b = 2;
	postabort_check = int_assert_gt_postabort;
	assert_gt(a, b);
}
END_TEST
SUCCESS_TEST(int_assert_gt_valid) {
	int a = 2, b = 1;
	assert_gt(a, b);
}
END_TEST

void int_assert_ge_postabort() {
	ck_assert_str_eq(stderr_data, "CRITICAL:tlog: Assertion 'a >= b' failed: 1 >= 2\n");
}
FAIL_TEST(int_assert_ge) {
	int a = 1, b = 2;
	postabort_check = int_assert_ge_postabort;
	assert_ge(a, b);
}
END_TEST
SUCCESS_TEST(int_assert_ge_valid1) {
	int a = 2, b = 1;
	assert_ge(a, b);
}
END_TEST
SUCCESS_TEST(int_assert_ge_valid2) {
	int a = 1, b = 1;
	assert_ge(a, b);
}
END_TEST

void int_assert_lt_postabort() {
	ck_assert_str_eq(stderr_data, "CRITICAL:tlog: Assertion 'a < b' failed: 2 < 1\n");
}
FAIL_TEST(int_assert_lt) {
	int a = 2, b = 1;
	postabort_check = int_assert_lt_postabort;
	assert_lt(a, b);
}
END_TEST
SUCCESS_TEST(int_assert_lt_valid) {
	int a = 1, b = 2;
	assert_lt(a, b);
}
END_TEST

void int_assert_le_postabort() {
	ck_assert_str_eq(stderr_data, "CRITICAL:tlog: Assertion 'a <= b' failed: 2 <= 1\n");
}
FAIL_TEST(int_assert_le) {
	int a = 2, b = 1;
	postabort_check = int_assert_le_postabort;
	assert_le(a, b);
}
END_TEST
SUCCESS_TEST(int_assert_le_valid1) {
	int a = 1, b = 2;
	assert_le(a, b);
}
END_TEST
SUCCESS_TEST(int_assert_le_valid2) {
	int a = 1, b = 1;
	assert_le(a, b);
}
END_TEST

void floatl_assert_ne_postabort() {
	ck_assert_str_eq(stderr_data, "CRITICAL:tlog: Assertion 'a != b' failed: 2.070000E+00 != 1.940000E+00\n");
}
FAIL_TEST(floatl_assert_ne) {
	float a = 2.07, b = 1.94;
	postabort_check = floatl_assert_ne_postabort;
	assert_ne(a, b, 0.2);
}
END_TEST
SUCCESS_TEST(floatl_assert_ne_valid) {
	float a = 2.07, b = 1.94;
	assert_ne(a, b, 0.08);
}
END_TEST

void floatl_assert_gt_postabort() {
	ck_assert_str_eq(stderr_data, "CRITICAL:tlog: Assertion 'a > b' failed: 1.940000E+00 > 2.070000E+00\n");
}
FAIL_TEST(floatl_assert_gt) {
	float a = 1.94, b = 2.07;
	postabort_check = floatl_assert_gt_postabort;
	assert_gt(a, b, 0.08);
}
END_TEST
SUCCESS_TEST(floatl_assert_gt_valid1) {
	float a = 1.94, b = 2.07;
	assert_gt(a, b, 0.2);
}
END_TEST
SUCCESS_TEST(floatl_assert_gt_valid2) {
	float a = 2.07, b = 1.94;
	assert_gt(a, b, 0.08);
}
END_TEST
SUCCESS_TEST(floatl_assert_gt_valid3) {
	float a = 2.07, b = 0.94;
	assert_gt(a, b, 0.08);
}
END_TEST

void floatl_assert_lt_postabort() {
	ck_assert_str_eq(stderr_data, "CRITICAL:tlog: Assertion 'a < b' failed: 2.070000E+00 < 1.940000E+00\n");
}
FAIL_TEST(floatl_assert_lt) {
	float a = 2.07, b = 1.94;
	postabort_check = floatl_assert_lt_postabort;
	assert_lt(a, b, 0.08);
}
END_TEST
SUCCESS_TEST(floatl_assert_lt_valid1) {
	float a = 2.07, b = 1.94;
	assert_lt(a, b, 0.2);
}
END_TEST
SUCCESS_TEST(floatl_assert_lt_valid2) {
	float a = 1.94, b = 2.07;
	assert_lt(a, b, 0.08);
}
END_TEST
SUCCESS_TEST(floatl_assert_lt_valid3) {
	float a = 0.94, b = 2.07;
	assert_lt(a, b, 0.08);
}
END_TEST
