// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020, CZ.NIC z.s.p.o. (http://www.nic.cz/)
#include <logc_argp.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>


log_t logc_argp_log = NULL;

enum {
	ARGPO_LOG_FILE = 1050,
	ARGPO_SYSLOG,
	ARGPO_NO_SYSLOG,
	ARGPO_LOG_LEVEL
};

#define BASE_OPTIONS \
	{"verbose", 'v', NULL, 0, "Increase output verbosity", 1},  \
	{"quiet", 'q', NULL, 0, "Decrease output verbosity", 1}, \
	{"log-level", ARGPO_LOG_LEVEL, "level", 0, "Set output verbosity level to provided value", 1}, \
	{"log-file", ARGPO_LOG_FILE, "file", 0, "Send logs to provided log file.", 2}

static const struct argp_option options[] = {
	BASE_OPTIONS,
	{"syslog", ARGPO_SYSLOG, NULL, 0, "Send logs to the syslog.", 2},
	{NULL}
};

static const struct argp_option options_daemon[] = {
	BASE_OPTIONS,
	{"no-syslog", ARGPO_NO_SYSLOG, NULL, 0, "Do not send logs to the syslog.", 2},
	{NULL}
};


static int parse_level(const char *arg, struct argp_state *state) {
	char *end_ptr;
	errno = 0;
	long int result = strtol(arg, &end_ptr, 10);
	if (errno) // conversion errors
		argp_error(state, "Invalid log level. Conversion error: %s",
			strerror(errno));
	else
		if (end_ptr == arg || *end_ptr != '\0')
			argp_error(state, "Invalid log level value. It must be integer.");
	return result;
}

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
	if (logc_argp_log == NULL)
		argp_error(state, "Invalid usage. Please set logc_argp_log variable before argp_parse.");
	switch (key) {
		case 'v':
			log_verbose(logc_argp_log);
			break;
		case 'q':
			log_quiet(logc_argp_log);
			break;
		case ARGPO_LOG_FILE: {
			FILE *file = fopen(arg, "a");
			if (file == NULL)
				argp_error(state, "Unable to open file '%s' for writing: %s", arg, strerror(errno));
			else
				log_add_output(logc_argp_log, file, LOG_F_AUTOCLOSE, 0, LOG_FORMAT_DEFAULT);
			break; }
		case ARGPO_SYSLOG:
			log_syslog_format(logc_argp_log, LOG_FORMAT_DEFAULT);
			// Intentional fall trough
		case ARGPO_NO_SYSLOG:
			log_syslog_fallback(logc_argp_log, false);
			break;
		case ARGPO_LOG_LEVEL:
			log_set_level(logc_argp_log, parse_level(arg, state));
			break;
		default:
			return ARGP_ERR_UNKNOWN;
	};
	return 0;
}


const struct argp logc_argp_parser = {
	.options = options,
	.parser = parse_opt,
};

const struct argp logc_argp_daemon_parser = {
	.options = options_daemon,
	.parser = parse_opt,
};
