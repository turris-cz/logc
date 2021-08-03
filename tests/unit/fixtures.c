// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021, CZ.NIC z.s.p.o. (http://www.nic.cz/)
#include <errno.h>
#include <check.h>
#include "fixtures.h"

struct obstack t_obstack;


void basic_setup() {
	obstack_init(&t_obstack);

	errno = 0; // Set to 0 to reset any previous possible error
}

void basic_teardown() {
	ck_assert_int_eq(errno, 0);

	obstack_free(&t_obstack, NULL);
}
