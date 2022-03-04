// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020, CZ.NIC z.s.p.o. (http://www.nic.cz/)
#include "level.h"

#define ENV_LOG_LEVEL_VAR "LOG_LEVEL"

static int log_level_from_env() {
	static int level = 0;
	static bool loaded = false;
	if (!loaded) {
		char *envlog = getenv(ENV_LOG_LEVEL_VAR);
		// atoi returns 0 on error and that is our default
		level = envlog ? atoi(envlog) : 0;
		loaded = true;
	}
	return level;
}

bool verbose_filter(int level, log_t log, const struct output *out) {
	return level - 
		(log->_log ? log->_log->level : 0) -
		(out ? out->level : 0) -
		log_level_from_env()
		>= 0;
}

int log_level(log_t log) {
	if (log->_log == NULL)
		return DEF_LEVEL;
	return log->_log->level;
}

void log_set_level(log_t log, int level) {
	log_allocate(log);
	log->_log->level = level;
}

void log_verbose(log_t log) {
	log_allocate(log);
	log->_log->level--;
}

void log_quiet(log_t log) {
	log_allocate(log);
	log->_log->level++;
}

void log_offset_level(log_t log, int offset) {
	log_allocate(log);
	log->_log->level += offset;
}
