require_relative 'spec_helper'
require 'potracer'

describe Potracer::Params do
  before :each do
    @params = Potracer::Params.new
  end

  it 'should allow you to get and set the turd size' do
    @params.turd_size.should eq(2)
    @params.turd_size = 3
    @params.turd_size.should eq(3)
  end

  it 'should allow you to specify the turnpolicy' do
    @params.turn_policy.should eq(Potracer::Turnpolicy::MINORITY)
    @params.turn_policy = Potracer::Turnpolicy::BLACK
    @params.turn_policy.should eq(Potracer::Turnpolicy::BLACK)
  end

  it 'should allow you to specify the alpha_max' do
    @params.alpha_max.should eq(1.0)
    @params.alpha_max = 0
    @params.alpha_max.should eq(0.0)
  end

  it 'should allow you to control optomization' do
    @params.should be_optimized
    @params.unoptimize!
    @params.should_not be_optimized
    @params.optimize!
    @params.should be_optimized
  end

  it 'should allow you to set the tolerance' do
    @params.tolerance.should eq(0.2)
    @params.tolerance = 0
    @params.tolerance.should eq(0)
  end
end

describe Potracer::Turnpolicy do
  it 'should contain all available turn policies' do
    ps = %w(BLACK WHITE LEFT RIGHT MAJORITY MINORITY RANDOM).map(&:to_sym)
    Potracer::Turnpolicy.constants.should include(*ps)
  end
end

describe Potracer::Bitmap do
  it 'should allow you to define a new empty bitmap' do
    bmp = Potracer::Bitmap.new
    bmp.width.should eq(250)
    bmp.height.should eq(250)
  end

  it 'should let you specify height and width when being created' do
    bmp = Potracer::Bitmap.new(100, 100)
    bmp.width.should eq(100)
    bmp.height.should eq(100)
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
    bmp.to_a.should eq(orig)
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
    trace.trace(bmp, params).to_a.should_not be_empty
  end

  it 'should allow you to track progress as you trace' do
    x = 0
    bitmap = Potracer::Bitmap.new
    params = Potracer::Params.new
    Potracer::Trace.new.trace(bitmap, params) { |p| x = p }
    x.should eq(100)
  end

  it 'should be able to handle non-homogeneous bitmaps' do
    a = [
      [1, 1, 1, 1, 1],
      [1, 1, 1],
      [1, 1, 1, 1],
      [1]
    ]
    Potracer::Trace.bitmap(a).to_a.should_not be_empty
  end

  it 'should be able to process string representations' do
    Potracer::Trace.bitmap(image_string, 1886, 601).to_a.should_not be_empty
  end

  it 'should be able to generate svgs' do
    svg = Potracer::Trace.bitmap(image_string, 1886, 601).to_svg
    svg.should start_with('<svg')
  end

  it 'should be able to use a callback durring tracing' do
    count = 0
    Potracer::Trace.bitmap(image_string, 1886, 601) do |prog|
      count = prog
    end
    count.should eq(100)
  end
end
