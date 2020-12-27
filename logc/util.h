/* Copyright (c) 2020 CZ.NIC z.s.p.o. (http://www.nic.cz/) *
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
#ifndef _LOGC_UTIL_INTERNAL_H_
#define _LOGC_UTIL_INTERNAL_H_
#include <logc.h>
#include <stdbool.h>
#include <syslog.h>


inline bool str_empty(const char *str) {
	return !str || *str == '\0';
}

inline int msg2syslog_level(enum log_message_level msg_level) {
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
	}
}

#endif
