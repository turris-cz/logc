// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright 2020-2022, CZ.NIC z.s.p.o. (http://www.nic.cz/)
#define SUITE "logc"
#define DEFAULT_SETUP testconfig_setup
#define DEFAULT_TEARDOWN testconfig_teardown
#include "unittests.h"
#include <logc_config.h>


static config_t testconfig;
static void testconfig_setup() {
	basic_setup();
	log_bind(tlog, log_logc_config);
	config_init(&testconfig);
}

static const char *testconf =
	"log: {\n"
	"	level = 2;\n"
	"	file = \"/dev/null\";\n"
	"	syslog = true\n"
	"}\n"
	"negativelevel: { level = -2; }\n"
	"empty: { }\n"
	"foo: { fee: { faa: 0 } }\n"
;
static void testconfig_testconf_setup() {
	testconfig_setup();
	ck_assert_int_eq(config_read_string(&testconfig, testconf), CONFIG_TRUE);
}

static void testconfig_teardown() {
	config_destroy(&testconfig);
	basic_teardown();
}

TEST_CASE(testconf, testconfig_testconf_setup) {}

// TODO we can't verify that file was configured right now thanks to limited
// API. We should extend API in the future to allow that and add this to tests
// here.

TEST(testconf, testconf_config) {
	logc_config_load(tlog, &testconfig);
	ck_assert_int_eq(log_level(tlog), 2);
	ck_assert(log_syslog(tlog));
}
END_TEST

TEST(testconf, testconf_setting) {
	logc_config_load(tlog, config_lookup(&testconfig, "log"));
	ck_assert_int_eq(log_level(tlog), 2);
	ck_assert(log_syslog(tlog));
}
END_TEST

TEST(testconf, testconf_negativelevel) {
	logc_config_load(tlog, config_lookup(&testconfig, "negativelevel"));
	ck_assert_int_eq(log_level(tlog), -2);
	ck_assert(!log_syslog(tlog));
}
END_TEST

TEST(testconf, testconf_empty) {
	logc_config_load(tlog, config_lookup(&testconfig, "empty"));
	ck_assert_int_eq(log_level(tlog), 0);
	ck_assert(!log_syslog(tlog));
}
END_TEST

TEST(testconf, testconf_null) {
	logc_config_load(tlog, (config_setting_t*)NULL);
	ck_assert_int_eq(log_level(tlog), 0);
	ck_assert(!log_syslog(tlog));
}
END_TEST


TEST_CASE(invalid) {}

static const struct {
	const char *conf;
	const char *error;
} setting_invalid_data[] = {
	// Note: we pass root setting to loading thus no group in is required in
	// configurations in here.
	{
		.conf = "level = 4.4;",
		.error = "ERROR:logc_config: The configuration has unexpected type, expected 'int' but was 'float': level\n"
	},
	{
		.conf = "file = 0;",
		.error = "ERROR:logc_config: The configuration has unexpected type, expected 'string' but was 'int': file\n"
	},
	{
		.conf = "syslog = 0;",
		.error = "ERROR:logc_config: The configuration has unexpected type, expected 'bool' but was 'int': syslog\n"
	},
};

ARRAY_TEST(invalid, setting_invalid, setting_invalid_data) {
	ck_assert_int_eq(config_read_string(&testconfig, _d.conf), CONFIG_TRUE);
	logc_config_load(tlog, config_root_setting(&testconfig));
	ck_assert_str_eq(stderr_data, _d.error);
}
END_TEST


TEST_CASE(path, testconfig_testconf_setup) {}

static const char *setting_paths_data[] = {
	"log",
	"log.level",
	"log.file",
	"negativelevel.level",
	"empty",
	"foo.fee.faa",
};
ARRAY_TEST(path, setting_paths, setting_paths_data) {
	char *path = config_setting_path(config_lookup(&testconfig, _d));
	ck_assert_str_eq(path, _d);
	free(path);
}
END_TEST

TEST(path, setting_path_root) {
	char *path = config_setting_path(config_root_setting(&testconfig));
	ck_assert_str_eq(path, "");
	free(path);
}
END_TEST

TEST(path, setting_path_null) {
	char *path = config_setting_path(NULL);
	ck_assert_str_eq(path, "");
	free(path);
}
END_TEST


TEST_CASE(type) {}

static const struct {
	const char *conf;
	const char *path;
	int valid_type;
	int invalid_type;
	const char *error;
} log_setting_type_data[] = {
	{
		.conf = "level = 4.4;",
		.path = "level",
		.valid_type = CONFIG_TYPE_FLOAT,
		.invalid_type = CONFIG_TYPE_INT,
		.error = "ERROR:tlog: The configuration has unexpected type, expected 'int' but was 'float': level\n"
	},
	{
		.conf = "foo: { level = 4.4; };",
		.path = "foo.level",
		.valid_type = CONFIG_TYPE_FLOAT,
		.invalid_type = CONFIG_TYPE_INT,
		.error = "ERROR:tlog: The configuration has unexpected type, expected 'int' but was 'float': foo.level\n"
	},
	{
		.conf = "foo: [ \"fee\", \"faa\" ];",
		.path = "foo.[0]",
		.valid_type = CONFIG_TYPE_STRING,
		.invalid_type = CONFIG_TYPE_BOOL,
		.error = "ERROR:tlog: The configuration has unexpected type, expected 'bool' but was 'string': foo.[0]\n"
	},
};

ARRAY_TEST(type, log_setting_type_valid, log_setting_type_data) {
	ck_assert_int_eq(config_read_string(&testconfig, _d.conf), CONFIG_TRUE);
	ck_assert(check_config_setting_type(
			config_lookup(&testconfig, _d.path),
			_d.valid_type
		));
	ck_assert_int_eq(stderr_len, 0);
}
END_TEST

ARRAY_TEST(type, log_setting_type_invalid, log_setting_type_data) {
	ck_assert_int_eq(config_read_string(&testconfig, _d.conf), CONFIG_TRUE);
	ck_assert(!check_config_setting_type(
			config_lookup(&testconfig, _d.path), _d.invalid_type
		));
	ck_assert_str_eq(stderr_data, _d.error);
}
END_TEST
