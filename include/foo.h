// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021, CZ.NIC z.s.p.o. (http://www.nic.cz/)
#ifndef _FOO_H_
#define _FOO_H_
#include <stdio.h>
#include <logc.h>

// LogC handle for this library
extern log_t log_foo;

// Count number of foo lines in data read from provided file.
// f: file object open for reading
// Returns number of "foo:" prefix lines found in file.
unsigned count_foo(FILE *f) __attribute__((nonnull));

#endif
