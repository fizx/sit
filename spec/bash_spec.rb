#!/usr/bin/env ruby -w
# encoding: UTF-8
require "rubygems"
require "bundler/setup"
require "rspec"
require "json"
require "rr"
require File.dirname(__FILE__) + "/../sit"
require File.dirname(__FILE__) + "/abc_tsv_parser"
include Sit

describe "integration" do
	Dir[File.dirname(__FILE__) + "/bash/*.flags"].each do |path|
		it "should run #{path} successfully" do
			out = path.sub(".flags", ".out")
			File.exists?(out).should == true
			`cd "#{File.dirname(path)}/../.." && ./sit #{File.read(path)} 2>/dev/null`.should == File.read(out)
		end
	end
end