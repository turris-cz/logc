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
#define DEFLOG log_logc_internal
#include "log.h"
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <syslog.h>
#include <unistd.h>
#include "log.h"
#include "format.h"
#include "output.h"
#include "level.h"

LOG(logc_internal)

static inline enum log_message_level message_level_sanity(int l) {
	return l > LL_CRITICAL ? LL_CRITICAL : l < LL_TRACE ? LL_TRACE : l;
}

void log_allocate(log_t log) {
	if (log->_log)
		return;
	log->_log = malloc(sizeof *log->_log);
	*log->_log = (typeof(*log->_log)){
		.level = 0,
		.outs = NULL,
		.outs_cnt = 0,
		.no_stderr = false,
		.use_origin = false,
	};
}

void log_free(log_t log) {
	if (!log->_log)
		return;
	log_wipe_outputs(log);
	free(log->_log);
	log->_log = NULL;
}

bool log_would_log(log_t log, enum log_message_level msg_level) {
	if (log->_log && log->_log->outs) {
		for (size_t i = 0; i < log->_log->outs_cnt; i++)
			if (verbose_filter(msg_level, log, &log->_log->outs[i]))
				return true;
		return false;
	} else
		return verbose_filter(msg_level, log, NULL);
}

bool log_use_origin(log_t log) {
	return log->_log && log->_log->use_origin;
}

void log_set_use_origin(log_t log, bool use) {
	log_allocate(log);
	log->_log->use_origin = use;
}



static inline bool str_empty(const char *str) {
	return !str || *str == '\0';
}

static const struct format *if_seek_forward(const struct format *format,
		enum log_message_level level, bool is_term, bool colors, bool use_origin,
		bool no_log_name, bool no_msg, bool no_err) {
	bool is_not_empty_check = true;
	bool skip = false;
	switch (format->condition) {
		case FIFC_NON_EMPTY:
			is_not_empty_check = false;
			break;
		case FIFC_LEVEL:
			skip = level < format->if_level;
			break;
		case FIFC_TERMINAL:
			skip = !is_term;
			break;
		case FIFC_COLORED:
			skip = !colors;
			break;
	}

	if (is_not_empty_check) {
		if (skip == format->if_invert)
			return format;

		size_t depth = 0;
		while (format) {
			switch (format->type) {
				case FF_IF:
					depth++;
					break;
				case FF_ELSE:
					if (depth > 1)
						break;
				case FF_IFEND:
					depth--;
					if (depth == 0)
						return format;
				default: // ignore anything else
					break;
			}
			format = format->next;
		};
		return NULL;
	}

	for (const struct format *f = format->next; f; f = f->next) {
		bool empty = true;
		switch (f->type) {
			case FF_TEXT:
				// We ignore text as that is part of format and not expanded field
				break;
			case FF_MESSAGE:
				empty = no_msg;
				break;
			case FF_NAME:
				empty = no_log_name;
				break;
			case FF_SOURCE_FILE:
			case FF_SOURCE_LINE:
			case FF_SOURCE_FUNC:
				empty = !use_origin;
				break;
			case FF_STD_ERR:
				empty = no_err;
				break;
			case FF_IF:
				// Use recurse to skip to FF_IFEND if condition is not satisfied
				f = if_seek_forward(f, level, is_term, colors, use_origin, no_log_name, no_msg, no_err);
				// This condition is empty if we skipped it. Otherwise it is not.
				empty = f->type == FF_IFEND;
				break;
			case FF_ELSE:
			case FF_IFEND:
				return f;
		}
		if (!empty)
			return format;
	}
	return NULL;
}

static const struct format *else_seek_forward(const struct format *format) {
	size_t depth = 1;
	for (; format; format = format->next) {
		switch (format->type) {
			case FF_IF:
				depth++;
				break;
			case FF_IFEND:
				depth--;
				if (depth == 0)
					return format;
				break;
			default:
				// Ignore the rest
				break;
		}
	}
	return NULL;
}

void _log(log_t log, enum log_message_level msg_level,
		const char *file, size_t line, const char *func,
		const char *msgformat, ...) {
	int stderrno = errno;
	msg_level = message_level_sanity(msg_level);

	size_t cnt = 1;
	struct log_output *outs = default_stderr_output();
	if (log->_log) {
		if (log->_log->outs_cnt) {
			cnt = log->_log->outs_cnt;
			outs = log->_log->outs;
		} else if (log->_log->no_stderr)
			cnt = 0;
	}
	bool use_origin = log_use_origin(log);

	va_list args;
	va_start(args, msgformat);
	bool msg_empty = vsnprintf(NULL, 0, msgformat, args) == 0;
	va_end(args);

	for (size_t i = 0; i < cnt; i++) {
		struct log_output *out = outs + i;
		if (!verbose_filter(msg_level, log, &outs[i]))
			continue;
		const struct format *format = outs[i].format;
		do {
			switch (format->type) {
				case FF_TEXT:
					fputs(format->text, out->f);
					break;
				case FF_MESSAGE: {
					va_list args;
					va_start(args, msgformat);
					fprintf(out->f, msgformat, args);
					va_end(args);
					break; }
				case FF_NAME:
					if (log->name)
						fputs(log->name, out->f);
					break;
				case FF_SOURCE_FILE:
					if (use_origin)
						fputs(file, out->f);
					break;
				case FF_SOURCE_LINE:
					if (use_origin)
						fprintf(out->f, "%zu", line);
					break;
				case FF_SOURCE_FUNC:
					if (use_origin)
						fputs(func, out->f);
					break;
				case FF_STD_ERR:
					if (stderrno)
						fputs(strerror(stderrno), out->f);
					break;
				case FF_IF:
					format = if_seek_forward(format, msg_level, out->is_terminal,
							out->use_colors, use_origin, str_empty(log->name),
							msg_empty, stderrno == 0);
					break;
				case FF_ELSE:
					// Just skip else block as it is termination of valid
					// condition.
					format = else_seek_forward(format);
					break;
				case FF_IFEND:
					// Everything already done in FF_IF
					break;
			}
			format = format->next;
		} while (format);
		fputc('\n', out->f);
	}

	errno = 0; // always end with errno zero
}
