require "rubygems"
require "bundler/setup"
require "rspec/core/rake_task"

RSpec::Core::RakeTask.new(:spec)

task :default => :spec

task :bison do 
  system "flex query_scanner.l && bison -yd query_parser.y"
end