#!/usr/bin/env ruby -w
# encoding: UTF-8
require "rubygems"
require "bundler/setup"
require "rspec"
require "json"
require "rr"
require File.dirname(__FILE__) + "/../sit"
include Sit

A = "foo ~ a"
B = "foo ~ b"
C = "foo ~ c"
D = "foo ~ d"
E = "foo ~ e"

describe "QueryParser" do
  before do
    @qp = QueryParser.new
  end
  
  it "should be creatable" do
  end
  
  it "should have queries" do
    @qp.queries.should == []
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
  end

  it "should understand precedence" do
    @qp.consume("#{A} AND #{B} AND #{C} AND #{D} OR #{E};")
    @qp.last_error.should be_nil
    @qp.last_query_to_s.should == "((#{A} AND #{B} AND #{C} AND #{D}) OR #{E})"
  end
  
  it "should bubble ors" do
    @qp.consume("#{A} AND (#{B} OR #{C});")
    @qp.last_error.should be_nil
    @qp.last_query_to_s.should == "((foo ~ a AND foo ~ b) OR (foo ~ a AND foo ~ c))"
  end
  
  # it "should bubble crazy ors" do
  #   @qp.consume("#{A} AND (#{B} OR (#{C} AND #{D} OR #{E}));")
  #   @qp.last_error.should be_nil
  #   @qp.last_query_to_s.should == ""
  # end
  
end