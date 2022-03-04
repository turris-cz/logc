// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021, CZ.NIC z.s.p.o. (http://www.nic.cz/)
#ifndef _LOGC_CONFIG_H_
#define _LOGC_CONFIG_H_
#include <libconfig.h>
#include <logc.h>

/* This implements common configuration for programs using LogC.
 */
extern log_t log_logc_config;

// Load LogC configuration for given log from configuraton.
// This looks automatically for section "log".
void logc_config_load_config(log_t log, config_t *config)
	__attribute__((nonnull(2)));

// Load LogC configuration for given log from given setting.
void logc_config_load_setting(log_t log, config_setting_t *setting);

// Generic macro for loading configuration regardless the source.
#define logc_config_load(log, config) _Generic((config), \
		config_t*: logc_config_load_config, \
		config_setting_t*: logc_config_load_setting \
	)(log, config)


// Assemble full path to the provided setting.
// This should be rather part of libconfig but there seems to be no C API that
// provides this while there is for C++.
// Returns malloc alocated null terminated string with path. It is caller's
// responsibilty to free provided memory.
char *config_setting_path(const config_setting_t *setting)
	__attribute__((malloc,returns_nonnull));

// Provides human readable name of config type.
// Returns statically allocated string (do not free it).
const char *config_type_name(int type)
	__attribute__((returns_nonnull));

// Helper to verify type of the configuration option in a consistent way.
bool log_check_config_setting_type(log_t log, const config_setting_t *setting, int type);

#endif

#ifdef DEFLOG
#ifndef _LOGC_CONFIG_H_DEFLOG
#define _LOGC_CONFIG_H_DEFLOG

#define check_config_setting_type(...) \
		log_check_config_setting_type(DEFLOG, __VA_ARGS__)

#endif
#endif
