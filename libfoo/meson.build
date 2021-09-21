libfoo_sources = [
  files(
    'log.c',
    'count.c',
  ),
  gperf.process('match.gperf'),
]

libfoo = library('foo', libfoo_sources,
  version: '0.0.0',
  dependencies: [logc],
  include_directories: includes,
  link_args: '-Wl,--version-script=' + join_paths(meson.current_source_dir(), 'libfoo.version'),
  install: true
)


pkg_mod = import('pkgconfig')
pkg_mod.generate(libfoo,
  description: 'Library counting foos..'
)
