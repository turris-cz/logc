// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020, CZ.NIC z.s.p.o. (http://www.nic.cz/)
#ifndef _LOGC_UTIL_INTERNAL_H_
#define _LOGC_UTIL_INTERNAL_H_
#include <logc.h>
#include <stdbool.h>
#include <syslog.h>


static inline bool str_empty(const char *str) {
	return !str || *str == '\0';
}

static inline int msg2syslog_level(enum log_message_level msg_level) {
	switch (msg_level) {
		case LL_CRITICAL:
			return LOG_CRIT;
		case LL_ERROR:
			return LOG_ERR;
		case LL_WARNING:
			return LOG_WARNING;
		case LL_NOTICE:
			return LOG_NOTICE;
		case LL_INFO:
			return LOG_INFO;
		case LL_DEBUG:
		case LL_TRACE:
			return LOG_DEBUG;
		default:
			return LOG_INFO;
	}
}

#endif
