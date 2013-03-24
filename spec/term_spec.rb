#!/usr/bin/env ruby -w
# encoding: UTF-8
require File.dirname(__FILE__) + "/test_helper"

describe "Term" do
	it "should to_s" do
		pstr = Term.new("hello", "world", 7, false)
		pstr.to_s.should == "[hello:world 7]"
	end
	
	it "should be comparable" do
	  a = Term.new("foo", "world", 0, false)
	  b = Term.new("foo", "hello", 0, false)
	  a.should > b
	  
	  a = Term.new("a", "world", 0, false)
	  b = Term.new("foo", "hello", 0, false)
	  a.should < b
  end
end