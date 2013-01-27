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
		cb = Callback.new("data", proc{})
		q = Query.new([term], cb)
		q.to_s.should == "[[#{term}], #{cb}]"
	end

	it "should be sorted"
end