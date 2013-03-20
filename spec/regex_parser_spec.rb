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
  before do
    $hit = false
  end
  
  it "should be parsable" do
    string = "qtime=7 hello sweet world\n"
    query = <<-STR 
      `qtime=(?<qtime>\\d+) (?<text>.*)` WITH qtime as int, text as tokenized(` `);
    STR
    query.strip!
    parser = Parser.new_regex(query)
    parser.on_document(proc{|db|
      db.terms.inspect.should == "[[text:hello 0], [text:sweet 0], [text:world 0]]"
      db.ints.should == {"time" => 7}
      db.doc.should == str
      $hit = true
    })
    @parser.consume(str)
    $hit.should be_true
  end
end