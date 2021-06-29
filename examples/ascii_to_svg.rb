# Usage: cat banner.txt | ruby ascii_to_svg.rb > banner.svg
require 'rubygems'
require 'progressbar'

require File.expand_path('../../lib/potracer', __FILE__)

bits = STDIN.read.each_line.map do |l|
  l.each_char.map { |c| c == ' ' || c == "\n" ? 0 : 1 }
end

pbar = ProgressBar.create(title: 'trace', total: 100, output: $stderr)

trace = Potracer::Trace.bitmap(bits) do |p|
  pbar.progress = p
end

puts trace.to_svg
