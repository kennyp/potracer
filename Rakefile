require 'rubygems'
require 'rake/clean'
require 'rspec/core/rake_task'
require 'rake/extensiontask'


spec = Gem::Specification.new do |s|
  s.name        = 'potracer'
  s.version     = '0.0.0'
  s.date        = '2012-09-20'
  s.summary     = "Ruby bindings for potrace."
  s.description = "Ruby bindings for potrace."
  s.authors     = ["Kenny Parnell"]
  s.email       = 'k.parnell@gmail.com'
  s.files       = Dir.glob('lib/**/*.rb')
  s.extensions  = ['ext/potracer/extconf.rb']
  s.homepage    = 'https://github.com/kparnell/potracer'
end

Gem::PackageTask.new(spec) do |pkg|
end

Rake::ExtensionTask.new do |ext|
  ext.name = 'potracer'
  ext.ext_dir = 'ext/potracer'
  ext.lib_dir = 'lib/potracer_ext'
  ext.gem_spec = spec
end

RSpec::Core::RakeTask.new(:spec)


desc "Run specs"
task :default => :spec

task :spec => :compile
