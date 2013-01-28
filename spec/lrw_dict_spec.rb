#!/usr/bin/env ruby -w
# encoding: UTF-8
require "rubygems"
require "bundler/setup"
require "rspec"
require "json"
require "rr"
require File.dirname(__FILE__) + "/../sit"
include Sit

describe "LrwDict" do
	it "should be creatable" do
		dict = LrwDict.new(10)
		dict.capacity.should == 10
	end
	
	it "should be put and gettable" do
		a = "hello"
		b = "world"
		dict = LrwDict.new(10)
		dict[a] = b
		dict.size.should == 1
		dict[a].should == b
	end
	
end