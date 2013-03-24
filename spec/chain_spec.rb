#!/usr/bin/env ruby -w
# encoding: UTF-8
require File.dirname(__FILE__) + "/test_helper"
require "socket"

ppid = Process.pid
SIT = "cd \"#{File.dirname(__FILE__)}/..\" && ./sit"

describe "chaining" do  
  before do
    fork do
      exec("#{SIT} --port 4001 --test-mode 2>/dev/null")
    end
    fork do
      exec("#{SIT} --port 4002 --test-mode 2>/dev/null")
    end
    sleep 0.1
  end
  
  after do
    system "echo stop | nc 127.0.0.1 4001"
    system "echo stop | nc 127.0.0.1 4002"
  end
  
  it "should work" do
    added = "{\"status\": \"ok\", \"message\": \"added\", \"details: {\"id\": 0, \"type\": \"client\", \"endpoint\": \"127.0.0.1:4002\", \"connected\": false}}\n"
    a = TCPSocket.new('localhost', 4001)
    b = TCPSocket.new('localhost', 4002)
    a.write("connect 127.0.0.1:4002\n")
    a.flush
    a.recv(1024).should == added
    a.write("tasks\n")
    a.recv(1024).should =~ /connected\W+true/
    a.write("register *;\n")
    a.recv(1024).should == "{\"status\": \"ok\", \"message\": \"registered\", \"id\": 0}\n"
    a.write("tell $! raw\n")
    a.recv(1024).should == "{\"status\": \"ok\", \"message\": \"success\"}\n"
    a.write("tell $! register *;\n")
    a.recv(1024).should == "{\"status\": \"ok\", \"message\": \"success\"}\n"
    sleep 0.01 # two channels open, mostly avoid the race
    b.write('{"hello": "world"}'"\n")
    a.recv(1024).should == "{\"status\": \"ok\", \"message\": \"found\", \"query_id\": 0, \"doc_id\": 0}\n"
    a.close
    b.close
  end
end