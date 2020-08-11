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
#ifndef _LOGC_H_
#define _LOGC_H_
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

enum log_level {
	LL_TRACE,
	LL_DEBUG,
	LL_INFO,
	LL_NOTICE,
	LL_WARNING,
	LL_ERROR,
	LL_CRITICAL,
};

struct _log;
struct log {
	const char *name;
	struct _log *_log;
};
typedef struct log* log_t;

#define APP_LOG(NAME) \
	struct log _log_ ## NAME = (struct log){.name = NULL, ._log = NULL }; \
	log_t log_ ## NAME = &_log_ ## NAME;

#define LOG(NAME) \
	struct log _log_ ## NAME = (struct log){.name = #NAME, ._log = NULL }; \
	log_t log_ ## NAME = &_log_ ## NAME;

void log_free(log_t) __attribute__((nonnull));

//// Verbosity settings //////////////////////////////////////////////////////////
enum log_level log_level(log_t) __attribute__((nonnull));
void log_set_level(log_t, enum log_level) __attribute__((nonnull));
void log_verbose(log_t) __attribute__((nonnull));
void log_quiet(log_t) __attribute__((nonnull));

// Check verbosity level to check if output would be used for given level.
// This should be used when message requires significant processing before logc is
// called.
// Returns true if message would be outputed and false if not.
bool log_would_log(log_t, enum log_level);

//// Terminal color format definition ////////////////////////////////////////////
#define LOG_FORMAT_COLOR "%(P%(C\033[31;1m%)%(c%(E\033[31m%)%)%(e%(W\033[35m%)%)%(w%(I\033[34m%)%)%(i%(D\033[37;1m%)%)%(d\033[37m%)%)"
#define LOG_FORMAT_COLOR_CLEAR "%(P\033[0m%)"

//// Some standard output formats ////////////////////////////////////////////////
#define LOG_FORMAT_PLAIN "%(_%n: %)%m%(_: %e%)"
#define LOG_FORMAT_DEFAULT (LOG_FORMAT_COLOR "%(_%(p%L:%)%(_%n%(D:%)%)%(d(%f:%i,%c):%) %)%m%(_: %e%)" LOG_FORMAT_COLOR_CLEAR)
#define LOG_FORMAT_SOURCE (LOG_FORMAT_COLOR "%(p%L:%)%n(%f:%i,%c): %m%(_: %e%)" LOG_FORMAT_COLOR_CLEAR)
#define LOG_FORMAT_FULL (LOG_FORMAT_COLOR "%L:%n(%f:%i,%c): %m%(_: %e%)" LOG_FORMAT_COLOR_CLEAR)

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
//   %l:  Lowercase string representation of message level
//   %L:  Uppercase string representation of message level
//   %e:  Standard error message (empty if errno == 0)
//   %(_:  Start of not-empty condition. Following text till the end of condition
//        is printed only if at least one field in it is not empty. It ignores
//        any constant text and conditions are not considered as field.
//   %(c: Start of less than critical level condition.
//   %(C: Start of critical level condition.
//   %(e: Start of less than error level condition.
//   %(E: Start of error or higher level condition.
//   %(w: Start of less than warning level condition.
//   %(W: Start of warning or higher level condition.
//   %(n: Start of less than notice level condition.
//   %(N: Start of notice or higher level condition.
//   %(i: Start of less than info level condition.
//   %(I: Start of info or higher level confition.
//   %(d: Start of less than debug level condition.
//   %(D: Start of debug or higher level condition.
//   %(t: Start of not terminal output condition. Text in condition is printed
//         only if output is not to terminal.
//   %(T: Start of terminal output condition. Text in condition is printed only if
//        output is to terminal.
//   %(p: Start of not colored output condition. Text in condition is printed only
//        if colors are not used.
//   %(P: Start of colored output condition. Text in condition is printed only if 
//        colors are used in output.
//   %):  End of condition block
//   %%:  Plain %
void log_add_output(log_t, FILE*, int flags, enum log_level, const char *format)
	__attribute__((nonnull));

// Remove provided FILE from registered outputs of log. Note that this won't
// ever trigger fclose (LOG_F_AUTOCLOSE does not apply here).
// Returns true if output was successfully removed or false if it wasn't found.
bool log_rm_output(log_t, FILE*) __attribute__((nonnull));

// Remove all outputs from log (fallback to stderr)
void log_wipe_outputs(log_t) __attribute__((nonnull));

// Set if stderr fallback output should be used or not. In default it is enabled.
// Fallback is used if no other output is configured.
void log_stderr_fallback(log_t, bool enabled) __attribute__((nonnull));

//// Output to Syslog ////////////////////////////////////////////////////////////
// Set if logs should be send to syslog
void log_syslog_enable(log_t) __attribute__((nonnull));
void log_syslog_enablef(log_t, const char *format) __attribute__((nonnull));
// Disable 
void log_syslog_disable(log_t) __attribute__((nonnull));
// Check if logs are passed to syslog
bool log_syslog_enabled(log_t) __attribute__((nonnull));


//// Chaining ////////////////////////////////////////////////////////////////////
// This chains slave log to master one. This makes it so all configuration set to
// master is also applied to slave. Note that this effectively makes all settings
// same with exception of log slave name. This is handy when various logs are used
// across application such as for example in external library.
void log_chain(log_t master, log_t slave) __attribute__((nonnull));
// Reverse process of log_chain. This splits slave from master but slave is
// still going to keep all of its 
void log_unchain(log_t master, log_t slave) __attribute__((nonnull));


//// Log function and helper macros //////////////////////////////////////////////
void _log(log_t, enum log_level,
		const char *file, size_t line, const char *func,
		const char *format, ...) __attribute__((nonnull,format(printf, 6, 7)));

#define log(LOG, LEVEL, ...) _log(LOG, LEVEL, __FILE__, __LINE__, __func__, __VA_ARGS__)
#define critical(LOG, ...) do { log(LOG, LL_CRITICAL, __VA_ARGS__); abort(); } while (0)
#define error(LOG, ...) log(LOG, LL_ERROR, __VA_ARGS__)
#define warning(LOG, ...) log(LOG, LL_WARNING, __VA_ARGS__)
#define notice(LOG, ...) log(LOG, LL_NOTICE, __VA_ARGS__)
#define info(LOG, ...) log(LOG, LL_INFO, __VA_ARGS__)
#define debug(LOG, ...) log(LOG, LL_DEBUG, __VA_ARGS__)
#define trace(LOG, ...) log(LOG, LL_TRACE, __VA_ARGS__)

#endif


#ifndef _LOGC_H_DEFLOG
#define _LOGC_H_DEFLOG
#ifdef DEFLOG

#define CRITICAL(...) critical(DEFLOG, __VA_ARGS__)
#define ERROR(...) error(DEFLOG, __VA_ARGS__)
#define WARNING(...) warning(DEFLOG, __VA_ARGS__)
#define NOTICE(...) notice(DEFLOG, __VA_ARGS__)
#define INFO(...) info(DEFLOG, __VA_ARGS__)
#define DEBUG(...) debug(DEFLOG, __VA_ARGS__)
#define TRACE(...) trace(DEFLOG, __VA_ARGS__)

#endif
#endif
