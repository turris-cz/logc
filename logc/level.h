// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020, CZ.NIC z.s.p.o. (http://www.nic.cz/)
#ifndef _LOGC_LEVEL_H_
#define _LOGC_LEVEL_H_
#include "log.h"
#include <logc.h>

bool verbose_filter(enum log_message_level, log_t, const struct output *out)
	__attribute__((nonnull(2)));

#endif
