#!/usr/bin/env ruby -w
# encoding: UTF-8
require "rubygems"
require "bundler/setup"
require "rspec"
require "json"
require "rr"
require File.dirname(__FILE__) + "/../sit"
include Sit

describe "integration" do
  before do
    @engine = Engine.new(Parser.new_json, 1_000_000)
 		@input = Input.new(@engine, 1 << 10, 1 << 20)
    @proto = LineProtocol.new(@input)
  end
  
	Dir[File.dirname(__FILE__) + "/proto/*.in"].each do |path|
		it "should run #{path} successfully" do
			out = path.sub(".in", ".out")
			File.exists?(out).should == true
			@proto.consume(File.read(path))
			expected = File.readlines(out)
			actual = @proto.output
      actual.zip(expected).each do |a,e|
        a.should == e.chomp
      end
      expected.size.should == actual.size
		end
	end
end