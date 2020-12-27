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
#ifndef _LOGC_OUTPUT_H_
#define _LOGC_OUTPUT_H_
#include "log.h"
#include <logc.h>
#include "format.h"

struct output {
	FILE *f;
	int level;
	const struct format *format;
	bool free_format;
	bool use_colors;
	bool is_terminal;
	bool autoclose;
};

void new_output(struct output *out, FILE *f, int level,
		const char *format, int flags);
void free_output(struct output *out, bool close_f);

void syslog_output(struct output *out, char **str, size_t *str_len,
		const struct format *format);
void free_syslog_output(struct output *out);

const struct output *default_stderr_output();

#endif
