#!/usr/bin/env ruby -w
# encoding: UTF-8
require File.dirname(__FILE__) + "/test_helper"

describe "LrwDict" do
	it "should be creatable" do
		dict = LrwDict.new(10)
		dict.capacity.should == 10
	end
	
	it "should be put and gettable" do
		dict = LrwDict.new(10)
		dict["hello"] = "world"
		dict.size.should == 1
		dict["hello"].should == "world"
	end
	
	it "should be limited" do
		dict = LrwDict.new(10)
		20.times do |i|
			dict[i.to_s] = i.to_s
		end
		Hash[*(dict.to_a.flatten)].keys.sort.should == ("10".."19").to_a
	end
	
	it "should still work when spammed out of order" do
		dict = LrwDict.new(10)
		20.times do |i|
			dict[(i-1).to_s] = (i-1).to_s
			dict[i.to_s] = i.to_s
			if i % 5 == 0
				dict["a"] = "b"
			end
		end
		Hash[*(dict.to_a.flatten)].keys.sort.should == ("11".."19").to_a + ["a"]
		dict.size.should == 10
	end
	
end