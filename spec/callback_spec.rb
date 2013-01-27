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
	it "should to_s" do
		cb = Callback.new(4, proc{|a, b| $foo = a + b })
		cb.to_s.should =~ /\[Callback \d+\]/
	end
	
	it "should be callable" do
		cb = Callback.new(4, proc{|a, b| $foo = a + b })
		cb.call(3)
		$foo.should == 7
	end
end