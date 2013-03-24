#!/usr/bin/env ruby -w
# encoding: UTF-8
require "rubygems"
require "bundler/setup"
require "rspec"
require "json"
require "rr"
require File.dirname(__FILE__) + "/../rb/sit"
include Sit

describe "DocBuf" do
  before do
    @doc_buf = DocBuf.new
  end
  
  it "should consume terms" do
    @doc_buf.field_found "foo"
    @doc_buf.int_found 1
    @doc_buf.term_found "bar", 3
    @doc_buf.doc_found "foo:bar"
    @doc_buf.ints.should == {"foo" => 1}
    @doc_buf.terms.should == [Term.new("foo", "bar", 3, false)]
    @doc_buf.doc.should == "foo:bar"
  end
end