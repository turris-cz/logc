project('c-template', 'c',
  version: '0.1.0',
  license : 'GPL-3.0-or-later',
  default_options: 'c_std=c11',
)

add_project_arguments('-D_GNU_SOURCE', language: 'c')
cc = meson.get_compiler('c')


logc = dependency('logc', version: '>=0.2.0')
logc_argp = dependency('logc_argp')
logc_config = dependency('logc_config')
argp = cc.has_function('argp_parse') ? declare_dependency() : cc.find_library('argp')
libconfig = dependency('libconfig')

includes = include_directories('include')

gperf = generator(find_program('gperf'),
  output: '@PLAINNAME@.h',
  arguments: ['@EXTRA_ARGS@', '--output-file=@OUTPUT@', '@INPUT@']
)

add_project_arguments('-DPROJECT_VERSION="' + meson.project_version() + '"', language: 'c')

subdir('libfoo')
subdir('src')

test_buildtypes = ['debug', 'debugoptimized']
if get_option('tests').enabled() or (get_option('tests').auto() and get_option('buildtype') in test_buildtypes)
  subdir('tests')
endif


cppcheck = find_program('cppcheck', required: false)
if cppcheck.found()
  run_target('cppcheck', command : [cppcheck, '--project=' + join_paths(meson.build_root(), 'compile_commands.json')])
endif

flawfinder = find_program('flawfinder', required: false)
if flawfinder.found()
  run_target('flawfinder', command : [flawfinder, '--error-level=5', meson.project_source_root()])
endif
