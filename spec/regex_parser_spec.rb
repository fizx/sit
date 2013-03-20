#!/usr/bin/env ruby -w
# encoding: UTF-8
require "rubygems"
require "bundler/setup"
require "rspec"
require "json"
require "rr"
require File.dirname(__FILE__) + "/../sit"
include Sit

describe "RegexParser" do  
  it "should be parsable" do
    hit = false
    string = "qtime=7 hello sweet world\n"
    query = <<-STR 
      `qtime=(?<qtime>\\d+) (?<text>.*)` WITH qtime as int, text as tokenized(` `);
    STR
    query.strip!
    parser = Parser.new_regex(query)
    parser.on_document(proc{|db|
      db.terms.inspect.should == "[[text:hello 0], [text:sweet 1], [text:world 2]]"
      db.ints.should == {"qtime" => 7}
      db.doc.should == string.chomp
      hit = true
    })
    parser.consume(string)
    hit.should be_true
  end
end