#!/usr/bin/env ruby -w
# encoding: UTF-8
require File.dirname(__FILE__) + "/test_helper"

describe "RingBuffer" do
	it "should escape string" do
	  Sit.json_escape("foo\nbar\\").should == "foo\\nbar\\\\"
	end
end