// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020, CZ.NIC z.s.p.o. (http://www.nic.cz/)
#include "format.h"
#include <string.h>

#include "format.gperf.h"

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

const struct format *default_format() {
	static struct format *format = NULL;
	if (format == NULL)
		format = parse_format(LOG_FORMAT_DEFAULT);
	return format;
}
