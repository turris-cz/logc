// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021, CZ.NIC z.s.p.o. (http://www.nic.cz/)
#ifndef _FOO_CONFIG_H_
#define _FOO_CONFIG_H_

struct config {
	const char *config_file;
	const char *source_file;
};


// Parse arguments and load configuration file
// Returned pointer is to statically allocated (do not call free on it).
struct config *load_config(int argc, char **argv) __attribute__((nonnull));

#endif
