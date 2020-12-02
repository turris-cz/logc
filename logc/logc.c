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
#include <logc.h>
#include <logc_util.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <syslog.h>
#include <unistd.h>
#include "format.h"

#define ENV_LOG_VAR "LOG_LEVEL"


struct log_output {
	FILE *f;
	int level;
	struct format *format;
	bool use_colors;
	bool is_terminal;
	bool autoclose;
};

struct _log {
	int level;
	struct log_output *outs;
	size_t outs_cnt;
	bool no_stderr;
	bool use_origin;
};


LOG(logc_internal)

static inline enum log_message_level message_level_sanity(int l) {
	return l > LL_CRITICAL ? LL_CRITICAL : l < LL_TRACE ? LL_TRACE : l;
}

static int level_from_env() {
	char *envlog = getenv(ENV_LOG_VAR);
	// atoi returns 0 on error and that is our default
	return envlog == NULL ? 0 : atoi(envlog);
}

static void log_allocate(log_t log) {
	if (log->_log)
		return;
	log->_log = malloc(sizeof *log->_log);
	*log->_log = (typeof(*log->_log)){
		.level = level_from_env(),
		.outs = NULL,
		.outs_cnt = 0,
		.no_stderr = false,
	};
}

static void free_log_output(struct log_output *out, bool close_f);

void log_free(log_t log) {
	if (!log->_log)
		return;
	log_wipe_outputs(log);
	free(log->_log);
	log->_log = NULL;
}

int log_level(log_t log) {
	log_allocate(log);
	return log->_log->level;
}

void log_set_level(log_t log, int level) {
	log_allocate(log);
	log->_log->level = level;
}

void log_verbose(log_t log) {
	log_allocate(log);
	printf("pre level %d\n", log->_log->level);
	log->_log->level--;
	printf("level %d\n", log->_log->level);
}

void log_quiet(log_t log) {
	log_allocate(log);
	log->_log->level++;
}

void log_offset_level(log_t log, int offset) {
	log_allocate(log);
	log->_log->level += offset;
}

bool log_would_log(log_t log, enum log_message_level level) {
	log_allocate(log);
	int loglevel = level - log->_log->level;
	if (log->_log->outs) {
		for (size_t i = 0; i < log->_log->outs_cnt; i++)
			if (loglevel >= log->_log->outs[i].level)
				return true;
	} else
		// Default output is used that has always zero as level.
		return loglevel >= 0;
	return false;
}

bool log_use_origin(log_t log) {
	return log->_log && log->_log->use_origin;
}

void log_set_use_origin(log_t log, bool use) {
	log_allocate(log);
	log->_log->use_origin = use;
}

static void new_log_output(struct log_output *out, FILE *f, int level, const char *format, int flags) {
	// TODO we can simplify format by removing color and terminal conditions
	*out = (typeof(*out)){
		.f = f,
		.level = level,
		.format = parse_format(format),
		.use_colors = (flags & LOG_F_COLORS) && !(flags & LOG_F_NO_COLORS),
		.is_terminal = false,
		.autoclose = flags & LOG_F_AUTOCLOSE,
	};

	int fd = fileno(f); errno = 0;
	if (fd != -1)
		out->is_terminal = isatty(fd);

	if (!(flags & (LOG_F_NO_COLORS | LOG_F_COLORS)))
		out->use_colors = out->is_terminal;
}

static void free_log_output(struct log_output *out, bool close_f) {
	if (!out)
		return;
	if (close && out->autoclose)
		fclose(out->f);
	free_format(out->format);
}

void log_add_output(log_t log, FILE *file, int flags, int level, const char *format) {
	log_allocate(log);
	size_t index = log->_log->outs_cnt;
	for (size_t i = 0; i < log->_log->outs_cnt; i++) // Locate if already present
		if (file == log->_log->outs[i].f) {
			free_log_output(log->_log->outs + i, false);
			index = i;
			break;
		}

	// We do not expect huge amount of outputs so optimizing addition to array for
	// speed is less beneficial over optimizing for memory (fitting exactly)
	if (index == log->_log->outs_cnt)
		log->_log->outs = realloc(log->_log->outs,
				++log->_log->outs_cnt * sizeof(struct log_output));

	new_log_output(log->_log->outs + index, file, level, format, flags);
}

bool log_rm_output(log_t log, FILE *file) {
	log_allocate(log);
	for (size_t i = 0; i < log->_log->outs_cnt; i++) {
		struct log_output *out = log->_log->outs + i;
		if (out->f == file) {
			free_log_output(out, true);
			log->_log->outs_cnt--;
			memmove(out, out + 1, (log->_log->outs_cnt - i) * sizeof *out);
			log->_log->outs = realloc(log->_log->outs,
				log->_log->outs_cnt * sizeof *out);
			return true;
		}
	}
	return false;
}

void log_wipe_outputs(log_t log) {
	if (!log->_log)
		return;
	for (size_t i = 0; i < log->_log->outs_cnt; i++)
		free_log_output(log->_log->outs + i, true);
	free(log->_log->outs);
	log->_log->outs_cnt = 0;
	log->_log->outs = NULL;
}

void log_stderr_fallback(log_t log, bool enabled) {
	log_allocate(log);
	log->_log->no_stderr = !enabled;
}

void log_flush(log_t log) {
	if (log->_log)
		for (size_t i = 0; i < log->_log->outs_cnt; i++)
			fflush(log->_log->outs[i].f);
	fflush(stderr); // alway flush stderr to cover cases when outs were just added
};


void log_syslog_enable(log_t log) {
	NOT_IMPLEMENTED;
}

void log_syslog_enablef(log_t log, const char *format) {
	NOT_IMPLEMENTED;
}

void log_syslog_disable(log_t log) {
	NOT_IMPLEMENTED;
}

bool log_syslog_enabled(log_t log) {
	NOT_IMPLEMENTED;
}


void log_chain(log_t master, log_t slave) {
	NOT_IMPLEMENTED;
}

void log_unchain(log_t master, log_t slave) {
	NOT_IMPLEMENTED;
}


static struct log_output *default_stderr_output() {
	static struct log_output *out = NULL;
	if (out && out->f != stderr) {
		free_log_output(out, false);
		out = NULL;
	}
	if (out == NULL) {
		out = malloc(sizeof *out);
		new_log_output(out, stderr, 0, LOG_FORMAT_DEFAULT, 0);
	}
	return out;
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
		const char *format, ...) {
	int stderrno = errno;
	msg_level = message_level_sanity(msg_level);
	if (!log_would_log(log, msg_level))
		return;

	va_list args;
	va_start(args, format);
	size_t msg_size = vsnprintf(NULL, 0, format, args);
	va_end(args);
	char msg[msg_size + 1];
	va_start(args, format);
	vsprintf(msg, format, args);
	va_end(args);

	size_t cnt = 1;
	struct log_output *outs = default_stderr_output();
	if (log->_log) {
		if (log->_log->outs_cnt) {
			cnt = log->_log->outs_cnt;
			outs = log->_log->outs;
		} else if (log->_log->no_stderr)
			cnt = 0;
	}
	bool use_origin = log->_log && log->_log->use_origin;

	int level = msg_level - log->_log->level;
	for (size_t i = 0; i < cnt; i++) {
		struct log_output *out = outs + i;
		if (level < out->level)
			continue;
		const struct format *format = outs[i].format;
		do {
			switch (format->type) {
				case FF_TEXT:
					fputs(format->text, out->f);
					break;
				case FF_MESSAGE:
					fputs(msg, out->f);
					break;
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
							msg_size == 0, stderrno == 0);
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
