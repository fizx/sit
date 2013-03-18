#!/usr/bin/env ruby -w
# encoding: UTF-8
require "rubygems"
require "bundler/setup"
require "rspec"
require "json"
require "rr"
require File.dirname(__FILE__) + "/../sit"
include Sit
include FileUtils

def abs(path)
  File.expand_path(File.dirname(__FILE__) + "/#{path}")
end

describe "Task" do
  before do
    @engine = Engine.new(Parser.new_json, 1_000_000, false)
    cp(abs("sample.json"), abs("tmp.json"))
    @task = Task.new_tail(@engine, abs("tmp.json"))
  end
  
	it "should be tailing the file" do
	  Sit.ev_loop_until {
      @engine.last_document_id == 29
    }
	  
	  File.open(abs("tmp.json"), "a") {|f| f.puts(File.read(abs("sample.json"))) }

	  Sit.ev_loop_until {
      @engine.last_document_id == 59
    }
	end
	
	it "should to_json" do
	  @task.to_json.should == "{\"id\": 1, \"type\": \"tail\", \"path\": \"#{abs("tmp.json").gsub('/', '\\/')}\"}"
  end
  
  it "should register with engine" do
	  @engine.tasks.size.should == 1
  end
  
  it "should be able to unregister with engine" do
	  @engine.tasks.size.should == 1
	  @engine.release_task(@task.task_id)
	  @engine.tasks.size.should == 0
  end
end