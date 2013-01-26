#!/usr/bin/env ruby -w
# encoding: UTF-8
require "rubygems"
require "bundler/setup"
require "rspec"
require "json"
require "rr"
require File.dirname(__FILE__) + "/../sit"
include Sit

describe "Term" do
	it "should to_s" do
		pstr = Term.new("hello", "world", 7)
		pstr.to_s.should == "[hello:world 7]"
	end
end