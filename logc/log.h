// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020, CZ.NIC z.s.p.o. (http://www.nic.cz/)
#ifndef _LOGC_LOG_H_
#define _LOGC_LOG_H_
#define DEFLOG log_logc_internal
#include <logc.h>
#include <logc_util.h>
#include "output.h"
#include "format.h"

struct _log {
	int level;
	struct log *dominator;
	struct output *outs;
	size_t outs_cnt;
	struct format *syslog_format;
	bool no_stderr;
	bool no_syslog;
	bool use_origin;
};

#define DEF_LEVEL 0
#define DEF_NO_STDERR false
#define DEF_NO_SYSLOG false
#define DEF_USE_ORIGIN false
extern const struct _log _log_default;

void log_allocate(log_t log);

#endif
