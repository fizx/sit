#!/usr/bin/env ruby -w
# encoding: UTF-8
require File.dirname(__FILE__) + "/test_helper"

# Sit.test_mode = true

describe "integration" do
  before do
    @engine = Engine.new(Parser.new_json, nil, 1_000_000, false, nil)
    @output = []
    @input = Input.new(@engine, 1 << 20, @output)
    @proto = LineProtocol.new(@input)
  end
  
  it "should understand auth" do
    @engine = Engine.new(Parser.new_json, nil, 1_000_000, false, "the password")
    @output = []
    @input = Input.new(@engine, 1 << 20, @output)
    @proto = LineProtocol.new(@input)
    @proto.consume("{\"hello\":\"world\"}\n")
    @proto.consume("auth the password\n")
    @proto.consume("{\"hello\":\"world\"}\n")
    @output.should == ["{\"status\": \"error\", \"message\": \"not authenticated\"}", 
                       "{\"status\": \"ok\", \"message\": \"welcome\"}", 
                       "{\"status\": \"ok\", \"message\": \"added\", \"doc_id\": 0}"]
  end
  
  it "should be able to handle a lot of lines" do
    10.times do
      @proto.consume("{\"hello\":\"world\"}\n" * 10000)
    end
  end
end