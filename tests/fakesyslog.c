/* Copyright (c) 2020 CZ.NIC z.s.p.o. (http://www.nic.cz/)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
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