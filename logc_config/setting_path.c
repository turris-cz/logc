/* Copyright (c) 2021 CZ.NIC z.s.p.o. (http://www.nic.cz/)
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
