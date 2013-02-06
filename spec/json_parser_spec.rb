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
  
  it "should do json" do
    str = "{\"hello\":\"world\"}"
    @parser = Parser.new_json()
    @parser.extend(TestEvents)
    @parser.consume(str)
    $events.should == [
      [:field, "hello"],
      [:term, str.index("w"), 5, 0],
      [:doc, 0, str.length],
    ]
  end
  
  it "should do account for newline" do
    str = "\n\n{\"hello\":\"world\"}"
    @parser = Parser.new_json()
    @parser.extend(TestEvents)
    @parser.consume(str)
    $events.should == [
      [:field, "hello"],
      [:term, str.index("w"), 5, 0],
      [:doc, 2, str.length - 2],
    ]
  end
    
  it "should do multiple docs" do
    str = "{\"hello\":\"world\"}\n{\"wha\":\"goodbye thanks\"}"
    @parser = Parser.new_json()
    @parser.extend(TestEvents)
    @parser.consume(str)
    start = str.index("{", 10)
    $events.should == [
      [:field, "hello"],
      [:term, str.index("w"), 5, 0],
      [:doc, 0, str.index("}") + 1],      
      [:field, "wha"],
      [:term, str.index("g"), 7, 0],
      [:term, str.index("t"), 6, 1],
      [:doc, start, str.length - start]
    ]
  end      
end