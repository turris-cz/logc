/* Copyright (c) 2020-2021 CZ.NIC z.s.p.o. (http://www.nic.cz/)
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
#ifndef _LOGC_H_
#define _LOGC_H_
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

enum log_message_level {
	LL_TRACE = -3,
	LL_DEBUG = -2,
	LL_INFO = -1,
	LL_NOTICE = 0,
	LL_WARNING = 1,
	LL_ERROR = 2,
	LL_CRITICAL = 3,
};

struct _log;
struct log {
	const char *name;
	bool daemon;
	struct _log *_log;
};
typedef struct log* log_t;

#define APP_LOG(logname) \
	struct log _log_ ## logname = (struct log){.name = NULL, ._log = NULL, .daemon = false }; \
	log_t log_ ## logname = &_log_ ## logname;

#define DAEMON_LOG(logname) \
	struct log _log_ ## logname = (struct log){.name = NULL, ._log = NULL, .daemon = true }; \
	log_t log_ ## logname = &_log_ ## logname;

#define LOG(logname) \
	struct log _log_ ## logname = (struct log){.name = #logname, ._log = NULL, .daemon = false }; \
	log_t log_ ## logname = &_log_ ## logname;

void log_free(log_t) __attribute__((nonnull));

//// Verbosity settings //////////////////////////////////////////////////////////
int log_level(log_t) __attribute__((nonnull));
void log_set_level(log_t, int level) __attribute__((nonnull));
void log_verbose(log_t) __attribute__((nonnull));
void log_quiet(log_t) __attribute__((nonnull));
void log_offset_level(log_t, int offset) __attribute__((nonnull));

// Check verbosity level to check if output would be used for given level.
// This should be used when message requires significant processing before logc is
// called.
// Returns true if message would be outputed and false if not.
bool log_would_log(log_t, enum log_message_level);

//// Additional options //////////////////////////////////////////////////////////
// Setting to specify that origin of log should be used. That includes name of
// source file, source line and name of function. In default origin is not used.
bool log_use_origin(log_t) __attribute__((nonnull));
void log_set_use_origin(log_t, bool) __attribute__((nonnull));

//// Standard format pieces free to reuse ////////////////////////////////////////
// Color based on level of message. Conditioned to be used only when colors should
// be used. This is intended to distinguish different message levels by colors.
// TRACE and DEBUG levels have light gray color
// INFO level is has plain white color (no coloring)
// NOTICE level has bright white color
// WARNING level has bright yellow color
// ERROR level has bright red color
// CRITICAL level has red color
#define LOG_FP_COLOR \
	"%(P%(C\033[31m%|%(E\033[31;1m%|%(W\033[33;1m%|%(N\033[37;1m%|%(i\033[37m%)%)%)%)%)%)"
#define LOG_FP_COLOR_CLEAR "%(P\033[0m%)"

// Level name
#define LOG_FP_LEVEL_NAME \
	"%(CCRITICAL%|%(EERROR%|%(WWARNING%|%(NNOTICE%|%(IINFO%|%(DDEBUG%|TRACE%)%)%)%)%)%)"

// This standard prefix format for origin of message. It provides name of log and
// origin of message in code (if of course that is enabled and provided).
#define LOG_FP_ORIGIN "%(_%n%(_(%f:%i,%c)%):%)"

//// Some standard output formats ////////////////////////////////////////////////
#define LOG_FORMAT_PLAIN "%(_%n: %)%m%(_: %e%)"
#define LOG_FORMAT_DEFAULT (LOG_FP_COLOR "%(_%(p" LOG_FP_LEVEL_NAME ":%)" LOG_FP_ORIGIN " %)%m%(_: %e%)" LOG_FP_COLOR_CLEAR)
#define LOG_FORMAT_FULL (LOG_FP_COLOR LOG_FP_LEVEL_NAME ":" LOG_FP_ORIGIN " %m%(_: %e%)" LOG_FP_COLOR_CLEAR)

//// Output to FILE //////////////////////////////////////////////////////////////
// Output no colors even when output is detected as capable terminal
#define LOG_F_NO_COLORS (1 << 1)
// Output colors even if output is not detected as capable terminal
// (LOG_F_NO_COLORS has precedence over CELOG_F_COLORS if both are specified)
#define LOG_F_COLORS (1 << 2)
// Automatically close passed FILE when log_rm_output is called
#define LOG_F_AUTOCLOSE (1 << 4)

// Add output stream to log with specified output format.
// Flags is ored set of LOG_F_* flags or zero.
// Log level can be used to further limit output besides primary verbosity
// Format is string with following optional fields:
//   %m:  Formatted log message itself
//   %n:  Log name
//   %f:  Source file of message
//   %i:  Source line of message (in source file)
//   %c:  Function message is raised from
//   %e:  Standard error message (empty if errno == 0)
//   %(_:  Start of not-empty condition. Following text till the end of condition
//        is printed only if at least one '%*' field in it is not empty.
//   %(C: Start of critical level of message condition.
//   %(c: Start of less than critical level of message condition.
//   %(E: Start of error or higher level of message condition.
//   %(e: Start of less than error level of message condition.
//   %(W: Start of warning or higher level of message condition.
//   %(w: Start of less than warning level of message condition.
//   %(N: Start of notice or higher level of message condition.
//   %(n: Start of less than notice level of message condition.
//   %(I: Start of info or higher level of message confition.
//   %(i: Start of less than info level of message condition.
//   %(D: Start of debug or higher level of message condition.
//   %(d: Start of less than debug level of message condition.
//   %(t: Start of not terminal output condition. Text in condition is printed
//         only if output is not to terminal.
//   %(T: Start of terminal output condition. Text in condition is printed only if
//        output is to terminal.
//   %(p: Start of not colored output condition. Text in condition is printed only
//        if colors are not used.
//   %(P: Start of colored output condition. Text in condition is printed only if 
//        colors are used in output.
//   %|:  Else in condition block
//   %):  End of condition block
//   %%:  Plain %
//  Single FILE can be added only once. If same FILE object is provided multiple
//  times then it replaces original.
void log_add_output(log_t, FILE*, int flags, int level, const char *format)
	__attribute__((nonnull));

// Remove provided FILE from registered outputs of log. Note that this won't
// ever trigger fclose (LOG_F_AUTOCLOSE does not apply here).
// Returns true if output was successfully removed or false if it wasn't found.
bool log_rm_output(log_t, FILE*) __attribute__((nonnull));

// Remove all outputs from log (fallback to stderr)
void log_wipe_outputs(log_t) __attribute__((nonnull));

// Set if stderr fallback output should be used or not. In default it is
// enabled. Fallback is used if no other output is configured.
void log_stderr_fallback(log_t, bool enabled) __attribute__((nonnull));

// Flush all outputs
// This should be always called when your program is about to exit to ensure that
// all logs were correctly delivered to logs (syslog is the only exception).
void log_flush(log_t) __attribute__((nonnull));

//// Output to syslog/////////////////////////////////////////////////////////////

// Check if syslog is enabled or not.
bool log_syslog(log_t) __attribute__((nonnull));

// Set format used to output messages to syslog.
// To disable in case of not daemon log or revert to default in case of daemon
// log you can pass NULL as format.
void log_syslog_format(log_t, const char *format) __attribute((nonnull(1)));

// Set if syslog fallback output should be used or not. In default it is
// enabled and syslog is used if log is marked as daemon log.
// Fallback is used if no other output is configured.
void log_syslog_fallback(log_t, bool enabled) __attribute__((nonnull));


//// Binding /////////////////////////////////////////////////////////////////////
// This binds one log to the other. Binded log can be used as usual but it outputs
// only trough dominant one. This means that it ignores its own outputs and uses
// output of most dominant log in the chain.
// The binded log ignores its own outputs but it uses its level. Level is added to
// the level of the dominant log. This way it is possible to decrease verbosity of
// client libraries while allowing users to seem them when high enough verbosity
// is used.	It is not in general good idea to use huge numbers. It is also
// beneficial to never hide criticals so you should never shift priority more than
// LL_CRITICAL.
// The common usage for this is to join multiple logs from libraries with
// application log.
void log_bind(log_t dominant, log_t submissive) __attribute__((nonnull(2)));

// Provides access to current log's dominator. It returns either NULL when log is
// not binded or pointer to dominant log.
log_t log_bound(log_t) __attribute__((nonnull));

// Reverse process of log_bind. This releases submissive log from dominant one.
void log_unbind(log_t) __attribute__((nonnull));


//// Log function and helper macros //////////////////////////////////////////////
void _logc(log_t, enum log_message_level,
		const char *file, size_t line, const char *func,
		const char *format, ...) __attribute__((nonnull,format(printf, 6, 7)));

#define logc(logt, level, ...) _logc(logt, level, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define log_critical(logt, ...) do { logc(logt, LL_CRITICAL, __VA_ARGS__); log_flush(logt); abort(); } while (0)
#define log_fatal(logt, exit_code, ...) do { logc(logt, LL_CRITICAL, __VA_ARGS__); exit(exit_code); } while (0)
#define log_error(logt, ...) logc(logt, LL_ERROR, __VA_ARGS__)
#define log_warning(logt, ...) logc(logt, LL_WARNING, __VA_ARGS__)
#define log_notice(logt, ...) logc(logt, LL_NOTICE, __VA_ARGS__)
#define log_info(logt, ...) logc(logt, LL_INFO, __VA_ARGS__)
#define log_debug(logt, ...) logc(logt, LL_DEBUG, __VA_ARGS__)
#define log_trace(logt, ...) logc(logt, LL_TRACE, __VA_ARGS__)

#endif


#ifdef DEFLOG
#ifndef _LOGC_H_DEFLOG
#define _LOGC_H_DEFLOG

#define critical(...) log_critical(DEFLOG, __VA_ARGS__)
#define fatal(...) log_fatal(DEFLOG, __VA_ARGS__)
#define error(...) log_error(DEFLOG, __VA_ARGS__)
#define warning(...) log_warning(DEFLOG, __VA_ARGS__)
#define notice(...) log_notice(DEFLOG, __VA_ARGS__)
#define info(...) log_info(DEFLOG, __VA_ARGS__)
#define debug(...) log_debug(DEFLOG, __VA_ARGS__)
#define trace(...) log_trace(DEFLOG, __VA_ARGS__)

#endif
#endif
