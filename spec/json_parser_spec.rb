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
	  $bufs = []
	end
  
  it "should do json" do
    str = "{\"hello\":\"world\"}"
    @parser = Parser.new_json()
    @parser.on_document(proc{|db| $bufs << db})
    @parser.consume(str)
    $bufs.size.should == 1
    $bufs.first.terms.should == [Term.new("hello", "world", 0, false)]
  end
  
  it "should do account for newline" do
    str = "\n\n{\"hello\":\"world\"}"
    @parser = Parser.new_json()
    @parser.on_document(proc{|db| $bufs << db})
    @parser.consume(str)
    $bufs.size.should == 1
    $bufs.first.terms.should == [Term.new("hello", "world", 0, false)]
  end
    
  it "should do multiple docs" do
    str = "{\"hello\":\"world\"}\n{\"wha\":\"goodbye thanks\"}"
    @parser = Parser.new_json()
    @parser.on_document(proc{|db| $bufs << db})
    @parser.consume(str)
    $bufs.size.should == 2
    $bufs.first.terms.should == [Term.new("hello", "world", 0, false)]
    $bufs.last.terms.should == [
      Term.new("wha", "goodbye", 0, false),
      Term.new("wha", "thanks", 1, false)
    ]
  end      
end