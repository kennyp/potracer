require 'mkmf'

RbConfig::MAKEFILE_CONFIG['CC'] = ENV['CC'] if ENV['CC']

extension_name = 'potracer/potracer'

unless have_header('potracelib.h')
    raise "potracelib.h not found"
end

unless have_library('potrace', 'potrace_version')
    raise "lib potrace not found"
end

create_header
create_makefile(extension_name)
