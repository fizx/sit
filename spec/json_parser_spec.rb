#!/usr/bin/env ruby -w
# encoding: UTF-8
require "rubygems"
require "bundler/setup"
require "rspec"
require "json"
require "rr"
require File.dirname(__FILE__) + "/../sit"
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
      [:doc, str],
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
      [:doc, "{\"hello\":\"world\"}"],
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
      [:doc, "{\"hello\":\"world\"}"],      
      [:field, "wha"],
      [:term, "goodbye", 0],
      [:term, "thanks", 1],
      [:doc, "{\"wha\":\"goodbye thanks\"}"]
    ]
  end      
end