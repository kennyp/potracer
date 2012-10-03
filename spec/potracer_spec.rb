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
end
