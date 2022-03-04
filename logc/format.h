// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020, CZ.NIC z.s.p.o. (http://www.nic.cz/)
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
