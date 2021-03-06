// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2021, CZ.NIC z.s.p.o. (http://www.nic.cz/)
#define DEFLOG log_logc_config
#include <logc_config.h>
#include <logc_assert.h>
#include <stdlib.h>
#include <string.h>


static char *assemble(const config_setting_t *setting, size_t siz) {
	config_setting_t *parent = config_setting_parent(setting);
	if (parent == NULL) { // This is root of configuration
		char *res = malloc(siz);
		res[0] = '\0';
		return res;
	}

	const char *name = config_setting_name(setting);
	char *indexname = NULL;
	if (name == NULL) {
		assert(asprintf(&indexname, "[%d]", config_setting_index(setting)) > 0);
		name = indexname;
	}
	char *str = assemble(parent, siz + strlen(name) + 1);
	if (*str != '\0')
		strcat(str, ".");
	strcat(str, name);
	free(indexname);
	return str;
}

char *config_setting_path(const config_setting_t *setting) {
	if (setting == NULL || config_setting_is_root(setting))
		return strdup("");
	return assemble(setting, 0);
}

const char *config_type_name(int type) {
	switch (type) {
		case CONFIG_TYPE_NONE:
			return "none";
		case CONFIG_TYPE_GROUP:
			return "group";
		case CONFIG_TYPE_INT:
			return "int";
		case CONFIG_TYPE_INT64:
			return "int64";
		case CONFIG_TYPE_FLOAT:
			return "float";
		case CONFIG_TYPE_STRING:
			return "string";
		case CONFIG_TYPE_BOOL:
			return "bool";
		case CONFIG_TYPE_ARRAY:
			return "array";
		case CONFIG_TYPE_LIST:
			return "list";
		default:
			return "UNKNOWN";
	}
}
