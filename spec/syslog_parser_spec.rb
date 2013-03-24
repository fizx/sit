#!/usr/bin/env ruby -w
# encoding: UTF-8
require File.dirname(__FILE__) + "/test_helper"

describe "Parser" do
	before do
	  $bufs = []
	  $hit = false
	end
  
  it "should do syslog" do
    str = "Mar 15 17:15:01 ip-10-148-147-201 CRON[24132]: (root) CMD (command -v debian-sa1 > /dev/null && debian-sa1 1 1)\n"
    @parser = Parser.new_syslog()
    @parser.on_document(proc{|db|
      db.terms.inspect.should == "[[host:ip-10-148-147-201 0], [tag:CRON 0], [tag:24132 1], [message:root 0], [message:CMD 1], [message:command 2], [message:v 3], [message:debian 4], [message:sa1 5], [message:dev 6], [message:null 7], [message:debian 8], [message:sa1 9], [message:1 10], [message:1 11]]"
      db.ints.should == {"time"=>1363392901}
      db.doc.should == str
      $hit = true
    })
    @parser.consume(str)
    $hit.should be_true
  end
end