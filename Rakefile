require "rubygems"
require "bundler/setup"
require "rspec/core/rake_task"
require "fileutils"
require "json"
include FileUtils::Verbose

RSpec::Core::RakeTask.new(:spec)

def sys(*a); puts a.join(" "); system(*a); end
def sys!(*a); puts a.join(" "); system(*a) or exit(1); end

task :test => :spec

task :default => :make

task :make => "Makefile" do
	sys!("make")
end

file "Makefile" => "y.tab.h" do
	sys!("./configure")
end

file "y.tab.h" => :bison
task :bison do 
  sys "flex query_scanner.l && /usr/local/bin/bison -ytd query_parser.y"
end

task :clean do
  sys "make clean"
  rm Dir["*.yy.c"]
  rm Dir["*.tab.*"]
	rm_f "Makefile"
end

task :valgrind do
  exec "cat spec/twitter.json | valgrind --track-origins=yes --leak-check=full --dsymutil=yes --show-reachable=yes ./sit"
end