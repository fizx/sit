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

task :make => ["Makefile", :bison] do
	sys!("make")
end

file "Makefile" => "extconf.rb" do
	sys!("./configure")
end

task :bison => ["re.tab.h", "qp.tab.h"]

file "qp.tab.h" => FileList["query_*"] do
  sys "flex -P qp query_scanner.l && /usr/local/bin/bison -ytd -b qp query_parser.y"
end

file "re.tab.h" => FileList["regex_*"] do
  sys "flex -P re regex_scanner.l && /usr/local/bin/bison -ytd -b re regex_parser.y"
end

task :clean do
  sys "make clean"
  rm Dir["lex.*"]
  rm Dir["*.tab.*"]
	rm_f "Makefile"
end

task :valgrind do
  exec "cat spec/twitter.json | valgrind --track-origins=yes --leak-check=full --dsymutil=yes --show-reachable=yes ./sit"
end