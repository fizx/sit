#!/usr/bin/env ruby -w
# encoding: UTF-8
require File.dirname(__FILE__) + "/test_helper"

# Sit.test_mode = true

describe "integration" do
  before do
    @engine = Engine.new(Parser.new_json, 1_000_000, false)
    @output = []
    @input = Input.new(@engine, 1 << 20, @output)
    @proto = LineProtocol.new(@input)
  end
  
  it "should be able to handle a lot of lines" do
    10.times do
      @proto.consume("{\"hello\":\"world\"}\n" * 10000)
    end
  end
end