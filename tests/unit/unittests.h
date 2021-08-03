// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021, CZ.NIC z.s.p.o. (http://www.nic.cz/)
#ifndef _UNITTESTS_H_
#define _UNITTESTS_H_
#include <check.h>
#include <math.h>
#include "fixtures.h"

void unittests_add_suite(Suite*);

#ifndef SUITE
#error Missing SUITE definition to name suite
#endif
static void unittests_add_tcase(TCase *tcase) {
	static Suite *suite = NULL;
	if (suite == NULL) {
		suite = suite_create(SUITE);
		unittests_add_suite(suite);
	}
	suite_add_tcase(suite, tcase);
}


#ifndef DEFAULT_SETUP
#define DEFAULT_SETUP basic_setup
#endif
#ifndef DEFAULT_TEARDOWN
#define DEFAULT_TEARDOWN basic_teardown
#endif

#define _TEST_CASE(name, setup, teardown, timeout) \
	static void _tcase_##name(TCase *tcase); \
	static TCase *unittests_tcase_##name() { \
		static TCase *tcase = NULL; \
		if (tcase == NULL) { \
			tcase = tcase_create(#name); \
			tcase_add_checked_fixture(tcase, setup, teardown); \
			if (!isnan(timeout)) \
				tcase_set_timeout(tcase, timeout); \
			unittests_add_tcase(tcase); \
			_tcase_##name(tcase); \
		} \
		return tcase; \
	} \
	static void _tcase_##name(TCase *tcase)
#define __test_case1(name) _TEST_CASE(name, DEFAULT_SETUP, DEFAULT_TEARDOWN, NAN)
#define __test_case2(name, setup) _TEST_CASE(name, setup, DEFAULT_TEARDOWN, NAN)
#define __test_case3(name, setup, teardown) _TEST_CASE(name, setup, teardown, NAN)
#define __test_case_select(_1, _2, _3, _4, X, ...) X
#define TEST_CASE(...) \
	__test_case_select(__VA_ARGS__, _TEST_CASE, __test_case3, __test_case2, __test_case1)(__VA_ARGS__)


#define _TEST(tcase_add, tcase, name, ...) \
	static const TTest *name; \
	__attribute__((constructor)) static void _test_##name() { \
		tcase_add(unittests_tcase_##tcase(), __VA_ARGS__); \
	} \
	START_TEST(name)

#define TEST(tcase, name) \
	_TEST(tcase_add_test, tcase, name, name)
#define TEST_RAISE_SIGNAL(tcase, name, signal) \
	_TEST(tcase_add_test_raise_signal, tcase, name, name, signal)
#define TEST_EXIT(tcase, name, exit_value) \
	_TEST(tcase_add_exit_test, tcase, name, name, exit_value)

#define LOOP_TEST(tcase, name, s, e) \
	_TEST(tcase_add_loop_test, tcase, name, name, s, e)
#define LOOP_TEST_RAISE_SIGNAL(tcase, name, signal, s, e) \
	_TEST(tcase_add_test_raise_signal, tcase, name, name, signal, s, e)
#define LOOP_TEST_EXIT(tcase, name, exit_value, s, e) \
	_TEST(tcase_add_exit_test, tcase, name, name, exit_value, s, e)

#define _ARRAY_TEST(tcase_add, tcase, name, array, ...) \
	static void name##_array_fn(__typeof__(*array) _d CK_ATTRIBUTE_UNUSED); \
	_TEST(tcase_add, tcase, name, __VA_ARGS__, 0, sizeof(array) / sizeof(*(array))) { \
		name##_array_fn(array[_i]); \
	} \
	static void name##_array_fn(__typeof__(*array) _d CK_ATTRIBUTE_UNUSED)

#define ARRAY_TEST(tcase, name, array) \
	_ARRAY_TEST(tcase_add_loop_test, tcase, name, array, name)
#define ARRAY_TEST_RAISE_SIGNAL(tcase, name, signal, array) \
	_ARRAY_TEST(tcase_add_loop_test_raise_signal, tcase, name, array, name, signal)
#define ARRAY_TEST_EXIT(tcase, name, exit_value, array) \
	_ARRAY_TEST(tcase_add_loop_exit_test, tcase, name, array, name, exit_value)

#endif
