#!/usr/bin/env ruby -w
# encoding: UTF-8
require "rubygems"
require "bundler/setup"
require "rspec"
require "json"
require "rr"
require File.dirname(__FILE__) + "/../sit"
include Sit

describe "PlistPool" do
  before do
		@pool = PlistPool.new(100_000)
		@pool.capacity.should == 100_000
  end
  
	it "should be creatable" do
	end
	
	it "should not crash when using freed" do
	  begin
  	  plist = Plist.new @pool
  	  plist.free
    rescue => e
      e.message.should == "already freed"
    end
  end
  
  it "should be able to wrap regions" do
	  plist = Plist.new @pool
    plist.region.should == nil
    plist.append(PlistEntry.new(1, 2))
    plist.region.should == 0
    while plist.region == 0
      plist.append(PlistEntry.new(1, 2))
    end
    
    plist.region.should == 1
    
    while plist.region != 0
      plist.append(PlistEntry.new(1, 2))
    end  
  end
  
  it "should be able to cursor" do
	  plist = Plist.new @pool
	  100.times do |i|
      plist.append(PlistEntry.new(i, i))
    end
    
    cursor = plist.new_cursor
    cursor.entry.should be_nil
    cursor.prev!
    cursor.entry.should == PlistEntry.new(99, 99)
    
    while cursor.prev!
    end

    cursor.entry.should be_nil
  end
  
	
	it "should be able to create plists" do
	  a = Plist.new @pool
	  a.size.should == 0
	  b = Plist.new @pool
	  a.should_not == b
	  a.ptr.should_not == b.ptr
  end
  
  it "should be able to add to plists" do
	  plist = Plist.new @pool
	  plist.append(PlistEntry.new(1, 2))
	  plist.append(PlistEntry.new(2, 3))
	  plist.size.should == 2
	  plist.blocks_count.should == 1
	  plist.to_a.should == [PlistEntry.new(2, 3), PlistEntry.new(1, 2)]
	  plist.to_a.to_s.should == "[<2:3>, <1:2>]"
  end
  
  it "should create more blocks when full" do
    plist = Plist.new @pool
    ary = []
    100.times do |i|
      entry = PlistEntry.new(i, i)
  	  plist.append(entry)
  	  ary << entry
	  end
	  plist.size.should == 100
	  plist.blocks_count.should == 2
	  plist.to_a.should == ary.reverse
  end
end