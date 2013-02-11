#!/usr/bin/env ruby -w
# encoding: UTF-8
require "rubygems"
require "bundler/setup"
require "rspec"
require "json"
require "rr"
require File.dirname(__FILE__) + "/../sit"
include Sit

describe "LineProtocol" do
  before do
    @engine = Engine.new(Parser.new_json, 1_000_000)
 		@input = Input.new(@engine, 1 << 10, 1 << 20)
    @proto = LineProtocol.new(@input)
  end

  it "should parse commands" do
    @proto.consume("add\n")
    @proto.output.should == ['{"status": "ok", "message": "ack", "command": "add"}']
  end
  
  it "should parse data" do
    @proto.consume('add
{"hello": "world"}
{"hello": "world"}
{"hello": "world"}
{"hello": "world"}
')
    @proto.output.should == [
     "{\"status\": \"ok\", \"message\": \"ack\", \"command\": \"add\"}",
     "{\"status\": \"ok\", \"message\": \"added\", \"doc_id\": 0\"}",
     "{\"status\": \"ok\", \"message\": \"added\", \"doc_id\": 1\"}",
     "{\"status\": \"ok\", \"message\": \"added\", \"doc_id\": 2\"}",
     "{\"status\": \"ok\", \"message\": \"added\", \"doc_id\": 3\"}"
    ]
  end
  
  it "should parse query registration" do
    @proto.consume('register
hello ~ world;
')
    @proto.output.first.should == "{\"status\": \"ok\", \"message\": \"ack\", \"command\": \"register\"}"
    @proto.output.last.should =~ /\{\"status\": \"ok\", \"message\":\"registered\", \"id\": \d+}/
  end
end