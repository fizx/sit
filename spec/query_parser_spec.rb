#!/usr/bin/env ruby -w
# encoding: UTF-8
require "rubygems"
require "bundler/setup"
require "rspec"
require "json"
require "rr"
require File.dirname(__FILE__) + "/../sit"
include Sit

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
    @qp.last_ast_to_s.should == "(foo ~ bar)"
  end
  
  it "should consume partial" do
    @qp.consume("foo ~ bar").should == :more
  end
  
  it "should consume strings" do
    @qp.consume("foo ~ bar AND bar ~ baz;")
    @qp.last_error.should be_nil
    @qp.last_ast_to_s.should == "((foo ~ bar) AND (bar ~ baz))"
  end
  
end