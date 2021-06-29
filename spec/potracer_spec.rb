require_relative 'spec_helper'
require File.expand_path('../../lib/potracer', __FILE__)

describe Potracer::Params do
  before :each do
    @params = Potracer::Params.new
  end

  it 'should allow you to get and set the turd size' do
    _(@params.turd_size).must_equal 2
    @params.turd_size = 3
    _(@params.turd_size).must_equal 3
  end

  it 'should allow you to specify the turnpolicy' do
    _(@params.turn_policy).must_equal Potracer::Turnpolicy::MINORITY
    @params.turn_policy = Potracer::Turnpolicy::BLACK
    _(@params.turn_policy).must_equal Potracer::Turnpolicy::BLACK
  end

  it 'should allow you to specify the alpha_max' do
    _(@params.alpha_max).must_equal 1.0
    @params.alpha_max = 0
    _(@params.alpha_max).must_equal 0.0
  end

  it 'should allow you to control optomization' do
    _(@params).must_be :optimized?
    @params.unoptimize!
    _(@params).wont_be :optimized?
    @params.optimize!
    _(@params).must_be :optimized?
  end

  it 'should allow you to set the tolerance' do
    _(@params.tolerance).must_equal 0.2
    @params.tolerance = 0
    _(@params.tolerance).must_equal 0
  end
end

describe Potracer::Turnpolicy do
  it 'should contain all available turn policies' do
    ps = %w(BLACK WHITE LEFT RIGHT MAJORITY MINORITY RANDOM).map(&:to_sym)
    ps.all? {|p| _(Potracer::Turnpolicy.constants).must_include p}
  end
end

describe Potracer::Bitmap do
  it 'should allow you to define a new empty bitmap' do
    bmp = Potracer::Bitmap.new
    _(bmp.width).must_equal 250
    _(bmp.height).must_equal 250
  end

  it 'should let you specify height and width when being created' do
    bmp = Potracer::Bitmap.new(100, 100)
    _(bmp.width).must_equal 100
    _(bmp.height).must_equal 100
  end

  it 'should let you provide the bitmap when being created' do
    orig = [
      [1, 1, 1, 1, 1],
      [1, 0, 0, 0, 1],
      [1, 0, 0, 0, 1],
      [1, 0, 0, 0, 1],
      [1, 1, 1, 1, 1]
    ]
    bmp = Potracer::Bitmap.new(5, 5, orig)
    _(bmp.to_a).must_equal orig
  end
end

describe Potracer::Trace do
  let(:fixtures) { File.join(File.dirname(__FILE__), 'fixtures') }
  let(:image_string) do
    File.read(File.join(fixtures, 'image_as_rgb_string.txt'))
  end

  it 'should allow you to trace a bitmap' do
    bmp = Potracer::Bitmap.new(5, 5, [
      [1, 1, 1, 1, 1],
      [1, 0, 0, 0, 1],
      [1, 0, 0, 0, 1],
      [1, 0, 0, 0, 1],
      [1, 1, 1, 1, 1]
    ])
    params = Potracer::Params.new
    trace = Potracer::Trace.new
    _(trace.trace(bmp, params).to_a).wont_be :empty?
  end

  it 'should allow you to track progress as you trace' do
    x = 0
    bitmap = Potracer::Bitmap.new
    params = Potracer::Params.new
    Potracer::Trace.new.trace(bitmap, params) { |p| x = p }
    _(x).must_equal 100
  end

  it 'should be able to handle non-homogeneous bitmaps' do
    a = [
      [1, 1, 1, 1, 1],
      [1, 1, 1],
      [1, 1, 1, 1],
      [1]
    ]
    _(Potracer::Trace.bitmap(a).to_a).wont_be_empty
  end

  it 'should be able to process string representations' do
    _(Potracer::Trace.bitmap(image_string, 1886, 601).to_a).wont_be_empty
  end

  it 'should be able to generate svgs' do
    svg = Potracer::Trace.bitmap(image_string, 1886, 601).to_svg
    _(svg).must_match(/^<svg/)
  end

  it 'should be able to use a callback durring tracing' do
    count = 0
    Potracer::Trace.bitmap(image_string, 1886, 601) do |prog|
      count = prog
    end
    _(count).must_equal 100
  end
end
