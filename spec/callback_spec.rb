#!/usr/bin/env ruby -w
# encoding: UTF-8
require File.dirname(__FILE__) + "/test_helper"

describe "Callback" do
  
  before do
  end
  
	it "should to_s" do
		cb = Callback.new(String, proc{|a, b| $foo = a + b })
		cb.to_s.should =~ /\[Callback -?\d+\]/
	end
	
	it "should be callable" do
		cb = Callback.new(String, proc{ |doc| 
		  doc.should == nil
		})
		cb.call
	end
end