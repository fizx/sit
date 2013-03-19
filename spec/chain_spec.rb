#!/usr/bin/env ruby -w
# encoding: UTF-8
require "rubygems"
require "bundler/setup"
require "rspec"
require "socket"
require "json"
require "rr"
require File.dirname(__FILE__) + "/../sit"
include Sit

ppid = Process.pid
SIT = "cd \"#{File.dirname(__FILE__)}/..\" && ./sit"

describe "chaining" do  
  before do
    fork do
      exec("#{SIT} --port 4001 --test-mode")
    end
    fork do
      exec("#{SIT} --port 4002 --test-mode")
    end
    sleep 0.01
  end
  
  after do
    system "echo stop | nc 127.0.0.1 4001"
    system "echo stop | nc 127.0.0.1 4002"
  end
  
  it "should work" do
    a = TCPSocket.new('localhost', 4001)
    b = TCPSocket.new('localhost', 4002)
    
    a.close
    b.close
  end
end