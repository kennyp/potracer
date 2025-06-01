begin
  require_relative '../build/potracer'
rescue LoadError
  require_relative '../ext/potracer/potracer'
end

module Potracer
  ##
  # This class represents a trace of a Potracer::Bitmap

  class Trace
    ##
    # Trace the given +bitmap+
    #
    # ==== Attributes
    #
    # * +bitmap+ - an instance of Potracer::Bitmap. If not given the +@bitmap+
    #   is used.
    # * +params+ - an instance of Potracer::Params. If not given +@params+ is
    #   used.
    # * +block+ - optional block called to report trace progress

    def trace(bitmap = nil, params = nil, &block)
      if block_given?
        do_trace(bitmap || @bitmap, params || @params, &block)
      else
        do_trace(bitmap || @bitmap, params || @params)
      end
    end

    ##
    # Trace a bitmap
    #
    # ==== Attributes
    #
    # * +bmp+ - mapped to a Potracer::Bitmap either a multi-dimensional array
    #   of bits or a string of image data
    # * +width+ - width of the image to be mapped if +bmp+ is a string
    # * +height+ - height of the image to be mapped if +bmp+ is a string
    # * +map+ - pixel data format if +bmp+ is a string
    # * +block+ - optional block called to report trace progress
    #
    # ==== Example
    #
    #   require 'ruby-progressbar'
    #
    #   pbar = ProgressBar.create(title: 'Tracing')
    #   Potracer::Trace.bitmap(bits) do |percent|
    #     pbar.progress = percent
    #   end
    #   pbar.finish

    def self.bitmap(bmp, width = nil, height = nil, map = 'RGB', &block)
      trace = new
      params = Potracer::Params.new
      bits = make_bits(bmp, width, height, map)

      if block_given?
        trace.trace(bits, params, &block)
      else
        trace.trace(bits, params)
      end
      trace
    end

    private

    def self.make_bits(bitmap, width, height, map)
      if bitmap.is_a?(Array) && !bitmap.is_a?(String) # Check it's an array of arrays
        derived_height = bitmap.length
        derived_width = bitmap.empty? ? 0 : bitmap.map { |r| r.length }.max

        width ||= derived_width
        height ||= derived_height

        # Pad rows to ensure all have the same length (width)
        # This should only happen if we have a valid width (greater than 0)
        if width && width > 0
          bitmap.map! { |r| r.fill(0, r.length, width - r.length) }
        end
      end
      # For string inputs, or if width/height were not determined from an array,
      # they must be explicitly passed or Potracer::Bitmap.new must handle nil width/height if that's valid.
      # Given the C error, width & height must be integers for Potracer::Bitmap.new.

      Potracer::Bitmap.new(width, height, bitmap, map)
    end
  end
end
