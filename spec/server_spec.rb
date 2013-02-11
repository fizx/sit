#!/usr/bin/env ruby -w
# encoding: UTF-8
require "rubygems"
require "bundler/setup"
require "fileutils"
require "rspec"
require "json"
require "rr"
require "socket"
require File.dirname(__FILE__) + "/../sit"
include Sit
include FileUtils

# Sit.test_mode = true
# 
# def harikari(ppid)
#   Thread.new do
#     loop do
#       begin
#         Process.kill(0, ppid)
#       rescue
#         exit
#       end
#       sleep 1
#     end
#   end
# end
# ppid = Process.pid
# 
# describe "Server" do
#   it "should start, take a document, and quit" do
#     @engine = Engine.new(Parser.new_json, 1_000_000)
#     @server = Server.new(@engine)
#     rm_f "/tmp/foo"
#     system "mkfifo /tmp/foo"
#     fork {
#       harikari(ppid)
#       sleep(1)
#       puts "opening socket"
#       sock = TCPSocket.new('localhost', 5555)
#       puts "writing"
#       sock.write(["add", {"hello" => "world"}.to_json, "stop"].join("\n") + "\n")
#       puts "flushing"
#       sock.flush
#       puts "reading"
#       r = sock.read
#       puts "read complete"
#       out = open("/tmp/foo", "w+")
#       out.print r
#       puts "closing"
#       out.close
#       sock.close
#     }
#     @server.start(5555)
#     input = open("/tmp/foo", "r+")
#     puts "reading fifo"
#     input.gets.should == "{ok}"
#   end
# end