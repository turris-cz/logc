bats_tests = [
  'foo',
]


bats = find_program('bats')

foreach bats_test : bats_tests
  test('runtest-' + bats_test, test_driver,
    args: ['-n', bats.full_path(), '--tap', join_paths(meson.current_source_dir(), bats_test + '.bats')],
    env: ['TEST_FOO=' + foo.full_path()],
    protocol: 'tap',
  )
endforeach
