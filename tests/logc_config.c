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
#define DEFLOG tlog
#include <check.h>
#include <logc_config.h>
#include "logc_fixtures.h"

void unittests_add_suite(Suite*);


static config_t testconfig;
static void testconfig_setup() {
	setup();
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
	teardown();
}

// TODO we can't verify that file was configured right now thanks to limited
// API. We should extend API in the future to allow that and add this to tests
// here.

START_TEST(testconf_config) {
	logc_config_load(tlog, &testconfig);
	ck_assert_int_eq(log_level(tlog), 2);
	ck_assert(log_syslog(tlog));
}
END_TEST

START_TEST(testconf_setting) {
	logc_config_load(tlog, config_lookup(&testconfig, "log"));
	ck_assert_int_eq(log_level(tlog), 2);
	ck_assert(log_syslog(tlog));
}
END_TEST

START_TEST(testconf_negativelevel) {
	logc_config_load(tlog, config_lookup(&testconfig, "negativelevel"));
	ck_assert_int_eq(log_level(tlog), -2);
	ck_assert(!log_syslog(tlog));
}
END_TEST

START_TEST(testconf_empty) {
	logc_config_load(tlog, config_lookup(&testconfig, "empty"));
	ck_assert_int_eq(log_level(tlog), 0);
	ck_assert(!log_syslog(tlog));
}
END_TEST

START_TEST(testconf_null) {
	logc_config_load(tlog, (config_setting_t*)NULL);
	ck_assert_int_eq(log_level(tlog), 0);
	ck_assert(!log_syslog(tlog));
}
END_TEST


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

START_TEST(setting_invalid) {
	ck_assert_int_eq(config_read_string(&testconfig, setting_invalid_data[_i].conf), CONFIG_TRUE);
	logc_config_load(tlog, config_root_setting(&testconfig));
	ck_assert_str_eq(stderr_data, setting_invalid_data[_i].error);
}
END_TEST


static const char *setting_paths_data[] = {
	"log",
	"log.level",
	"log.file",
	"negativelevel.level",
	"empty",
	"foo.fee.faa",
};
START_TEST(setting_paths) {
	char *path = config_setting_path(config_lookup(&testconfig, setting_paths_data[_i]));
	ck_assert_str_eq(path, setting_paths_data[_i]);
	free(path);
}
END_TEST

START_TEST(setting_path_root) {
	char *path = config_setting_path(config_root_setting(&testconfig));
	ck_assert_str_eq(path, "");
	free(path);
}
END_TEST

START_TEST(setting_path_null) {
	char *path = config_setting_path(NULL);
	ck_assert_str_eq(path, "");
	free(path);
}
END_TEST


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

START_TEST(log_setting_type_valid) {
	ck_assert_int_eq(config_read_string(&testconfig, log_setting_type_data[_i].conf), CONFIG_TRUE);
	ck_assert(check_config_setting_type(
			config_lookup(&testconfig, log_setting_type_data[_i].path),
			log_setting_type_data[_i].valid_type
		));
	ck_assert_int_eq(stderr_len, 0);
}
END_TEST

START_TEST(log_setting_type_invalid) {
	ck_assert_int_eq(config_read_string(&testconfig, log_setting_type_data[_i].conf), CONFIG_TRUE);
	ck_assert(!check_config_setting_type(
			config_lookup(&testconfig, log_setting_type_data[_i].path),
			log_setting_type_data[_i].invalid_type
		));
	ck_assert_str_eq(stderr_data, log_setting_type_data[_i].error);
}
END_TEST



__attribute__((constructor))
static void suite() {
	Suite *suite = suite_create("logc_config");

	TCase *tctestconf = tcase_create("testconf");
	tcase_add_checked_fixture(tctestconf, testconfig_testconf_setup, testconfig_teardown);
	tcase_add_test(tctestconf, testconf_config);
	tcase_add_test(tctestconf, testconf_setting);
	tcase_add_test(tctestconf, testconf_negativelevel);
	tcase_add_test(tctestconf, testconf_empty);
	tcase_add_test(tctestconf, testconf_null);
	suite_add_tcase(suite, tctestconf);

	TCase *tcinvalid = tcase_create("invalid");
	tcase_add_checked_fixture(tcinvalid, testconfig_setup, testconfig_teardown);
	tcase_add_loop_test(tcinvalid, setting_invalid,
			0, sizeof(setting_invalid_data) / sizeof(*setting_invalid_data));
	suite_add_tcase(suite, tcinvalid);

	TCase *tcpath = tcase_create("config_setting_path");
	tcase_add_checked_fixture(tcpath, testconfig_testconf_setup, testconfig_teardown);
	tcase_add_loop_test(tcpath, setting_paths,
			0, sizeof(setting_paths_data) / sizeof(*setting_paths_data));
	tcase_add_test(tcpath, setting_path_root);
	tcase_add_test(tcpath, setting_path_null);
	suite_add_tcase(suite, tcpath);

	TCase *tctype = tcase_create("log_setting_type");
	tcase_add_checked_fixture(tctype, testconfig_setup, testconfig_teardown);
	tcase_add_loop_test(tctype, log_setting_type_valid,
			0, sizeof(log_setting_type_data) / sizeof(*log_setting_type_data));
	tcase_add_loop_test(tctype, log_setting_type_invalid,
			0, sizeof(log_setting_type_data) / sizeof(*log_setting_type_data));
	suite_add_tcase(suite, tctype);

	unittests_add_suite(suite);
}
