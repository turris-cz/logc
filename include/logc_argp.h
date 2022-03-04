// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020, CZ.NIC z.s.p.o. (http://www.nic.cz/)
#ifndef _LOGC_ARGP_H_
#define _LOGC_ARGP_H_
#include <argp.h>
#include <logc.h>

/* This implements common arguments parsing for programs using LogC.
 * It provides full set of arguments that allow user fully control LogC parser.
 *
 * To use it just simply include logc_arpg_parser or logc_argp_parser_daemon as
 * child parser in your argp_parser definition and set logc_argp_log to your
 * APP_LOG instance.
 *
 * Warning: logc_argp uses internally keys from 1050 till 1100 for options without
 * short variant. Do not use these for your own options. (To be exact this is
 * about 'key' in 'struct argp_option'.)
 */

extern const struct argp logc_argp_parser;
extern const struct argp logc_argp_daemon_parser;
extern log_t logc_argp_log;

#endif
