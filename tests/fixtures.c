// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021, CZ.NIC z.s.p.o. (http://www.nic.cz/)
#include <stdio.h>
#include <errno.h>
#include <check.h>
#include "fixtures.h"

struct obstack t_obstack;

FILE* orig_stderr;
char *stderr_data;
size_t stderr_len;

log_t tlog;


void basic_setup() {
	obstack_init(&t_obstack);

	orig_stderr = stderr;
	stderr = open_memstream(&stderr_data, &stderr_len);

	tlog = malloc(sizeof *tlog);
	*tlog = (struct log) { .name = "tlog" };

	errno = 0; // Set to 0 to reset any previous possible error
}

void basic_teardown() {
	ck_assert_int_eq(errno, 0);

	log_free(tlog);
	free(tlog);

	fclose(stderr);
	stderr = orig_stderr;

	obstack_free(&t_obstack, NULL);
}
