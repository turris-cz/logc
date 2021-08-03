// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021, CZ.NIC z.s.p.o. (http://www.nic.cz/)
#include <string.h>
#define SUITE "count"
#include "unittests.h"

#include <foo.h>

TEST_CASE(simple) {}

TEST(simple, empty) {
	char *text = "";
	FILE *f = fmemopen(text, strlen(text), "r");
	ck_assert_int_eq(count_foo(f), 0);
	fclose(f);
}
END_TEST

TEST(simple, simple) {
	char *text = "foo: well\nups: nope\nfoo: fee\n";
	FILE *f = fmemopen(text, strlen(text), "r");
	ck_assert_int_eq(count_foo(f), 2);
	fclose(f);
}
END_TEST
