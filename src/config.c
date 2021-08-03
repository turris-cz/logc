// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021, CZ.NIC z.s.p.o. (http://www.nic.cz/)
#include "config.h"
#include <unistd.h>
#include <argp.h>
#include <logc_argp.h>
#include <libconfig.h>
#include <logc_config.h>
#include "log.h"

static struct config conf = {
	.config_file = DEFAULT_CONFIG_FILE,
	.source_file = NULL,
};

static error_t parse_opt(int key, char *arg, struct argp_state *state);

const static struct argp_option argp_options[] = {
	{"config", 'f', "path",  0, "Path to config file"},
	{NULL}
};

const char *argp_program_version = PACKAGE_STRING;
const char *argp_program_bug_address = PACKAGE_BUGREPORT;
const static struct argp argp_parser = {
	.options = argp_options,
	.parser = parse_opt,
	.doc = "Counts number of lines starting with \"foo\"",
	.args_doc = "[FILE]",
	.children = (struct argp_child[]){{&logc_argp_parser, 0, "Logging", 2}, {NULL}},
};


static error_t parse_opt(int key, char *arg, struct argp_state *state) {
	switch (key) {
		case 'f':
			conf.config_file = arg;
			break;
		case ARGP_KEY_ARG:
			if (conf.source_file == NULL) {
				conf.source_file = arg;
				break;
			}
			__attribute__((fallthrough));
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static void load_config_file() {
	config_t cfg;
	config_init(&cfg);
	if (config_read_file(&cfg, conf.config_file) != CONFIG_TRUE) {
		switch(config_error_type(&cfg)) {
			case CONFIG_ERR_FILE_IO:
				warning("Couldn't open config file: %s", conf.config_file);
				config_destroy(&cfg);
				return;
			case CONFIG_ERR_PARSE:
				fatal(1,
					"Unable to parse configuration file '%s' (failed on line %d due to: %s)",
					conf.config_file, config_error_line(&cfg),
					config_error_text(&cfg));
			default:
				critical("Unknown error '%d' while reading file: %s",
					config_error_type(&cfg),
					conf.config_file);
		}
	}

	log_bind(log_foo, log_logc_config);
	logc_config_load(log_foo, &cfg);


	config_destroy(&cfg);
}


struct config *load_config(int argc, char **argv) {
	logc_argp_log = DEFLOG; // set our log to be configured by logc_argp
	argp_parse(&argp_parser, argc, argv, 0, NULL, NULL);
	load_config_file();
	return &conf;
}
