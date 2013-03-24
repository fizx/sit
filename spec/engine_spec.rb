#!/usr/bin/env ruby -w
# encoding: UTF-8
require File.dirname(__FILE__) + "/test_helper"

describe "Engine" do
  before do
    @engine = Engine.new(Parser.new_json, 1_000_000, false)
    @output = []
 		@input = Input.new(@engine, 1 << 20, @output)
    $events = []
    # Sit.test_mode = true
  end
  
  it "should understand documents" do
    last_id = @engine.last_document_id
    @engine.last_document.should be_nil
    @input.consume('{"hello": "world"}
    {"goodbye": "world"}');
    @engine.last_document.should == '{"goodbye": "world"}'
    @input.consume("\n")
    id = @engine.last_document_id
    id.should == last_id + 2
    @engine.get_int(id, "_level").should == 1
    @engine.incr(id, "_level", 2)
    @engine.get_int(id, "_level").should == 3
    @engine.set_int(id, "columns", 5)
    @engine.get_int(id, "columns").should == 5
  end
  
  it "should be able to register queries" do
    term = Term.new("hello", "world", 0, false)
    cj = Conjunction.new([term])
    cb = Callback.new(Numeric, proc{})
    q = Query.new([cj], cb)   
    @engine.register(q)
    @engine.queries.should == [q]
  end
    
  it "should be able to remove queries" do
    term = Term.new("hello", "world", 0, false)
    cb = Callback.new(Numeric, proc{})
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
    cb = Callback.new(Numeric, proc{|doc| $events << doc })
    q = Query.new([cj], cb)   
    id = @engine.register(q)
    @input.consume("{\"a\": \"hello world\"}\n")
    $events.should == [0]
  end
  
  it "should be able to percolate a not query" do
    term = Term.new("a", "dsafdsa", 0, true)
    cj = Conjunction.new([term])
    cb = Callback.new(Numeric, proc{|doc| $events << doc })
    q = Query.new([cj], cb)   
    id = @engine.register(q)
    @engine.queries.first.to_s.should =~ /<\(NOT a ~ dsafdsa\) cb:\d+>/
    @input.consume("{\"a\": \"hello world\"}\n")
    $events.should == [0]
  end
  
  it "can search a not query by itself" do
    term = Term.new("a", "miss", 0, true)
    cj = Conjunction.new([term])
    cb = Callback.new(Numeric, proc{|doc| $events << doc })
    q = Query.new([cj], cb) 
    @input.consume("{\"a\": \"hello world\"}\n")  
    cursor = @engine.search(q)
    while cursor.prev!
      cursor.call
    end
    $events.should == [0]
  end
  
  it "should be able to search a not query" do
    a = Term.new("a", "miss", 0, true)
    b = Term.new("a", "hello", 0, false)
    cj = Conjunction.new([a, b])
    cb = Callback.new(Numeric, proc{|doc| $events << doc })
    q = Query.new([cj], cb) 
    @input.consume("{\"a\": \"hello world\"}\n")  
    cursor = @engine.search(q)
    while cursor.prev!
      cursor.call
    end
    $events.should == [0]
  end
  
  it "should be able to search another not query" do
    a = Term.new("b", "world", 0, true)
    b = Term.new("a", "hello", 0, false)
    cj = Conjunction.new([a, b])
    cb = Callback.new(Numeric, proc{|doc| $events << doc })
    q = Query.new([cj], cb) 
    @input.consume("hello\tworld\n")  
    cursor = @engine.search(q)
    while cursor.prev!
      cursor.call
    end
    $events.should == []
  end
    
  it "should be able to search a query" do
    term = Term.new("a", "hello", 0, false)
    cj = Conjunction.new([term])
    cb = Callback.new(Numeric, proc{|doc| $events << doc })
    q = Query.new([cj], cb) 
    @input.consume("{\"a\": \"hello\", \"b\": \"world\"}\n")  
    cursor = @engine.search(q)
    while cursor.prev!
      cursor.call
    end
    $events.should == [0]
  end
  
  it "should be able to search a query" do
    term = Term.new("a", "miss", 0, false)
    cj = Conjunction.new([term])
    cb = Callback.new(Numeric, proc{|doc| $events << doc })
    q = Query.new([cj], cb) 
    @input.consume("{\"a\": \"hello\", \"b\": \"world\"}\n")  
    cursor = @engine.search(q)
    while cursor.prev!
      cursor.call
    end
    $events.should == []
  end
  
  it "should be able to search a numeric term" do
    term = Term.new_numeric("_level", "<", 2)
    cj = Conjunction.new([term])
    cb = Callback.new(Numeric, proc{|doc| $events << doc })
    q = Query.new([cj], cb) 
    @input.consume("{\"a\": \"hello\", \"b\": \"world\"}\n")  
    cursor = @engine.search(q)
    while cursor.prev!
      cursor.call
    end
    @engine.last_document_id.should == 0
    $events.should == [0]
  end
  
  it "should be able to miss a numeric term" do
    term = Term.new_numeric("columns", "=", 100000)
    cj = Conjunction.new([term])
    cb = Callback.new(Numeric, proc{|doc| $events << doc })
    q = Query.new([cj], cb) 
    @input.consume("{\"a\": \"hello\", \"b\": \"world\"}\n")  
    cursor = @engine.search(q)
    while cursor.prev!
      cursor.call
    end
    @engine.last_document_id.should == 0
    $events.should == []
  end
  
  it "should not respond to a miss" do
    term = Term.new("a", "nope", 0, false)
    cj = Conjunction.new([term])
    cb = Callback.new(Numeric, proc{|doc| $events << doc })
    q = Query.new([cj], cb)   
    id = @engine.register(q)
    @input.consume("{\"a\": \"hello\", \"b\": \"world\"}\n")
    $events.should == []
  end
end