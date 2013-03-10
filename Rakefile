require 'date'

require 'rubygems'
require 'rake/clean'
require 'rspec/core/rake_task'
require 'rake/extensiontask'


spec = Gem::Specification.new do |s|
  s.name        = 'potracer'
  s.version     = '1.0.9'
  s.date        = Date.today.strftime('%Y-%m-%d')
  s.summary     = "Ruby bindings for potrace."
  s.description = "Ruby bindings for the potrace library."
  s.authors     = ["Kenny Parnell"]
  s.email       = 'k.parnell@gmail.com'
  s.platform    = Gem::Platform::RUBY
  s.files       = FileList['lib/*.rb'] + FileList['ext/**/*.{h,c,rb}']
  s.extensions  = FileList['ext/**/extconf.rb']
  s.homepage    = 'https://github.com/kparnell/potracer'
end

Gem::PackageTask.new(spec) do |pkg|
end

Rake::ExtensionTask.new('potracer', spec) do |ext|
  ext.lib_dir = 'build'
end
end

RSpec::Core::RakeTask.new(:spec)


desc "Run specs"
task :default => :spec

task :spec => :compile
