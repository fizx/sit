#!/usr/bin/env ruby -w
# encoding: UTF-8
require "rubygems"
require "bundler/setup"
require "rspec"
require "json"
require "rr"
require File.dirname(__FILE__) + "/../rb/sit"
include Sit

describe "Protocol" do
  before do
    $cmds = []
    $data = ""
    @proto = Protocol.new(
      proc{|cmd, args| $cmds << [cmd, args]},
      proc{|data| $data << data}
    )
  end

  it "should parse commands" do
    @proto.consume("hello\n")
    $cmds.should == [["hello", ""]]
  end
  
  it "should parse data" do
    @proto.consume("{hello}\n")
    $data.should == "{hello}"
  end
end