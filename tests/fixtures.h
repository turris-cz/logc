// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021, CZ.NIC z.s.p.o. (http://www.nic.cz/)
#ifndef _FIXTURE_H_
#define _FIXTURE_H_
#include <stdlib.h>
#include <stdio.h>
#include <obstack.h>
#define obstack_chunk_alloc malloc
#define obstack_chunk_free free

#ifndef DEFLOG
#define DEFLOG tlog
#endif
#include <logc.h>


// Commonly used stderr capture
extern FILE* orig_stderr;
extern char *stderr_data;
extern size_t stderr_len;

// The test log that is automatically initialized and freed
extern log_t tlog;

// obstack that is automatically freed on test teardown
extern struct obstack t_obstack;


// Basic fixture
// * checks errno at teardown
// * setups obstack that can be used in tests
// * configures stderr capture
// * initializes common log instance tlog
void basic_setup();
void basic_teardown();


// Open the scope for t_obstack. You should assign result to void* variable and
// pass it to obscope_end later on to end the scope.
#define obscope_start obstack_alloc(&t_obstack, 0)
// Closes the close previouslu opened with obstcope_start. Note that obscope
// implements stack and thus it also closes all scopes opened after after the
// one being closed.
#define obscope_end(SCOPE) obstack_free(&t_obstack, (SCOPE))
// Wrapper for function call that automatically frees all obscope allocations.
#define obscope(FUNC) do { \
		void *_obstack_scope_guard_ = obscope_start; \
		FUNC; \
		obscope_end(_obstack_scope_guard_); \
	} while(false);

#endif
