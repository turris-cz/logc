// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020, CZ.NIC z.s.p.o. (http://www.nic.cz/)
#include "log.h"
#include "util.h"

#define ENV_LOG_ORIGIN "LOG_ORIGIN"

static bool log_origin_from_env() {
	static bool log_origin = DEF_USE_ORIGIN;
	static bool loaded = false;
	if (!loaded)
		log_origin = !str_empty(getenv(ENV_LOG_ORIGIN));
	return log_origin;
}

bool log_use_origin(log_t log) {
	if (log->_log)
		return log->_log->use_origin;
	else
		return log_origin_from_env();
}

void log_set_use_origin(log_t log, bool use) {
	log_allocate(log);
	log->_log->use_origin = use;
}
