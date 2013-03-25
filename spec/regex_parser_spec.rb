#!/usr/bin/env ruby -w
# encoding: UTF-8
require File.dirname(__FILE__) + "/test_helper"

describe "RegexParser" do  
  it "should be parsable" do
    hit = false
    string = "qtime=7 hello sweet world\n"
    query = <<-STR 
      `qtime=(?<qtime>\\d+) (?<text>.*)` WITH int(qtime), tokenized(text, ` `);
    STR
    query.strip!
    parser = Parser.new_regex(query)
    parser.on_document(proc{|db|
      db.ints.should == {"qtime" => 7}
      db.terms.inspect.should == "[[text:hello 0], [text:sweet 1], [text:world 2]]"
      db.doc.should == string.chomp
      hit = true
    })
    parser.consume(string)
    hit.should be_true
  end
  
  it "should be renamable" do
    hit = false
    string = "qtime=7 hello sweet world\n"
    query = <<-STR 
      `qtime=(?<qtime>\\d+) (?<text>.*)` WITH int(qtime), tokenized(text, ` `) AS foo;
    STR
    query.strip!
    parser = Parser.new_regex(query)
    parser.on_document(proc{|db|
      db.ints.should == {"qtime" => 7}
      db.terms.inspect.should == "[[foo:hello 0], [foo:sweet 1], [foo:world 2]]"
      db.doc.should == string.chomp
      hit = true
    })
    parser.consume(string)
    hit.should be_true
  end
  
  
  it "handle multiple docs" do
    hits = 0
    string = "qtime=7 hello sweet world\n"
    strings = string * 3
    query = <<-STR 
      `qtime=(?<qtime>\\d+) (?<text>.*)` WITH int(qtime), tokenized(text, ` `);
    STR
    query.strip!
    parser = Parser.new_regex(query)
    parser.on_document(proc{|db|
      db.terms.inspect.should == "[[text:hello 0], [text:sweet 1], [text:world 2]]"
      db.ints.should == {"qtime" => 7}
      db.doc.should == string.chomp
      hits += 1
    })
    parser.consume(strings)
    hits.should == 3
  end
  
  it "handle multiple docs across consumes" do
    hits = 0
    string = "qtime=7 hello sweet world\n"
    strings = string * 3
    query = <<-STR 
      `qtime=(?<qtime>\\d+) (?<text>.*)` WITH int(qtime), tokenized(text, ` `);
    STR
    query.strip!
    parser = Parser.new_regex(query)
    parser.on_document(proc{|db|
      db.terms.inspect.should == "[[text:hello 0], [text:sweet 1], [text:world 2]]"
      db.ints.should == {"qtime" => 7}
      db.doc.should == string.chomp
      hits += 1
    })
    strings.scan(/.{,7}/mi).each do |chunk|
      parser.consume(chunk)
    end
    hits.should == 3
  end
  
end