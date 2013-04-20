#!/usr/bin/env ruby -w
# encoding: UTF-8
require File.dirname(__FILE__) + "/test_helper"

describe "LrwDict" do
  it "should be creatable" do
    dict = LrwDict.new(10)
    dict.capacity.should == 10
  end
  
  it "should be put and gettable" do
    term = Term.new("hello", "world", 0, false)
    dict = LrwDict.new(10)
    dict[term] = "world"
    dict.size.should == 1
    dict[term].should == "world"
  end
  
  it "should be limited" do
    dict = LrwDict.new(10)
    20.times do |i|
      term = Term.new("hello", i.to_s, 0, false)
      dict[term] = i.to_s
    end
    Hash[*(dict.to_a.flatten)].values.sort.should == ("10".."19").to_a
  end
  
  it "should still work when spammed out of order" do
    dict = LrwDict.new(10)
    20.times do |i|
      term = Term.new("hello", i.to_s, 0, false)
      dict[term] = i.to_s
      term = Term.new("hello", (i - 1).to_s, 0, false)
      dict[term] = (i - 1).to_s
      if i % 5 == 0
        term = Term.new("hello", "a", 0, false)
        dict[term] = "a"
      end
    end
    dict.size.should == 10
    Hash[*(dict.to_a.flatten)].values.sort.should == ("11".."19").to_a + ["a"]
  end
  
end