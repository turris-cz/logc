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
#include "util.h"

#define ENV_LOG_ORIGIN "LOG_ORIGIN"

static bool log_origin_from_env() {
	static bool log_origin = DEF_USE_ORIGIN;
	static bool loaded = false;
	if (!loaded)
		log_origin = !str_empty(getenv(ENV_LOG_ORIGIN));
	return log_origin;
}

bool log_use_origin(log_t log) {
	if (log->_log)
		return log->_log->use_origin;
	else
		return log_origin_from_env();
}

void log_set_use_origin(log_t log, bool use) {
	log_allocate(log);
	log->_log->use_origin = use;
}
