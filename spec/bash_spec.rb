#!/usr/bin/env ruby -w
# encoding: UTF-8
require "rubygems"
require "bundler/setup"
require "rspec"
require "json"
require "rr"
require File.dirname(__FILE__) + "/../rb/sit"
include Sit

describe "integration" do
	Dir[File.dirname(__FILE__) + "/proto/*.in"].each do |path|
		it "should run #{path} successfully" do
			out = path.sub(".in", ".out")
			opts = path.sub(".in", ".opts")
			File.exists?(out).should == true
			optstr = File.exists?(opts) ? File.read(opts).strip : ""
			`cd "#{File.dirname(path)}/../.." && cat #{path} | ./sit #{optstr} --test-mode 2>/dev/null`.strip.should == File.read(out).strip
		end
	end
end