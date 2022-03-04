// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020, CZ.NIC z.s.p.o. (http://www.nic.cz/)
#ifndef _LOGC_UTIL_H_
#define _LOGC_UTIL_H_
#include <logc.h>
#include <logc_assert.h>

// It is common to have prepared functions in program without functional content.
// In C this just commonly leads to undocumented behavior but most other languages
// provide some form of common error for such situation. This tries to provide the
// same. Writing this to unimplemented functions prevents you from looking for
// errors in rest of the code when empty function is called.
#define log_not_implemented(log) log_critical(log, "%s not implemented yet", __func__)

// In standard C library most of the functions report error by setting errno and
// returning value -1. It is common to ignore return codes but they sometimes can
// help you debug program execution. It is better to wrap such function calls with
// this macro instead of just not doing anything. Secondary it also mutes compiler
// warning about unused return value.
#define log_std_ignore(log, stdexpr) ({ \
		if ((stdexpr) == -1) \
			log_trace(log, "Ignored fail of '%s'", #stdexpr); \
	});

// This is variant of std_ignore that instead of ignoring error terminates
// execution. This is for same type of functions as std_ignore but for cases where
// failure is most unlikely. Using this simplifies error handling while still
// easing debugging if unlikely case occurs.
// Compared to std_ignore this also provides result value.
#define log_std_fatal(log, stdexpr) ({ \
		int _std_res = (stdexpr); \
		if (_std_res == -1) \
			log_critical(log, "Unexpected fail of '%s'", #stdexpr); \
		_std_res; \
	});

#endif

#ifdef DEFLOG
#ifndef _LOGC_UTIL_H_DEFLOG
#define _LOGC_UTIL_H_DEFLOG

#define not_implemented log_not_implemented(DEFLOG)
#define std_ignore(...) log_std_ignore(DEFLOG, __VA_ARGS__)
#define std_fatal(...) log_std_fatal(DEFLOG, __VA_ARGS__)

#endif
#endif
