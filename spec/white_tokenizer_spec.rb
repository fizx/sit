#!/usr/bin/env ruby -w
# encoding: UTF-8
require "rubygems"
require "bundler/setup"
require "rspec"
require "json"
require "rr"
require File.dirname(__FILE__) + "/../sit"
include Sit

describe "Tokenizer" do
	it "should do whitespace" do
	  tokens = []
	  parser = Tokenizer.new_whitespace(tokens)
	  parser.consume("hello world")
	  tokens.should == ["hello"]
		parser.end_stream
		tokens.should == ["hello", "world"]
  end
end