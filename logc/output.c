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
#include <errno.h>

void new_log_output(struct log_output *out, FILE *f, int level, const char *format, int flags) {
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

void free_log_output(struct log_output *out, bool close_f) {
	if (!out)
		return;
	if (close_f && out->autoclose)
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

struct log_output *default_stderr_output() {
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
