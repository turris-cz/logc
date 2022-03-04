// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021, CZ.NIC z.s.p.o. (http://www.nic.cz/)
#include <logc_config.h>
#include <stdlib.h>
#include <string.h>

bool log_check_config_setting_type(log_t log, const config_setting_t *setting, int type) {
	int stype = config_setting_type(setting);
	bool correct_type = stype == type;
	if (!correct_type) {
		char *path = config_setting_path(setting);
		log_error(log, "The configuration has unexpected type, expected '%s' but was '%s': %s",
			config_type_name(type), config_type_name(stype), path);
		free(path);
	}
	return correct_type;
}
