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
  end
  
	it "should link the parser" do
	  @engine.parser.engine.should == @engine
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
end