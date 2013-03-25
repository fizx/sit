#!/usr/bin/env ruby -w
# encoding: UTF-8
require File.dirname(__FILE__) + "/test_helper"
include FileUtils

def abs(path)
  File.expand_path(File.dirname(__FILE__) + "/#{path}")
end

describe "Task" do
  before do
    @engine = Engine.new(Parser.new_json, 1_000_000, false)
    cp(abs("sample.json"), abs("tmp.json"))
    @task = Task.new_tail(@engine, abs("tmp.json") + " json")
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
	  @task.to_json.should be_start_with("{\"id\": 1, \"type\": \"tail\", \"path\": \"#{abs("tmp.json").gsub('/', '\\/')}\", \"docs\": 0, \"errors\": 0, \"offset\": ")
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