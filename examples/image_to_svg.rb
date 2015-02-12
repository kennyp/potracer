require File.expand_path('../../lib/potracer', __FILE__)

fixture_path = File.expand_path('../../spec/fixtures/image_as_rgb_string.txt', __FILE__)
puts fixture_path
image_string = File.read(fixture_path)


File.open('out.svg', 'w') do |f|
  f.write Potracer::Trace.bitmap(image_string, 1886, 601).to_svg
end
