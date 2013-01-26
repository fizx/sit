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
	it "should accept content and forget old content" do
		buf = RingBuffer.new(20)
		buf.append("hello world")
		buf.get(0, 11).should == "hello world"
		buf.append("goodbye world")
		buf.get(11, 13).should == "goodbye world"
		buf.get(0, 11).should be_nil
	end
end