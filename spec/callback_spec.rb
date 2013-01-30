#!/usr/bin/env ruby -w
# encoding: UTF-8
require "rubygems"
require "bundler/setup"
require "rspec"
require "json"
require "rr"
require File.dirname(__FILE__) + "/../sit"
include Sit

describe "Callback" do
  before do
    @engine = Engine.new(RealAbcTsvParser.new, 1_000_000)
  end
	it "should to_s" do
		cb = Callback.new(@engine, proc{|a, b| $foo = a + b })
		cb.to_s.should =~ /\[Callback \d+\]/
	end
	
	it "should be callable" do
		cb = Callback.new(@engine, proc{|doc| 
		  doc.should be_nil
		  $foo = @engine 
		})
		cb.call
		$foo.should == @engine
	end
end