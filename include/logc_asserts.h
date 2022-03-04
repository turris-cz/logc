// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021, CZ.NIC z.s.p.o. (http://www.nic.cz/)
#ifndef _LOGC_ASSERTS_H_
#define _LOGC_ASSERTS_H_
#include <logc_assert.h>
#include <stdbool.h>
#include <string.h>
#include <tgmath.h>

#define __logc_concat(a, b) __logc_concat_(a, b)
#define __logc_concat_(a, b) a ## b
#define __logc_quote(str) #str
#define __logc_equote(str) __logc_quote(str)


#define __log_assert_cmp_op_eq ==
#define __log_assert_cmp_op_ne !=
#define __log_assert_cmp_op_lt <
#define __log_assert_cmp_op_le <=
#define __log_assert_cmp_op_gt >
#define __log_assert_cmp_op_ge >=

#define __cmp_num(cmp) \
	static inline bool __log_assert_cmp_num_##cmp(long long a, long long b) { \
		return a __logc_concat(__log_assert_cmp_op_, cmp) b; \
	}
#define __cmp_float(cmp) \
	static inline bool __log_assert_cmp_float_##cmp(long double a, long double b) { \
		return a __logc_concat(__log_assert_cmp_op_, cmp) b; \
	}
#define __cmp_string(cmp) \
	static inline bool __log_assert_cmp_string_##cmp(const char *a, const char *b) { \
		return strcmp(a, b) __logc_concat(__log_assert_cmp_op_, cmp) 0; \
	}
#define __cmp_stringl(cmp) \
	static inline bool __log_assert_cmp_stringl_##cmp(const char *a, const char *b, size_t len) { \
		return strncmp(a, b, len) __logc_concat(__log_assert_cmp_op_, cmp) 0; \
	}
#define __cmp_mem(cmp) \
	static inline bool __log_assert_cmp_mem_##cmp(const void *a, const void *b, size_t len) { \
		return memcmp(a, b, len) __logc_concat(__log_assert_cmp_op_, cmp) 0; \
	}
static inline bool __log_assert_cmp_floatl_eq(long double a, long double b, long double prec) {
	return fabs(a - b) < prec;
}
static inline bool __log_assert_cmp_floatl_ne(long double a, long double b, long double prec) {
	return fabs(a - b) >= prec;
}
static inline bool __log_assert_cmp_floatl_gt(long double a, long double b, long double prec) {
	return (a - b) > -prec;
}
static inline bool __log_assert_cmp_floatl_lt(long double a, long double b, long double prec) {
	return (b - a) > -prec;
}
// Note: We do not implement le and ge variant for float precision as with
// precision the le and lt as well as ge and gt are same.
#define __cmp_functions__(prototype) \
	prototype(eq); \
	prototype(ne); \
	prototype(lt); \
	prototype(le); \
	prototype(gt); \
	prototype(ge);
__cmp_functions__(__cmp_num);
__cmp_functions__(__cmp_float);
__cmp_functions__(__cmp_string);
__cmp_functions__(__cmp_stringl);
__cmp_functions__(__cmp_mem);
#undef __cmp_functions__
#undef __cmp_mem
#undef __cmp_stringl
#undef __cmp_string
#undef __cmp_float
#undef __cmp_num

// Note: integer types are handled by default case
#define __log_assert_cmp_compare(cond, a, b) _Generic((a), \
	float: __log_assert_cmp_float_##cond, \
	double: __log_assert_cmp_float_##cond, \
	long double: __log_assert_cmp_float_##cond, \
	char*: __log_assert_cmp_string_##cond, \
	const char*: __log_assert_cmp_string_##cond, \
	default: __log_assert_cmp_num_##cond)(a, b)
#define __log_assert_cmp_compare_l(cond, a, b, len) _Generic((a), \
	char*: __log_assert_cmp_stringl_##cond, \
	const char*: __log_assert_cmp_stringl_##cond, \
	float: __log_assert_cmp_floatl_##cond, \
	double: __log_assert_cmp_floatl_##cond, \
	long double: __log_assert_cmp_floatl_##cond, \
	default: __log_assert_cmp_mem_##cond)(a, b, len)


#define __log_assert_cmp_type(x) _Generic((x), \
	short: "%hd", unsigned short: "%hu", \
	int: "%d", unsigned: "%u", \
	long: "%ld", unsigned long: "%lu", \
	long long: "%lld", unsigned long long: "%llu", \
	float: "%E", double: "%lE", long double: "%LE", \
	char: "%c", \
	char*: "'%s'", const char*: "'%s'", \
	default: "%p")
#define __log_assert_cmp_type_l(x, l) _Generic((x), \
	float: "%E", double: "%lE", long double: "%LE", \
	char*: "'%.*s'", const char*: "'%.*s'", \
	default: "%p")

// Expands to boolean that controls if l parameter should be passed to printf
#define __log_assert_cmp_arg_l(x) _Generic((x), \
	char*: true, const char*: true, \
	default: false)


#define __log_assert_format(a, b, c, d) \
		const char *__str_a = (a); \
		const char *__str_b = (b); \
		const char *__str_c = (c); \
		const char *__str_d = (d); \
		char __format[strlen(__str_a) + strlen(__str_b) + strlen(__str_c) + strlen(__str_d) + 1]; \
		strcpy(__format, __str_a); \
		strcat(__format, __str_b); \
		strcat(__format, __str_c); \
		strcat(__format, __str_d);


#define _log_assert_cmp_(log, cond, a, b) ({ \
		__auto_type _cmp_a = (a); __auto_type _cmp_b = (b); \
		if (__log_assert_cmp_compare(cond, _cmp_a, _cmp_b)); else { \
			__log_assert_format( \
				"Assertion '%s "__logc_equote(__logc_concat(__log_assert_cmp_op_, cond))" %s' failed: ", \
				__log_assert_cmp_type(a), \
				" "__logc_equote(__logc_concat(__log_assert_cmp_op_, cond))" ", \
				__log_assert_cmp_type(b) \
			) \
			log_critical(log, __format, #a, #b, _cmp_a, _cmp_b); \
		} \
	})
#define _log_assert_cmp_l(log, cond, a, b, l) ({ \
		__auto_type _cmp_a = (a); __auto_type _cmp_b = (b); \
		__auto_type _cmp_l = (l); \
		if (__log_assert_cmp_compare_l(cond, _cmp_a, _cmp_b, _cmp_l)); else { \
			__log_assert_format( \
				"Assertion '%s "__logc_equote(__logc_concat(__log_assert_cmp_op_, cond))" %s' failed: ", \
				__log_assert_cmp_type_l(a, l), \
				" "__logc_equote(__logc_concat(__log_assert_cmp_op_, cond))" ", \
				__log_assert_cmp_type_l(b, l) \
			) \
			if (__log_assert_cmp_arg_l(a)) \
				log_critical(log, __format, #a, #b, (int)_cmp_l, _cmp_a, (int)_cmp_l, _cmp_b); \
			else \
				log_critical(log, __format, #a, #b, _cmp_a, _cmp_b); \
		} \
	})
#define __logc_assert_cmp_select(_1, _2, _3, _4, _5, X, ...) X
#define _log_assert_cmp(...) \
	__logc_assert_cmp_select(__VA_ARGS__, _log_assert_cmp_l, _log_assert_cmp_)(__VA_ARGS__)

#define log_assert_eq(log, ...) _log_assert_cmp(log, eq, __VA_ARGS__)
#define log_assert_ne(log, ...) _log_assert_cmp(log, ne, __VA_ARGS__)
#define log_assert_lt(log, ...) _log_assert_cmp(log, lt, __VA_ARGS__)
#define log_assert_le(log, ...) _log_assert_cmp(log, le, __VA_ARGS__)
#define log_assert_gt(log, ...) _log_assert_cmp(log, gt, __VA_ARGS__)
#define log_assert_ge(log, ...) _log_assert_cmp(log, ge, __VA_ARGS__)

#endif

#ifdef DEFLOG
#ifndef _LOGC_ASSERTS_H_DEFLOG
#define _LOGC_ASSERTS_H_DEFLOG

#define assert_cmp(...) log_assert_cmp(DEFLOG, __VA_ARGS__)
#define assert_eq(...) log_assert_eq(DEFLOG, __VA_ARGS__)
#define assert_ne(...) log_assert_ne(DEFLOG, __VA_ARGS__)
#define assert_lt(...) log_assert_lt(DEFLOG, __VA_ARGS__)
#define assert_le(...) log_assert_le(DEFLOG, __VA_ARGS__)
#define assert_gt(...) log_assert_gt(DEFLOG, __VA_ARGS__)
#define assert_ge(...) log_assert_ge(DEFLOG, __VA_ARGS__)

#endif
#endif
