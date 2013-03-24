#!/usr/bin/env ruby -w
# encoding: UTF-8
require File.dirname(__FILE__) + "/test_helper"

describe "Parser" do
	before do
	  $bufs = []
	  $hit = false
	end
  
  it "should do solr" do
    str = "INFO: [1234567898765] webapp=/solr path=/select params={start=0&q=*:*&wt=ruby&fq=type:User&rows=30} hits=3186235 status=0 QTime=1\n"
    @parser = Parser.new_solr()
    @parser.on_document(proc{|db|
      db.terms.should == [
        Term.new("severity", "INFO", 0, false),
        Term.new("core", "1234567898765", 0, false),
        Term.new("path", "/select", 0, false)
      ]
      db.ints.should == {"hits"=>3186235, "qtime"=>1, "status"=>0}
      db.doc.should == str
      $hit = true
    })
    @parser.consume(str)
    $hit.should be_true
  end
end