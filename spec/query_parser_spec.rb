#!/usr/bin/env ruby -w
# encoding: UTF-8
require "rubygems"
require "bundler/setup"
require "rspec"
require "json"
require "rr"
require File.dirname(__FILE__) + "/../rb/sit"
include Sit

A = "foo ~ a"
B = "foo ~ b"
C = "foo ~ c"
D = "foo ~ d"
E = "foo ~ e"

describe "QueryParser" do
  before do
    @qp = QueryParser.new
		$queries = []
  end
  
  it "should be creatable" do
  end
      
  it "should consume strings" do
    status = @qp.consume("foo ~ bar;")
    @qp.last_error.should be_nil
    status.should == :more
    @qp.last_query_to_s.should == "foo ~ bar"
  end
  
  it "should consume partial" do
    @qp.consume("foo ~ bar").should == :more
  end
  
  it "should understand precedence" do
    @qp.consume("#{A} AND #{B} OR #{C} AND #{D} OR #{E};")
    @qp.last_error.should be_nil
    @qp.last_query_to_s.should == "((#{A} AND #{B}) OR (#{C} AND #{D}) OR #{E})"
		$queries.size.should == 1
  end
  
  # 
  it "should understand parens" do
    @qp.consume("#{A} AND (#{B} OR #{C});")
    @qp.last_error.should be_nil
    # "(#{A} AND (#{B} OR #{C}))" # without bubbling ors
    @qp.last_query_to_s.should == "((foo ~ a AND foo ~ b) OR (foo ~ a AND foo ~ c))" # with bubbling ors
  end
  
  it "should understand parens with a clause after" do
    @qp.consume("#{A} AND (#{B} OR #{C}) AND #{D} OR #{E};")
    @qp.last_error.should be_nil
    @qp.last_query_to_s.should == "((foo ~ a AND foo ~ b AND foo ~ d) OR (foo ~ a AND foo ~ c AND foo ~ d) OR foo ~ e)"
    # "((foo ~ a AND (foo ~ b OR foo ~ c) AND foo ~ d) OR foo ~ e)" # without bubbling ors
  end
    
  it "should group clauses" do
    @qp.consume("#{A} AND #{B} AND #{C} AND #{D} OR #{E};")
    @qp.last_error.should be_nil
    @qp.last_query_to_s.should == "((#{A} AND #{B} AND #{C} AND #{D}) OR #{E})"
  end
  
  it "should bubble ors" do
    @qp.consume("#{A} AND (#{B} OR #{C});")
    @qp.last_error.should be_nil
    @qp.last_query_to_s.should == "((foo ~ a AND foo ~ b) OR (foo ~ a AND foo ~ c))"
  end
  
  it "should bubble crazy ors" do
    @qp.consume("#{A} AND (#{B} OR (#{C} AND #{D} OR #{E}));")
    @qp.last_error.should be_nil
    @qp.last_query_to_s.should == "((foo ~ a AND foo ~ b) OR (foo ~ a AND foo ~ c AND foo ~ d) OR (foo ~ a AND foo ~ e))"
  end
  
  it "should bubble NOTS downward 1" do  
    @qp.consume("#{A} AND NOT (#{B} OR #{C});")
    @qp.last_error.should be_nil
    @qp.last_query_to_s.should == "(#{A} AND NOT #{B} AND NOT #{C})"
  end
  
  it "should bubble NOTS downward 2" do  
    @qp.consume("#{A} AND NOT (#{B} AND #{C});")
    @qp.last_error.should be_nil
    @qp.last_query_to_s.should == "((#{A} AND NOT #{B}) OR (#{A} AND NOT #{C}))"
  end
  
  it "should work with multi-term queries" do  
    @qp.consume("foo ~ \"hello sweet world\";")
    @qp.last_error.should be_nil
    @qp.last_query_to_s.should == "(foo ~ hello AND foo ~ sweet AND foo ~ world)"
  end
  
end