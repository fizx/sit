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

describe "AST" do
  before do
    @ast = AST.new
    
    @r = ASTNode.new(@ast, "r")
    @a = ASTNode.new(@ast, "a")
    @b = ASTNode.new(@ast, "b")
    
    @a.data.should == "a"
    @a.parent.should == nil
    @a.child.should == nil
    @a.prev.should == nil
    @a.next.should == nil
  end
  
  it "should be created" do
    @node = ASTNode.new(@ast, "hi")
  end
  # 
  # it "should be assignable" do
  #   @node = ASTNode.new(@ast, "hi")
  #   @node.data = "holla"
  #   @node.data.should == "holla"
  #   @node.data = []
  #   @node.data.should == []
  # end
  # 
  # it "should insert_after and remove" do
  #   ASTNode.insert_after(@a, @b)
  #   @a.next.should == @b
  #   @b.prev.should == @a
  #   
  #   ASTNode.remove(@a)
  #   @b.prev.should be_nil
  #   @a.next.should be_nil
  # end
  # 
  # it "should move when prepending child" do
  #   ASTNode.insert_after(@a, @b)
  #   ASTNode.prepend_child(@r, @a)
  #   @a.next.should be_nil
  # end
end