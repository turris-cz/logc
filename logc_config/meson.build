liblogc_config_sources = [
  files(
    'logc_config.c',
    'setting_path.c',
    'setting_type.c',
  ),
]

liblogc_config = library('logc_config', liblogc_config_sources,
  version: '0.0.0',
  dependencies: [libconfig],
  link_with: [liblogc],
  include_directories: includes,
  link_args: '-Wl,--version-script=' + join_paths(meson.current_source_dir(), 'liblogc_config.version'),
  install: true
)
install_headers(liblogc_config_headers)


pkg_mod = import('pkgconfig')
pkg_mod.generate(liblogc_config,
  description: 'libconfig extension for logc logging library'
)
