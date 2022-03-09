project('logc', 'c',
  version: '0.4.0',
  license : 'MIT',
  default_options: 'c_std=c11',
)

add_project_arguments('-D_GNU_SOURCE', language: 'c')
cc = meson.get_compiler('c')

gperf = generator(find_program('gperf'),
  output: '@PLAINNAME@.h',
  arguments: ['@EXTRA_ARGS@', '--output-file=@OUTPUT@', '@INPUT@']
)


subdir('include')
subdir('logc')

meson.override_dependency(meson.project_name(), logc_dep)

argp = cc.has_function('argp_parse') ? declare_dependency() : cc.find_library('argp', required: get_option('libargp'))
if argp.found()
  subdir('logc_argp')
endif

libconfig = dependency('libconfig', required: get_option('libconfig'))
if libconfig.found()
  subdir('logc_config')
endif


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
