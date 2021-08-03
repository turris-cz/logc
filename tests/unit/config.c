// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021, CZ.NIC z.s.p.o. (http://www.nic.cz/)
#include <string.h>

#define SUITE "count"
#include "unittests.h"

#include "config.h"

// Test various command line arguments
TEST_CASE(arguments) {}

char *parse_arguments_arg0[] = {"foo"};
char *parse_arguments_arg1[] = {"foo", "--config", "/dev/null"};
char *parse_arguments_arg2[] = {"foo", "/dev/null"};
char *parse_arguments_arg3[] = {"foo", "--config", "/dev/null", "/dev/null"};

#define ARGUMENTS(arg) .argc = sizeof(arg) / sizeof(*(arg)), .argv = arg
const struct {
	struct config config;
	int argc;
	char **argv;
} parse_arguments_data[] = {
	{
		.config = {
			.config_file = DEFAULT_CONFIG_FILE,
			.source_file = NULL,
		},
		ARGUMENTS(parse_arguments_arg0),
	},
	{
		.config = {
			.config_file = "/dev/null",
			.source_file = NULL,
		},
		ARGUMENTS(parse_arguments_arg1),
	},
	{
		.config = {
			.config_file = DEFAULT_CONFIG_FILE,
			.source_file = "/dev/null",
		},
		ARGUMENTS(parse_arguments_arg2),
	},
	{
		.config = {
			.config_file = "/dev/null",
			.source_file = "/dev/null",
		},
		ARGUMENTS(parse_arguments_arg3),
	},
};

ARRAY_TEST(arguments, parse_arguments, parse_arguments_data) {
	struct config *conf = load_config(_d.argc, _d.argv);
	ck_assert_str_eq(conf->config_file, _d.config.config_file);
	ck_assert_pstr_eq(conf->source_file, _d.config.source_file);
}
END_TEST


// Test configuration loading
TEST_CASE(config) {}
