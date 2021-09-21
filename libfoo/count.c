// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021, CZ.NIC z.s.p.o. (http://www.nic.cz/)
#include <stdio.h>
#include <string.h>
#include "log.h"

#include "match.gperf.h"

unsigned count_foo(FILE *f) {
	unsigned res = 0;
	char *line = NULL;
	size_t len = 0;
	ssize_t nread;
	while ((nread = getline(&line, &len, f)) != -1) {
		trace("Read line: %.*s", (int)nread, line);
		char *colon = strchr(line, ':');
		if (colon == NULL)
			continue; // ignore lines without colon
		const struct gperf_match *gm = gperf_match(line, colon - line);
		if (gm != NULL && gm->matches)
			res++;
	}
	free(line);
	return res;
}
