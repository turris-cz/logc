// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020, CZ.NIC z.s.p.o. (http://www.nic.cz/)
#include "log.h"

bool log_syslog(log_t log) {
	if (!log->_log)
		return log->daemon;
	return (!log->_log->no_syslog && log->daemon) || log->_log->syslog_format;
}

void log_syslog_format(log_t log, const char *format) {
	log_allocate(log);
	free_format(log->_log->syslog_format);
	log->_log->syslog_format = format ? parse_format(format) : NULL;
}

void log_syslog_fallback(log_t log, bool enabled) {
	log_allocate(log);
	log->_log->no_syslog = !enabled;
}
