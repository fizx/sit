#!/usr/bin/env ruby -w
# encoding: UTF-8
require "rubygems"
require "bundler/setup"
require "rspec"
require "json"
require "rr"
require File.dirname(__FILE__) + "/../sit"
include Sit

describe "RingBuffer" do
	it "should escape string" do
	  Sit.json_escape("foo\nbar\\").should == "foo\\nbar\\\\"
	end
end