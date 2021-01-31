/* Copyright (c) 2020 CZ.NIC z.s.p.o. (http://www.nic.cz/)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <logc_argp.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>


log_t logc_argp_log = NULL;

enum {
	ARGPO_LOG_FILE = 1050,
};

static const struct argp_option options[] = {
	{"verbose", 'v', NULL, 0, "Increase output verbosity", 1},
	{"quiet", 'q', NULL, 0, "Decrease output verbosity", 1},
	{"log-file", ARGPO_LOG_FILE, "file", 0, "Send logs to provided log file. This disables logging to stderr.", 2},
	{NULL}
};

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
		default:
			return ARGP_ERR_UNKNOWN;
	};
	return 0;
}

const struct argp logc_argp_parser = {
	.options = options,
	.parser = parse_opt,
};
