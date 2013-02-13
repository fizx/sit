#!/usr/bin/env ruby
require "erb"

START = %r[(\s*)//!ruby]
END = %r[//!end]
DIRECTIVE = %r[^\s*//!(\S+)]
COMMENT = %r[^\s*//(.*)]

indent = 0
comment_indent = ""
Dir["*.[ch]"].each do |file|
  contents = File.read(file)
  state = :default
  if contents =~ START
    out = []
    erbbuf = []
    replacing = false
    lines = File.readlines
    while line = lines.shift
      case state
      when :default  
        out << line.chomp
        if(line =~ START)
          state = :erb 
          comment_indent = $1
        end
      when :erb  
        out << line.chomp
        if line =~ DIRECTIVE
        elsif line =~ COMMENT
          erbbuf << $1
        else
          template = ERB.new(erbbuf.join("\n"))
          results = template.result(binding).split("\n")
          results.each do |result|
            out << result
          end
          out << "#{comment_indent}//!end"
          state = :looking_for_end
        end
      when :looking_for_end
        state = :default if line =~ END
      end
    end
  end
  File.open(file, "w") {|h| h.print(out.join("\n"))}
end