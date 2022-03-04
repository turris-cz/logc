// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020, CZ.NIC z.s.p.o. (http://www.nic.cz/)
#ifndef _LOGC_ASSERT_H_
#define _LOGC_ASSERT_H_

// Logc compatible replacement for assert.h
#define log_assert(log, expr) ({ \
		if (expr); else \
			log_critical(log, "Assertion '%s' failed", #expr); \
	})


#endif

#ifdef DEFLOG
#ifndef _LOGC_ASSERT_H_DEFLOG
#define _LOGC_ASSERT_H_DEFLOG

#define assert(expr) log_assert(DEFLOG, expr)

#endif
#endif
