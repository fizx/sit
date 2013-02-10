#!/usr/bin/env ruby -w
# encoding: UTF-8
require "rubygems"
require "bundler/setup"
require "rspec"
require "json"
require "rr"
require "socket"
require File.dirname(__FILE__) + "/../sit"
include Sit

def harikari(ppid)
  Thread.new do
    loop do
      begin
        Process.kill(0, ppid)
      rescue
        exit
      end
      sleep 1
    end
  end
end
ppid = Process.pid

describe "Server" do
	it "should start, take a document, and quit" do
    @engine = Engine.new(Parser.new_json, 1_000_000)
    @server = Server.new(@engine)
    fork {
      harikari(ppid)
      sleep(0.1)
      sock = TCPSocket.new('localhost', 5555)
      sock.write(["add", {"hello" => "world"}.to_json, "stop"].join("\n"))
      sock.write("\n")
      sock.flush
      sock.read.should == "hi"
      sock.close
    }
    @server.start(5555)
	end
end