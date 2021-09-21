// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021, CZ.NIC z.s.p.o. (http://www.nic.cz/)
#include <stdio.h>
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


const char *test_files() {
	static char *path = NULL;
	if (path == NULL) {
		path = getenv("TEST_FILES");
		if (path == NULL)
			ck_abort_msg("The TEST_FILES environment variable is not set");
	}
	return path;
}

char *test_file(const char *file) {
	obstack_printf(&t_obstack, "%s/%s", test_files(), file);
	obstack_grow0(&t_obstack, "", 0);
	return obstack_finish(&t_obstack);
}
