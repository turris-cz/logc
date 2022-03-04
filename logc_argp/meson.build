liblogc_argp_sources = [
  files(
    'logc_argp.c',
  ),
]

liblogc_argp = library('logc_argp', liblogc_argp_sources,
  version: '0.0.0',
  dependencies: [argp],
  link_with: liblogc,
  include_directories: includes,
  link_args: '-Wl,--version-script=' + join_paths(meson.current_source_dir(), 'liblogc_argp.version'),
  install: true
)
install_headers(liblogc_argp_headers)


pkg_mod = import('pkgconfig')
pkg_mod.generate(liblogc_argp,
  description: 'argp extension for logc logging library'
)
