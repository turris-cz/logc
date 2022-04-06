// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020, CZ.NIC z.s.p.o. (http://www.nic.cz/)
#include <unistd.h>

struct fakesyslog {
	int priority;
	char *msg;
};

extern struct fakesyslog *fakesyslog;
extern size_t fakesyslog_cnt;

void fakesyslog_reset();
void fakesyslog_free();
