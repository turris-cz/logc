unittests_common = files(
  'unittests.c',
  'fixtures.c',
)
unittests_env = [
  'CK_TAP_LOG_FILE_NAME=/dev/stdout',
  'CK_VERBOSITY=silent',
  'TEST_FILES=' + test_files,
]

check = dependency('check', version: '>=0.11')
obstack = cc.has_function('obstack_free') ? declare_dependency() : cc.find_library('obstack')


unittest_libfoo = executable('unittest-libfoo', unittests_common + [
    'count.c',
  ],
  dependencies: [libfoo_dep, check, obstack],
  include_directories: includes,
)
test('unittest-libfoo', test_driver,
  args: [unittest_libfoo.full_path()],
  env: unittests_env,
  protocol: 'tap',
)


unittest_foo = executable('unittest-foo', unittests_common + foo_sources + [
    'config.c'
  ],
  dependencies: foo_dependencies + [check, obstack],
  c_args: foo_c_args + [
    '-iquote', join_paths(meson.project_source_root(), 'src'),
  ],
)
test('unittest-foo', test_driver,
  args: [unittest_foo.full_path()],
  env: unittests_env,
  protocol: 'tap',
)
