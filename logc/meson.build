liblogc_sources = [
  files(
    'bind.c',
    'format.c',
    'level.c',
    'log.c',
    'origin.c',
    'output.c',
    'syslog.c',
  ),
  gperf.process('format.gperf'),
]

liblogc = library('logc', liblogc_sources,
  version: '0.0.0',
  include_directories: includes,
  link_args: '-Wl,--version-script=' + join_paths(meson.current_source_dir(), 'liblogc.version'),
  install: true
)
install_headers(liblogc_headers)


pkg_mod = import('pkgconfig')
pkg_mod.generate(liblogc,
  description: 'logc is a logging library'
)
