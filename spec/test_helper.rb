begin
  require "rubygems"
  require "bundler/setup"
  require "rspec"
  require "socket"
  require "json"
  require "rr"
  require File.dirname(__FILE__) + "/../rb/sit"
  include Sit
rescue Exception => e
  puts e.message
  puts e.backtrace
  puts
  puts "=" * 80
  puts
  puts "    There's a pretty good chance you can fix the above"
  puts
  puts "=" * 80
  puts 
  exit(1)
end