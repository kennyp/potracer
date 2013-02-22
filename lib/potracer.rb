require_relative 'potracer/potracer'

module Potracer
  class Trace
    def trace(bitmap=nil, params=nil, &block)
      if block_given?
        self.do_trace(bitmap || @bitmap, params || @params, &block)
      else
        self.do_trace(bitmap || @bitmap, params || @params)
      end
    end

    def self.bitmap(bmp, width=nil, height=nil, map='RGB', &block)
      width ||= bmp.map {|r| r.length}.max
      height ||= bmp.length

      unless bmp.is_a? String
        bmp.map! {|r| r.fill(0, r.length, width - r.length)}
      end

      trace = self.new
      params = Potracer::Params.new
      bits = Potracer::Bitmap.new(width, height, bmp, map)

      if block_given?
        trace.trace(bits, params, &block)
      else
        trace.trace(bits, params)
      end
      trace
    end
  end
end
