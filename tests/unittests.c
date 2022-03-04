// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020, CZ.NIC z.s.p.o. (http://www.nic.cz/)
#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

static SRunner *runner = NULL;

void unittests_add_suite(Suite *s) {
	if (runner == NULL)
		runner = srunner_create(NULL);
	srunner_add_suite(runner, s);
}


int main(void) {
	srunner_set_fork_status(runner, CK_FORK); // We have to fork to catch signals

	srunner_run_all(runner, CK_ENV);
	int failed = srunner_ntests_failed(runner);

	srunner_free(runner);
	return (bool)failed;
}
