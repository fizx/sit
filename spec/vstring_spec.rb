#!/usr/bin/env ruby -w
# encoding: UTF-8
require "rubygems"
require "bundler/setup"
require "rspec"
require "json"
require "rr"
require File.dirname(__FILE__) + "/../sit"
include Sit

describe "VString" do
  before do
    @vs = VString.new
  end
  
	it "should be creatable" do
	end
	
	it "should be appendable" do
	  @vs.append("ohai")
	  @vs.to_s.should == "ohai"
	  @vs.append(" world")
	  @vs.to_s.should == "ohai world"
	  @vs.size.should == "ohai world".length
	  @vs.get(1, 3).should == "hai"
	end
	
	it "should be freeable" do
	  @vs.append("ohai")
	  @vs.append(" world")
	  @vs.shift(7)
	  @vs.to_s.should == "ohai world"[7..-1]
	  @vs.nodes.should == [" world"]
	  @vs.size.should == 3
	  @vs.get(1, 2).should == "ld"
	  
	  @vs.get(1, 6).should == nil
  end
end