Gem::Specification.new do |s|
  s.name        = 'potracer'
  s.version     = '0.0.0'
  s.date        = '2012-09-20'
  s.summary     = "Ruby bindings for potrace."
  s.description = "Ruby bindings for potrace."
  s.authors     = ["Kenny Parnell"]
  s.email       = 'k.parnell@gmail.com'
  s.files       = Dir.glob('lib/**/*.rb') + Dir.glob('ext/**/*.{c,h,rb}')
  s.extensions  = ['ext/potracer/extconf.rb']
  s.homepage    = 'https://github.com/kparnell/potracer'
end
