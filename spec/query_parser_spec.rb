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
    @qp.last_ast_to_s.should == "[EXPR NULL 0 _NA false]\n\t[CLAUSE NULL 0 _NA false]\n\t\t[STR foo 0 _NA false]\n\t\t[CMP NULL 0 _TILDE false]\n\t\t[STR bar 0 _NA false]\n"
  end
  
  it "should consume partial" do
    @qp.consume("foo ~ bar").should == :more
  end
  
  it "should understand precedence" do
    @qp.consume("#{A} AND #{B} OR #{C} AND #{D} OR #{E};")
    @qp.last_error.should be_nil
    @qp.last_ast_to_s.should == "[EXPR NULL 0 _NA false]\n\t[CLAUSE NULL 0 _NA false]\n\t\t[STR foo 0 _EQ false]\n\t\t[CMP NULL 0 _TILDE false]\n\t\t[STR a 0 _NA false]\n\t[BAND NULL 0 _NA false]\n\t[CLAUSE NULL 0 _NA false]\n\t\t[STR foo 0 _NA false]\n\t\t[CMP NULL 0 _TILDE false]\n\t\t[STR b 0 _NA false]\n\t[BOR NULL 0 _NA false]\n\t[CLAUSE NULL 0 _NA false]\n\t\t[STR foo 0 _NA false]\n\t\t[CMP NULL 0 _TILDE false]\n\t\t[STR c 0 _NA false]\n\t[BAND NULL 0 _NA false]\n\t[CLAUSE NULL 0 _NA false]\n\t\t[STR foo 0 _NA false]\n\t\t[CMP NULL 0 _TILDE false]\n\t\t[STR d 0 _NA false]\n\t[BOR NULL 0 _NA false]\n\t[CLAUSE NULL 0 _NA false]\n\t\t[STR foo 0 _NA false]\n\t\t[CMP NULL 0 _TILDE false]\n\t\t[STR e 0 _NA false]\n"
  end
  
end