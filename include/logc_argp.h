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
