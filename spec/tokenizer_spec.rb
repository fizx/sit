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
	end
  
	it "should do whitespace" do
	  @parser = Tokenizer.new
	  @parser.extend(TestEvents)
	  @parser.consume("hello world")
	  $events.should == [
			[:term, 0, 5, 0],
		]
		@parser.end_stream
		$events.should == [
			[:term, 0, 5, 0],
			[:term, 6, 5, 1]
		]
  end
end