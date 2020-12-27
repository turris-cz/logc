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
#include "format.h"
#include <string.h>

#include "format.gperf.c"

struct format *parse_format(const char *format) {
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

void free_format(struct format *f) {
	while (f) {
		struct format *tmp = f;
		f = f->next;
		free(tmp->text);
		free(tmp);
	}
}

void log_syslog_format(log_t log, const char *format) {
	log_allocate(log);
	if (log->_log->syslog_format)
		free_format(log->_log->syslog_format);
	log->_log->syslog_format = parse_format(format);
}

const struct format *default_format() {
	static struct format *format = NULL;
	if (format == NULL)
		format = parse_format(LOG_FORMAT_DEFAULT);
	return format;
}
