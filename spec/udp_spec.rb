#!/usr/bin/env ruby -w
# encoding: UTF-8
require File.dirname(__FILE__) + "/test_helper"

describe "udp" do
	it "should respond to udp" do
	  begin
  	  pid = fork do
  	    exec "./sit --port 4009 --log-file=/tmp/test.log"
  	  end
  	  sleep 0.1
	    u = UDPSocket.new
	    u.send "{}", 0, "127.0.0.1", 4009
	    u.send "{}", 0, "127.0.0.1", 4009
	    u.send "{}", 0, "127.0.0.1", 4009
	    
	    # don't blow up
	    u.send "dsfasdfdas", 0, "127.0.0.1", 4009
	    
	    # don't blow up
	    u.send "register *;", 0, "127.0.0.1", 4009
	    
	    
	    u.send "{}", 0, "127.0.0.1", 4009
	    u.send "{}", 0, "127.0.0.1", 4009
	    sleep 0.1
	    
	    t = TCPSocket.new "127.0.0.1", 4009
	    t.send "range\n", 0
	    t.gets.should == "{\"status\": \"ok\", \"message\": \"range\", \"min_id\": 0, \"max_id\": 4}\n"
	  ensure
	    Process.kill 9, pid
    end
	end
end