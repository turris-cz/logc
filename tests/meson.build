test_driver = find_program('test-driver.sh', dirs: meson.current_source_dir())


unittests_common = files(
  'unittests.c',
  'fixtures.c',
)
unittests_env = [
  'CK_TAP_LOG_FILE_NAME=/dev/stdout',
  'CK_VERBOSITY=silent',
]

check = dependency('check', version: '>=0.11')
obstack = cc.has_function('obstack_free') ? declare_dependency() : cc.find_library('obstack')


libfakesyslog = library('fakesyslog', ['fakesyslog.c'],
  dependencies: [check]
)

unittest_logc = executable('unittest-logc', unittests_common + [
    'logc.c',
    'logc_bind.c',
    'logc_asserts.c',
    'logc_formats.c',
    'logc_syslog.c',
  ],
  dependencies: [logc_dep, check, obstack],
  include_directories: includes,
  link_with: libfakesyslog,
)
test('unittest-logc', test_driver,
  args: [unittest_logc.full_path()],
  env: unittests_env,
  protocol: 'tap',
)

unittest_logc_argp = executable('unittest-logc_argp', unittests_common + [
    'logc_argp.c',
  ],
  dependencies: [logc_argp_dep, check, obstack, argp],
  include_directories: includes,
  link_with: libfakesyslog,
)
test('unittest-logc_argp', test_driver,
  args: [unittest_logc_argp.full_path()],
  env: unittests_env,
  protocol: 'tap',
)

unittest_logc_config = executable('unittest-logc_config', unittests_common + [
    'logc_config.c',
  ],
  dependencies: [logc_config_dep, check, obstack, libconfig],
  include_directories: includes,
  link_with: libfakesyslog,
)
test('unittest-logc_config', test_driver,
  args: [unittest_logc_config.full_path()],
  env: unittests_env,
  protocol: 'tap',
)
