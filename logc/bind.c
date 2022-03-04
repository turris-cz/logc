// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021, CZ.NIC z.s.p.o. (http://www.nic.cz/)
#include "log.h"

void log_bind(log_t dominant, log_t submissive) {
	log_allocate(submissive);
	submissive->_log->dominator = dominant;
}

log_t log_bound(log_t log) {
	if (log->_log == NULL)
		return NULL;
	return log->_log->dominator;
}

void log_unbind(log_t log) {
	if (log->_log == NULL)
		return;
	log->_log->dominator = NULL;
}
