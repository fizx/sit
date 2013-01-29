#!/usr/bin/env ruby -w
# encoding: UTF-8
require "rubygems"
require "bundler/setup"
require "rspec"
require "json"
require "rr"
require File.dirname(__FILE__) + "/../sit"
require File.dirname(__FILE__) + "/abc_tsv_parser"
include Sit

describe "Parser" do
	before do
	  $events.clear
		@parser = TestAbcTsvParser.new
		@engine = Engine.new(@parser, 10_000_000)
  end

	it "should be able to assign engine" do
		@parser.engine = @engine
	end

	it "should work standalone" do
	  @engine.terms.size.should == 0
		@engine.consume("z\ty")
		$events.should == [
			[:field, "a"], 
			[:term, 0, 1, 0], 
			[:field, "b"]
		]
		$events.clear
		@engine.consume("yyy\tx\n")
		$events.should == [
			[:term, 2, 4, 0], 
			[:field, "c"],
			[:term, 7, 1, 0],
			[:doc, 0, 9]
		]
		$events.clear
		@engine.consume("y\n\n\nx x\n")
		$events.should == [
			[:term, 9, 1, 0], 
			[:doc, 9, 2], 
			[:doc, 11, 1], 
			[:doc, 12, 1], 
			[:term, 13, 1, 0], 
			[:term, 15, 1, 1],
			[:doc, 13, 4]
		]
	end
end