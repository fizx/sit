#!/usr/bin/env ruby -w
# encoding: UTF-8
require "rubygems"
require "bundler/setup"
require "rspec"
require "json"
require "rr"
require File.dirname(__FILE__) + "/../sit"
include Sit

describe "PlistPool" do
	it "should be creatable" do
		pool = PlistPool.new(10_000_000)
		pool.capacity.should == 10_000_000
	end
end