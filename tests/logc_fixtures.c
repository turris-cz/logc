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
#include <check.h>
#include <errno.h>
#include "logc_fixtures.h"

FILE* orig_stderr;
char *stderr_data;
size_t stderr_len;
log_t tlog;

void setup() {
	errno = 0; // Set to 0 to reset any previous possible error
	orig_stderr = stderr;
	stderr = open_memstream(&stderr_data, &stderr_len);
	tlog = malloc(sizeof *tlog);
	*tlog = (struct log) { .name = "tlog" };
}

void teardown() {
	ck_assert_int_eq(errno, 0);
	log_free(tlog);
	free(tlog);
	fclose(stderr);
	stderr = orig_stderr;
}
