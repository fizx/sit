#!/usr/bin/env ruby -w
# encoding: UTF-8
require "rubygems"
require "bundler/setup"
require "rspec"
require "json"
require "rr"
require File.dirname(__FILE__) + "/../sit"
include Sit

describe "Query" do
  it "should be to_s" do
    term = Term.new("hello", "world", 0)
    cb = Callback.new(String, proc{})
    q = Query.new([term], cb)
    q.to_s.should =~ /<query \[hello:world 0\]\[cb:\d+\]>/
  end

	it "should be sorted" do
		a = Term.new("hello", "world", 0)
		b = Term.new("goodbye", "world", 0)
		cb = Callback.new(String, proc{})
		qa = Query.new([a, b], cb)
		qb = Query.new([b, a], cb)
		qa.to_s.should == qb.to_s
    qa.to_s.should =~ /<query \[goodbye:world 0\] \[hello:world 0\]\[cb:\d+\]>/
	end
end