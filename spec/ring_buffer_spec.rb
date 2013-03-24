#!/usr/bin/env ruby -w
# encoding: UTF-8
require "rubygems"
require "bundler/setup"
require "rspec"
require "json"
require "rr"
require File.dirname(__FILE__) + "/../rb/sit"
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
	
	it "should work with ints" do
		buf = IntRingBuffer.new(100)
		n = 20
		(0..n).each do |i|
		  buf.append(i)
	  end
    buf.get(0).should == 0
    buf.get(1).should == 1
		buf.get(18).should == 18
		
		cursor = IntRingBufferCursor.new(buf, nil, nil)
		cursor.get.should == nil
		cursor.prev!.should == true
		cursor.pos.should == n
		cursor.get.should == n
		while(cursor.prev!) do
		  cursor.get.should == n -= 1
		end
		n.should == 0
	end
	
	it "should ring with ints" do
		buf = IntRingBuffer.new(10)
		(0..20).each do |i|
		  buf.append(i)
	  end
    buf.get(0).should == nil
    buf.get(1).should == nil
		buf.get(18).should == 18
		
		cursor = IntRingBufferCursor.new(buf, nil, nil)
		cursor.get.should == nil
		cursor.prev!.should == true
		cursor.pos.should == 20
		cursor.get.should == 20
		cursor.prev!.should == true
		cursor.get.should == 19
		while(cursor.prev!) do
      $foo = cursor.get
		end
		$foo.should == 11
	end
	
	it "should ring with ints" do
		buf = IntRingBuffer.new(10)
		(0..20).each do |i|
		  buf.append(i)
	  end
    buf.get(0).should == nil
    buf.get(1).should == nil
		buf.get(18).should == 18
		
		cursor = IntRingBufferCursor.new(buf, ">", 17)
		cursor.get.should == nil
		cursor.prev!.should == true
		cursor.pos.should == 20
		cursor.get.should == 20
		cursor.prev!.should == true
		cursor.get.should == 19
		while(cursor.prev!) do
      $foo = cursor.get
		end
		$foo.should == 18
	end
end