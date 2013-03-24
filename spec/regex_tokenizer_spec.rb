#!/usr/bin/env ruby -w
# encoding: UTF-8
require "rubygems"
require "bundler/setup"
require "rspec"
require "json"
require "rr"
require File.dirname(__FILE__) + "/../rb/sit"
include Sit

describe "Tokenizer" do
	it "should do tokens" do
	  tokens = []
	  parser = Tokenizer.new_regex("[a-z]+", tokens)
	  parser.consume("ABCabc123")
		tokens.should == ["ABC", "123"]
  end
  
  it "should parse syslog" do
    s = "Mar 15 17:15:01 ip-10-148-147-201 CRON[24131]: (munin) CMD (if [ -x /usr/bin/munin-cron ]; then /usr/bin/munin-cron; fi)"
    tokens = []
	  parser = Tokenizer.new_regex("\\b", tokens)
    parser.consume(s)
    tokens.should == s.split(/\b/)
    tokens.size.should == 52
    
    tokens = []
	  parser = Tokenizer.new_regex("\\W+", tokens)
    parser.consume(s)
    tokens.should == s.split(/\W+/)
    tokens.size.should == 26
    
  end
end