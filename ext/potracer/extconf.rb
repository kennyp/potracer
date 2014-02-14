require 'mkmf'

RbConfig::MAKEFILE_CONFIG['CC'] = ENV['CC'] if ENV['CC']

LIBDIR = RbConfig::CONFIG['libdir']
INCLUDEDIR = RbConfig::CONFIG['includedir']

HEADER_DIRS = ['/opt/local/include', '/usr/local/include', INCLUDEDIR,
               '/usr/include']

LIB_DIRS = ['/opt/local/lib', '/usr/local/lib', LIBDIR, '/usr/lib']

dir_config('potrace', HEADER_DIRS, LIB_DIRS)

abort 'potracelib.h not found' unless find_header('potracelib.h')
abort 'lib potrace not found' unless find_library('potrace', 'potrace_version')

create_header
create_makefile('potracer/potracer')
