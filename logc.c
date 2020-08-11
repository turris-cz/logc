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
#define DEFLOG log_logc_internal
#include <logc.h>
#include <logc_util.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <syslog.h>
#include <unistd.h>

#define DEFAULT_LOG_LEVEL LL_NOTICE
#define ENV_LOG_VAR "LOG_LEVEL"

enum format_fields {
	FF_TEXT,
	FF_MESSAGE,
	FF_NAME,
	FF_SOURCE_FILE,
	FF_SOURCE_LINE,
	FF_SOURCE_FUNC,
	FF_LEVEL,
	FF_LEVEL_LOWCASE,
	FF_STD_ERR,
	FF_IF,
	FF_IFEND,
};

enum format_if_condition {
	FIFC_NON_EMPTY,
	FIFC_LEVEL,
	FIFC_TERMINAL,
	FIFC_COLORED,
};

struct format {
	enum format_fields type;

	char *text;

	enum format_if_condition condition;
	bool if_invert;
	enum log_level if_level;

	struct format *next;
};

struct log_output {
	FILE *f;
	struct format *format;
	bool use_colors;
	bool is_terminal;
	bool autoclose;
};

struct _log {
	enum log_level level;
	struct log_output *outs;
	size_t outs_cnt;
	bool no_stderr;
};

static const  struct level_info {
	const char *name;
	int syslog_prio;
} levels_info[] = {
	[LL_CRITICAL] = { "CRITICAL", LOG_CRIT },
	[LL_ERROR] = { "ERROR", LOG_ERR },
	[LL_WARNING] = { "WARNING", LOG_WARNING },
	[LL_NOTICE] = { "NOTICE", LOG_NOTICE },
	[LL_INFO] = { "INFO", LOG_INFO },
	[LL_DEBUG] = { "DEBUG", LOG_DEBUG },
	[LL_TRACE] = { "TRACE", LOG_DEBUG }
};


LOG(logc_internal)


static void free_format(struct format*);

static inline enum log_level level_sanity(int l) {
	return l > LL_CRITICAL ? LL_CRITICAL : l < LL_TRACE ? LL_TRACE : l;
}

#include "envlog.gperf.c"

static enum log_level log_level_from_env() {
	const enum log_level undef = LL_CRITICAL + 1;
	static enum log_level level = undef;
	if (level == undef) {
		char *envlog = getenv(ENV_LOG_VAR);
		const struct envlog_level *envlevel = gperf_envlog(envlog, envlog ? strlen(envlog) : 0);
		level = envlevel ? ((envlevel->level) ?: DEFAULT_LOG_LEVEL) : DEFAULT_LOG_LEVEL;
	}
	return level;
}

static void log_allocate(log_t log) {
	if (log->_log)
		return;
	log->_log = malloc(sizeof *log->_log);
	*log->_log = (typeof(*log->_log)){
		.level = log_level_from_env(),
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

enum log_level log_level(log_t log) {
	log_allocate(log);
	return log->_log->level;
}

void log_set_level(log_t log, enum log_level level) {
	log_allocate(log);
	log->_log->level = level_sanity(level);
}

void log_verbose(log_t log) {
	log_allocate(log);
	log->_log->level = level_sanity(--log->_log->level);
}

void log_quiet(log_t log) {
	log_allocate(log);
	log->_log->level = level_sanity(++log->_log->level);
}

bool log_would_log(log_t log, enum log_level level) {
	level = level_sanity(level);
	enum log_level set_level = log->_log ? log->_log->level : log_level_from_env();
	return level >= set_level;
}

#include "format.gperf.c"

static struct format *parse_format(const char *format) {
	struct format *first = NULL;
	struct format *f;
	while (*format != '\0') {
		struct format *new = malloc(sizeof *new);
		bool plain_text = true;
		if (*format == '%') {
			format++;
			const struct gperf_format *fg;
			size_t len = 0;
			do fg = gperf_format(format, ++len); while (!fg && len <= 2);
			if (fg != NULL) {
				*new = fg->f;
				format += len;
				plain_text = false;
			}
			// Note: if fd == NULL then we eat up %
		}
		if (plain_text) {
			// First character is not considered as if it is % it was already detected
			const char *next = strchrnul(format + 1, '%');
			*new = (struct format){
				.type = FF_TEXT,
				.text = strndup(format, next - format),
			};
			format = next;
		}
		if (first)
			f->next = new;
		else
			first = new;
		f = new;
	}
	return first;
}

static void free_format(struct format *f) {
	while (f) {
		struct format *tmp = f;
		f = f->next;
		free(tmp->text);
		free(tmp);
	}
}

static void new_log_output(struct log_output *out, FILE *f, const char *format, int flags) {
	// TODO we can simplify format by removing color and terminal conditions
	*out = (typeof(*out)){
		.f = f,
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

void log_add_output(log_t log, FILE *file, int flags, enum log_level level, const char *format) {
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

	new_log_output(log->_log->outs + index, file, format, flags);
	// TODO level
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
		new_log_output(out, stderr, LOG_FORMAT_DEFAULT, 0);
	}
	return out;
}

static inline bool str_empty(const char *str) {
	return !str || *str == '\0';
}

static const struct format *if_seek_forward(const struct format *format,
		enum log_level set_level, bool is_term, bool colors,
		bool no_log_name, bool no_msg, bool no_err) {
	bool not_empty_check = true;
	bool skip = false;
	switch (format->condition) {
		case FIFC_NON_EMPTY:
			not_empty_check = false;
			break;
		case FIFC_LEVEL:
			skip = set_level < format->if_level;
			break;
		case FIFC_TERMINAL:
			skip = !is_term;
			break;
		case FIFC_COLORED:
			skip = !colors;
	}

	if (not_empty_check) {
		if (skip == format->if_invert)
			return format;

		size_t depth = 0;
		while (format) {
			switch (format->type) {
				case FF_IF:
					depth++;
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

	size_t depth = 1;
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
			case FF_LEVEL:
			case FF_LEVEL_LOWCASE:
				empty = false; // We always have these
				break;
			case FF_STD_ERR:
				empty = no_err;
				break;
			case FF_IF:
				// Use recurse to skip to FF_IFEND if condition is satisfied
				f = if_seek_forward(f, set_level, is_term, colors, no_log_name, no_msg, no_err);
				if (f->type == FF_IF)
					depth++;
				break;
			case FF_IFEND:
				depth--;
				if (depth == 0)
					return f;
				break;
		}
		if (!empty)
			return format;
	}
	return NULL;
}

void _log(log_t log, enum log_level level,
		const char *file, size_t line, const char *func,
		const char *format, ...) {
	level = level_sanity(level);
	enum log_level set_level = log->_log ? log->_log->level : log_level_from_env();
	int stderrno = errno;
	if (level < set_level)
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

	for (size_t i = 0; i < cnt; i++) {
		struct log_output *out = outs + i;
		FILE *f = out->f;
		const struct format *format = outs[i].format;
		do {
			switch (format->type) {
				case FF_TEXT:
					fputs(format->text, f);
					break;
				case FF_MESSAGE:
					fputs(msg, f);
					break;
				case FF_NAME:
					if (log->name)
						fputs(log->name, f);
					break;
				case FF_SOURCE_FILE:
					fputs(file, f);
					break;
				case FF_SOURCE_LINE:
					fprintf(f, "%zu", line);
					break;
				case FF_SOURCE_FUNC:
					fputs(func, f);
					break;
				case FF_LEVEL:
					fputs(levels_info[level].name, f);
					break;
				case FF_LEVEL_LOWCASE: {
						const char *name = levels_info[level].name;
						while (*name != '\0')
							putc(tolower(*name++), f);
						break;
					}
				case FF_STD_ERR:
					if (stderrno)
						fputs(strerror(stderrno), f);
					break;
				case FF_IF:
					format = if_seek_forward(format, set_level, out->is_terminal,
							out->use_colors, str_empty(log->name),
							msg_size == 0, stderrno == 0);
					break;
				case FF_IFEND:
					// Everything already done in FF_IF
					break;
			}
			format = format->next;
		} while (format);
		fputc('\n', f);
	}

	errno = 0; // always end with errno zero
}
