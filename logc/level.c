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
#define DEFLOG log_logc_internal
#include "log.h"

#define ENV_LOG_LEVEL_VAR "LOG_LEVEL"

static int log_level_from_env() {
	static int level = 0;
	static bool loaded = false;
	if (!loaded) {
		char *envlog = getenv(ENV_LOG_LEVEL_VAR);
		// atoi returns 0 on error and that is our default
		level = envlog ? atoi(envlog) : 0;
		loaded = true;
	}
	return level;
}

bool verbose_filter(enum log_message_level msg_level, log_t log, struct log_output *out) {
	return msg_level - 
		(log->_log ? log->_log->level : 0) -
		(out ? out->level : 0) -
		log_level_from_env()
		>= 0;
}

int log_level(log_t log) {
	log_allocate(log);
	return log->_log->level;
}

void log_set_level(log_t log, int level) {
	log_allocate(log);
	log->_log->level = level;
}

void log_verbose(log_t log) {
	log_allocate(log);
	printf("pre level %d\n", log->_log->level);
	log->_log->level--;
	printf("level %d\n", log->_log->level);
}

void log_quiet(log_t log) {
	log_allocate(log);
	log->_log->level++;
}

void log_offset_level(log_t log, int offset) {
	log_allocate(log);
	log->_log->level += offset;
}
