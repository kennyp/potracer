require 'pp'

require 'rubygems'
require 'progressbar'

require_relative '../lib/potracer'

bits = STDIN.read.each_line.map do |l|
  l.each_char.map {|c| (c == ' ' || c == "\n") ? 0 : 1}
end

pbar = ProgressBar.new("trace", 100)

trace = Potracer::Trace.bitmap(bits) do |p|
  pbar.set p
end

pbar.finish

puts '<svg xmlns="http://www.w3.org/2000/svg" version="1.1">'
print "  <path fill-rule=\"evenodd\" fill=\"black\" d=\""
trace.to_a.each do |path|
  print path[:parts].join(' ').gsub('moveto', 'M').gsub('curveto', 'C').gsub('lineto', 'L')
end
puts '" />'
puts '</svg>'
