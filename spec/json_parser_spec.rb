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
	  $events = []
	  @te = TestEvents.new
	end
  
  it "should do json" do
    str = "{\"hello\":\"world\"}"
    @parser = Parser.new_json()
    @parser.receiver = @te
    @parser.consume(str)
    $events.should == [
      [:field, "hello"],
      [:term, "world", 0],
      [:doc, 0, str.length],
    ]
  end
  
  it "should do account for newline" do
    str = "\n\n{\"hello\":\"world\"}"
    @parser = Parser.new_json()
    @parser.receiver = @te
    @parser.consume(str)
    $events.should == [
      [:field, "hello"],
      [:term, "world", 0],
      [:doc, 2, str.length - 2],
    ]
  end
    
  it "should do multiple docs" do
    str = "{\"hello\":\"world\"}\n{\"wha\":\"goodbye thanks\"}"
    @parser = Parser.new_json()
    @parser.receiver = @te
    @parser.consume(str)
    start = str.index("{", 10)
    $events.should == [
      [:field, "hello"],
      [:term, "world", 0],
      [:doc, 0, str.index("}") + 1],      
      [:field, "wha"],
      [:term, "goodbye", 0],
      [:term, "thanks", 1],
      [:doc, start, str.length - start]
    ]
  end      
end