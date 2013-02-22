require_relative './lib/potracer'

fixtures = File.join(File.dirname(__FILE__), 'spec', 'fixtures')
image_string = File.read(File.join(fixtures, 'image_as_rgb_string.txt'))

File.open('out.svg', 'w') do |f|
  f.write Potracer::Trace.bitmap(image_string, 1886, 601).to_svg
end
