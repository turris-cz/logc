// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021, CZ.NIC z.s.p.o. (http://www.nic.cz/)
#define DEFLOG log_logc_config
#include <logc_config.h>
#include <stdio.h>

LOG(logc_config);


void logc_config_load_config(log_t log, config_t *config) {
	logc_config_load_setting(log, config_lookup(config, "log"));
}

void logc_config_load_setting(log_t log, config_setting_t *setting) {
	if (setting == NULL) {
		debug("No log configuration provided in configuration file.");
		return;
	}
	// level
	const config_setting_t *level = config_setting_lookup(setting, "level");
	if (level && check_config_setting_type(level, CONFIG_TYPE_INT)) {
		if (log_level(log) == 0) // applied only if level wasn't modified
			log_set_level(log, config_setting_get_int(level));
	}
	// file
	const config_setting_t *file = config_setting_lookup(setting, "file");
	if (file && check_config_setting_type(file, CONFIG_TYPE_STRING)) {
		const char *strfile = config_setting_get_string(file);
		if (strfile) {
			FILE *logfile = fopen(strfile, "w+");
			if (logfile)
				log_add_output(log, logfile, LOG_F_AUTOCLOSE, 0, LOG_FORMAT_DEFAULT);
			else
				error("Unable to open log file '%s' for writing", strfile);
		}
	}
	// syslog
	const config_setting_t *ssyslog = config_setting_lookup(setting, "syslog");
	if (ssyslog && check_config_setting_type(ssyslog, CONFIG_TYPE_BOOL)) {
		bool boolsyslog = config_setting_get_bool(ssyslog);
		if (log->daemon) {
			if (log_syslog(log))
				// Note: syslog is in default enabled for daemon and thus this
				// is used only to disable if not already disabled.
				log_syslog_fallback(log, boolsyslog);
		} else {
			if (!log_syslog(log))
				// Note: syslog is disabled in default for non-daemon logs and
				// thus this is unsed only to enable if not already enabled.
				log_syslog_format(log, LOG_FORMAT_DEFAULT);
		}
	}
}
