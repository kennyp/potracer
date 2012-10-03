require 'potracer_ext/potracer'

module Potracer
  class Trace
    def trace(bitmap=nil, params=nil, &block)
      if block_given?
        self.do_trace(bitmap || @bitmap, params || @params) &block
      else
        self.do_trace(bitmap || @bitmap, params || @params)
      end
    end
  end
end
