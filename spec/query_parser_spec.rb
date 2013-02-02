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
  end
  
  it "should consume partial" do
    @qp.consume("foo ~ bar").should == :more
  end
end