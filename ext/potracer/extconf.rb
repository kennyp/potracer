require 'mkmf'

RbConfig::MAKEFILE_CONFIG['CC'] = ENV['CC'] if ENV['CC']

extension_name = 'potracer/potracer'

raise "potracelib.h not found" unless have_header('potracelib.h')
raise "lib potrace not found" unless have_library('potrace', 'potrace_version')

create_header
create_makefile(extension_name)
