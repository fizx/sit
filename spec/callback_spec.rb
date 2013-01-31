#!/usr/bin/env ruby -w
# encoding: UTF-8
require "rubygems"
require "bundler/setup"
require "rspec"
require "json"
require "rr"
require File.dirname(__FILE__) + "/../sit"
include Sit

describe "Callback" do
  
  before do
    @string = "hello"
  end
  
	it "should to_s" do
		cb = Callback.new(@string, proc{|a, b| $foo = a + b })
		cb.to_s.should =~ /\[Callback \d+\]/
	end
	
	it "should be callable" do
		cb = Callback.new(@string, proc{ |doc| 
		  doc.should == @string
		  $foo = @string
		})
		cb.call
		$foo.should == @string
	end
end