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

describe "Tokenizer" do
	before do
	  $events = []
	end
  
	it "should do whitespace" do
	  @parser = Parser.new_whitespace
	  @parser.receiver = TestEvents.new
	  @parser.consume("hello world")
	  $events.should == [
			[:term, "hello", 0],
		]
		@parser.end_stream
		$events.should == [
			[:term, "hello", 0],
			[:term, "world", 1]
		]
  end
end

describe "Parser" do
  
  before do
    $events = []
    @parser = AbcTsvParser.new
    @parser.receiver = TestEvents.new
  end
  
  it "should work standalone" do
    @parser.consume("z\ty")
    $events.should == [
      [:field, "a"], 
      [:term, "z", 0], 
      [:field, "b"]
    ]
    $events.clear
    @parser.consume("yyy\tx\n")
    $events.should == [
      [:term, "yyyy", 0], 
      [:field, "c"],
      [:term, "x", 0],
      [:field, "columns"],
      [:int, 3],
      [:doc, "z\tyyyy\tx\n"]
    ]
    $events.clear
    @parser.consume("y\n\n\nx x\n")
    $events.should == [
      [:term, "y", 0],
      [:field, "columns"],
      [:int, 1],
      [:doc, "y\n"],
      [:field, "columns"],
      [:int, 1],
      [:doc, "\n"],
      [:field, "columns"],
      [:int, 1],
      [:doc, "\n"],
      [:term, "x", 0],
      [:term, "x", 1],
      [:field, "columns"],
      [:int, 1],
      [:doc, "x x\n"]
    ]
  end
end