require "rubygems"
require "bundler/setup"
require "rspec/core/rake_task"
require "fileutils"
include FileUtils::Verbose

RSpec::Core::RakeTask.new(:spec)

def sys(*a); puts a.join(" "); system(*a); end

task :default => :spec

task :bison do 
  sys "flex query_scanner.l && /usr/local/bin/bison -yd query_parser.y"
end

task :clean do
  sys "make clean"
  rm Dir["*.yy.c"]
  rm Dir["*.tab.*"]
end