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
#include "log.h"

#ifndef _LOGC_PARSER_H_
#define _LOGC_PARSER_H_

enum format_fields {
	FF_TEXT,
	FF_MESSAGE,
	FF_NAME,
	FF_SOURCE_FILE,
	FF_SOURCE_LINE,
	FF_SOURCE_FUNC,
	FF_STD_ERR,
	FF_IF,
	FF_ELSE,
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
	enum log_message_level if_level;

	struct format *next;
};


struct format *parse_format(const char *format);
void free_format(struct format *f);

const struct format *default_format();

#endif
