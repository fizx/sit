#!/usr/bin/env ruby -w
# encoding: UTF-8
require File.dirname(__FILE__) + "/test_helper"

describe "Parser" do
	before do
	  $bufs = []
	  $hit = false
	end
  
  it "should do json" do
    str = "{\"hello\":\"world\"}"
    @parser = Parser.new_json()
    @parser.on_document(proc{|db|
      db.terms.should == [Term.new("hello", "world", 0, false)]
      $hit = true
    })
    @parser.consume(str)
    $hit.should be_true
  end
  
  it "should do tokenized json" do
    str = "{\"hello\":[\"world\", \"two\"]}"
    @parser = Parser.new_json()
    @parser.on_document(proc{|db|
      db.terms.should == [Term.new("hello", "world", 0, false), Term.new("hello", "two", 1, false)]
      $hit = true
    })
    @parser.consume(str)
    $hit.should be_true
  end
  
  it "should do account for newline" do
    str = "\n\n{\"hello\":\"world\"}  "
    @parser = Parser.new_json()
    @parser.on_document(proc{|db|
      db.terms.should == [Term.new("hello", "world", 0, false)]
      $hit = true
    })
    @parser.consume(str)
    $hit.should be_true
  end
    
  it "should do multiple docs" do
    str = "{\"hello\":\"world\"}\n{\"wha\":\"goodbye thanks\"}"
    answers = [
      [Term.new("hello", "world", 0, false)],
      [
        Term.new("wha", "goodbye", 0, false),
        Term.new("wha", "thanks", 1, false)
      ]
    ]
    @parser = Parser.new_json()
    @parser.on_document(proc{|db| 
      db.terms.should == answers.shift
    })
    @parser.consume(str)
    answers.should == []
  end      
end