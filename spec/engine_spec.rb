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

describe "Engine" do
  before do
    @engine = Engine.new(RealAbcTsvParser.new, 1_000_000)
    $events = []
  end
  
  it "should link the parser" do
    @engine.parser.engine.should == @engine
  end
  
  it "should understand documents" do
    last_id = @engine.last_document_id
    @engine.consume("a\tb")
    @engine.last_document.should be_nil
    @engine.consume("\n")
    @engine.last_document_id.should == last_id + 1
    @engine.last_document.should == "a\tb\n"
  end
  
  it "should be able to parse a little" do
    @engine.consume("a\tb")
      @engine.terms.size.should == 1
  end
  
    it "should be able to register queries" do
      term = Term.new("hello", "world", 0)
      cb = Callback.new("data", proc{})
      q = Query.new([term], cb)   
      @engine.register(q)
      @engine.queries.should == [q]
    end
  
    it "should be able to remove queries" do
      term = Term.new("hello", "world", 0)
      cb = Callback.new("data", proc{})
      q = Query.new([term], cb)   
      id = @engine.register(q)
      @engine.queries.should == [q]
      @engine.unregister(id)
      @engine.queries.should == []
    end
  
  it "should be able to percolate a query" do
    term = Term.new("a", "hello", 0)
    cb = Callback.new(@engine, proc{|doc| $events << doc })
    q = Query.new([term], cb)   
    id = @engine.register(q)
	  @engine.consume("hello\tworld\n")
    $events.should == ["hello\tworld\n"]
	end
	
	it "should not respond to a miss" do
    term = Term.new("a", "nope", 0)
    cb = Callback.new(@engine, proc{|doc| $events << doc })
    q = Query.new([term], cb)   
    id = @engine.register(q)
	  @engine.consume("hello\tworld\n")
    $events.should == []
	end
	
end