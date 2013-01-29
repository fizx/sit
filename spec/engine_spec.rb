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
end