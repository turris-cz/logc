// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020, CZ.NIC z.s.p.o. (http://www.nic.cz/)
#ifndef _LOGC_OUTPUT_H_
#define _LOGC_OUTPUT_H_
#include "log.h"
#include <logc.h>
#include <sys/types.h>
#include "format.h"

struct output {
	FILE *f;
	int fd;
	int level;
	const struct format *format;
	bool free_format;
	bool use_colors;
	bool is_terminal;
	bool autoclose;
};

void new_output(struct output *out, FILE *f, int level,
		const char *format, int flags);
void free_output(struct output *out, bool close_f);

void syslog_output(struct output *out, char **str, size_t *str_len,
		const struct format *format);
void free_syslog_output(struct output *out);

const struct output *default_stderr_output();

void lock_output(const struct output *out);
void unlock_output(const struct output *out);

#endif
