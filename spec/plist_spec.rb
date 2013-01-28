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
  before do
		@pool = PlistPool.new(10_000_000)
		@pool.capacity.should == 10_000_000
  end
  
	it "should be creatable" do
	end
	
	it "should be able to create plists" do
	  a = Plist.new @pool
	  a.size.should == 0
	  b = Plist.new @pool
	  a.should_not == b
  end
  
  it "should be able to add to plists" do
	  plist = Plist.new @pool
	  plist.append(PlistEntry.new(1, 2))
	  plist.append(PlistEntry.new(2, 3))
	  plist.size.should == 2
	  
	  plist.to_a.should == [PlistEntry.new(2, 3), PlistEntry.new(1, 2)]
	  plist.to_a.to_s.should == "[<2:3>, <1:2>]"
  end
end