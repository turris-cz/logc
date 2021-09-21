foo_sources = files(
  'config.c',
  'log.c',
)
foo_c_args = [
  '-DDEFAULT_CONFIG_FILE="' + get_option('defconfig') + '"',
]
foo_dependencies = [
  logc, logc_argp, logc_config, argp, libconfig
]

foo = executable('foo', foo_sources + ['foo.c'],
  dependencies: foo_dependencies,
  c_args: foo_c_args,
  include_directories: includes,
  link_with: libfoo,
  install: true
)
