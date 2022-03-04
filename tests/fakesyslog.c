// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020, CZ.NIC z.s.p.o. (http://www.nic.cz/)
#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "fakesyslog.h"

struct fakesyslog *fakesyslog = NULL;
size_t fakesyslog_cnt = 0;

void fakesyslog_reset() {
	fakesyslog_free();
	fakesyslog = NULL;
	fakesyslog_cnt = 0;
}

void fakesyslog_free() {
	for (size_t i = 0; i < fakesyslog_cnt; i++)
		free(fakesyslog[i].msg);
	free(fakesyslog);
}

extern void vsyslog(int priority, const char *format, va_list args) {
	fakesyslog = realloc(fakesyslog, (fakesyslog_cnt + 1) * sizeof *fakesyslog);
	fakesyslog[fakesyslog_cnt].priority = priority;
	ck_assert_int_ne(-1, vasprintf(&fakesyslog[fakesyslog_cnt].msg, format, args));
	fakesyslog_cnt++;
}

void __vsyslog_chk(int priority, int flag, const char *format, va_list args) {
	vsyslog(priority, format, args);
}

void syslog (int priority, const char *format, ...) {
	va_list args;
	va_start(args, format);
	vsyslog(priority, format, args);
	va_end(args);
}

void __syslog_chk(int priority, int flag, const char *format, ...) {
	va_list args;
	va_start(args, format);
	__vsyslog_chk(priority, flag, format, args);
	va_end(args);
}
