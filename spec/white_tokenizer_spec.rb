#!/usr/bin/env ruby -w
# encoding: UTF-8
require File.dirname(__FILE__) + "/test_helper"

describe "Tokenizer" do
	it "should do whitespace" do
	  tokens = []
	  parser = Tokenizer.new_whitespace(tokens)
	  parser.consume("hello world")
		tokens.should == ["hello", "world"]
  end
end