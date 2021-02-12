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
#include "output.h"
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

void new_output_f(struct output *out, FILE *f, int level, const struct format *format, int flags) {
	*out = (typeof(*out)){
		.f = f,
		.fd = -1,
		.level = level,
		.format = format,
		.free_format = false,
		.use_colors = (flags & LOG_F_COLORS) && !(flags & LOG_F_NO_COLORS),
		.is_terminal = false,
		.autoclose = flags & LOG_F_AUTOCLOSE,
	};

	out->fd = fileno(f);
	if (out->fd != -1)
		out->is_terminal = isatty(out->fd);
	errno = 0; // annul possible fileno and isatty errors

	if (!(flags & (LOG_F_NO_COLORS | LOG_F_COLORS)))
		out->use_colors = out->is_terminal;
}

void new_output(struct output *out, FILE *f, int level, const char *format, int flags) {
	struct format *fformat = parse_format(format);
	new_output_f(out, f, level, fformat, flags);
	out->free_format = true;
}

void free_output(struct output *out, bool close_f) {
	if (!out)
		return;
	if (close_f && out->autoclose)
		fclose(out->f);
	if (out->free_format)
		free_format((struct format*)out->format);
}

void syslog_output(struct output *out, char **str, size_t *str_len,
		const struct format *format) {
	*out = (typeof(*out)){
		.f = open_memstream(str, str_len),
		.fd = -1,
		.level = 0,
		.format = format ?: default_format(),
		.use_colors = false,
		.is_terminal = false,
		.autoclose = false,
	};
}

void free_syslog_output(struct output *out) {
	if (out->format != default_format())
		free_format((struct format*)out->format);
	fclose(out->f);
}

void log_add_output(log_t log, FILE *file, int flags, int level, const char *format) {
	log_allocate(log);
	size_t index = log->_log->outs_cnt;
	for (size_t i = 0; i < log->_log->outs_cnt; i++) // Locate if already present
		if (file == log->_log->outs[i].f) {
			free_output(log->_log->outs + i, false);
			index = i;
			break;
		}

	// We do not expect huge amount of outputs so optimizing addition to array for
	// speed is less beneficial over optimizing for memory (fitting exactly)
	if (index == log->_log->outs_cnt)
		log->_log->outs = realloc(log->_log->outs,
				++log->_log->outs_cnt * sizeof(struct output));

	new_output(log->_log->outs + index, file, level, format, flags);
}

bool log_rm_output(log_t log, FILE *file) {
	log_allocate(log);
	for (size_t i = 0; i < log->_log->outs_cnt; i++) {
		struct output *out = log->_log->outs + i;
		if (out->f == file) {
			free_output(out, true);
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
		free_output(log->_log->outs + i, true);
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

const struct output *default_stderr_output() {
	static struct output *out = NULL;
	if (out && out->f != stderr) {
		free_output(out, false);
		out = NULL;
	}
	if (out == NULL) {
		out = malloc(sizeof *out);
		new_output_f(out, stderr, 0, default_format(), 0);
	}
	return out;
}


void lock_output(const struct output *out) {
	if (out->fd == -1)
		return;

	struct flock fl = {
		.l_type = F_WRLCK,
		.l_whence = SEEK_SET,
		.l_start = 0,
		.l_len = 0,
	};
	fcntl(out->fd, F_SETLKW, &fl);
	errno = 0; // ignore failure
}

void unlock_output(const struct output *out) {
	if (out->fd == -1)
		return;

	struct flock fl = {
		.l_type = F_UNLCK,
		.l_whence = SEEK_SET,
		.l_start = 0,
		.l_len = 0,
	};
	fcntl(out->fd, F_SETLKW, &fl);
	errno = 0; // ignore failure
}
