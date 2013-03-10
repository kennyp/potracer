# Usage: cat banner.txt | ruby ascii_to_svg.rb > banner.svg
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

puts trace.to_svg
