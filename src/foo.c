// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021, CZ.NIC z.s.p.o. (http://www.nic.cz/)
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <foo.h>
#include "log.h"
#include "config.h"


int main(int argc, char **argv) {
	struct config *conf = load_config(argc, argv);

	FILE *f = stdin;
	bool close = false;
	if (conf->source_file != NULL && strcmp(conf->source_file, "-")) {
		f = fopen(conf->source_file, "r");
		if (f == NULL) {
			error("Can't open input file '%s'", conf->source_file);
			return 1;
		}
	}

	unsigned cnt = count_foo(f);
	printf("%u\n", cnt);

	if (close)
		fclose(f);
	return 0;
}
