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
    id = @engine.last_document_id
    id.should == last_id + 1
    @engine.last_document.should == "a\tb\n"
    @engine.get_int(id, "columns").should == 2
    @engine.incr(id, "columns", 1)
    @engine.get_int(id, "columns").should == 3
    @engine.set_int(id, "columns", 5)
    @engine.get_int(id, "columns").should == 5
  end
  
  it "should understand integers" do
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
    term = Term.new("hello", "world", 0, false)
    cj = Conjunction.new([term])
    cb = Callback.new(String, proc{})
    q = Query.new([cj], cb)   
    @engine.register(q)
    @engine.queries.should == [q]
  end
  
  it "should be able to remove queries" do
    term = Term.new("hello", "world", 0, false)
    cb = Callback.new(String, proc{})
    cj = Conjunction.new([term])
    q = Query.new([cj], cb)   
    id = @engine.register(q)
    @engine.queries.should == [q]
    @engine.unregister(id)
    @engine.queries.should == []
  end
  
  it "should be able to percolate a query" do
    term = Term.new("a", "hello", 0, false)
    cj = Conjunction.new([term])
    cb = Callback.new(String, proc{|doc| $events << doc })
    q = Query.new([cj], cb)   
    id = @engine.register(q)
    @engine.consume("hello\tworld\n")
    $events.should == ["hello\tworld\n"]
  end
  
  it "should be able to percolate a not query" do
    term = Term.new("a", "dsafdsa", 0, true)
    cj = Conjunction.new([term])
    cb = Callback.new(String, proc{|doc| $events << doc })
    q = Query.new([cj], cb)   
    id = @engine.register(q)
    @engine.consume("hello\tworld\n")
    $events.should == ["hello\tworld\n"]
  end
  
  it "cannot search a not query by itself" do
    term = Term.new("a", "miss", 0, true)
    cj = Conjunction.new([term])
    cb = Callback.new(String, proc{|doc| $events << doc })
    q = Query.new([cj], cb) 
    @engine.consume("hello\tworld\n")  
    cursor = @engine.search(q)
    while cursor.prev!
      cursor.call
    end
    $events.should == []
  end
  
  
  it "should be able to search a not query" do
    a = Term.new("a", "miss", 0, true)
    b = Term.new("a", "hello", 0, false)
    cj = Conjunction.new([a, b])
    cb = Callback.new(String, proc{|doc| $events << doc })
    q = Query.new([cj], cb) 
    @engine.consume("hello\tworld\n")  
    cursor = @engine.search(q)
    while cursor.prev!
      cursor.call
    end
    $events.should == ["hello\tworld\n"]
  end
  
  it "should be able to search another not query" do
    a = Term.new("b", "world", 0, true)
    b = Term.new("a", "hello", 0, false)
    cj = Conjunction.new([a, b])
    cb = Callback.new(String, proc{|doc| $events << doc })
    q = Query.new([cj], cb) 
    @engine.consume("hello\tworld\n")  
    cursor = @engine.search(q)
    while cursor.prev!
      cursor.call
    end
    $events.should == []
  end
  
  it "should be able to search a query" do
    term = Term.new("a", "hello", 0, false)
    cj = Conjunction.new([term])
    cb = Callback.new(String, proc{|doc| $events << doc })
    q = Query.new([cj], cb) 
    @engine.consume("hello\tworld\n")  
    cursor = @engine.search(q)
    while cursor.prev!
      cursor.call
    end
    $events.should == ["hello\tworld\n"]
  end
  
  it "should be able to search a query" do
    term = Term.new("a", "miss", 0, false)
    cj = Conjunction.new([term])
    cb = Callback.new(String, proc{|doc| $events << doc })
    q = Query.new([cj], cb) 
    @engine.consume("hello\tworld\n")  
    cursor = @engine.search(q)
    while cursor.prev!
      cursor.call
    end
    $events.should == []
  end
  
  it "should not respond to a miss" do
    term = Term.new("a", "nope", 0, false)
    cj = Conjunction.new([term])
    cb = Callback.new(String, proc{|doc| $events << doc })
    q = Query.new([cj], cb)   
    id = @engine.register(q)
    @engine.consume("hello\tworld\n")
    $events.should == []
  end
  
end