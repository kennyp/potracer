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
    ps = %w(BLACK WHITE LEFT RIGHT MAJORITY MINORITY RANDOM).map {|p| p.to_sym}
    Potracer::Turnpolicy.constants.should include(*ps)
  end
end
