#!/usr/bin/env ruby

# This runs an erb code generator that I occasionally use when I want to generate code
# that's too much of a pita to generate with the ordinary CPP.
#
# You'll see it marked up as comments starting with //!ruby, and the generated code will
# go until //!end.

require "erb"

START = %r[(\s*)//!ruby]
FIN = %r[//!end]
DIRECTIVE = %r[^\s*//!(\S+.*)]
COMMENT = %r[^\s*//(.*)]

undent = 0
comment_indent = ""
Dir["*.[chly]"].each do |file|
  contents = File.read(file)
  state = :default
  if contents =~ START
    out = []
    replacing = false
    lines = File.readlines(file)
    while line = lines.shift
      case state
      when :default  
        out << line.chomp
        if(line =~ START)
          state = :erb 
          erbbuf = []
          comment_indent = $1
          undent = 1
        end
      when :erb  
        out << line.chomp
        if line =~ DIRECTIVE
          directive, *args = $1.split(/\s+/)
          if directive == "undent"
            undent = args.first.to_i
            puts "undenting #{undent}, #{args.inspect}"
          end
        elsif line =~ COMMENT
          erbbuf << $1
        else
          template = ERB.new(erbbuf.join("\n"), nil, '-')
          erbout = template.result(binding)
          erbout.gsub!(/\n[ \t]*\n[ \t]*\n/, "\n\n")
          erbout.gsub!(/\n[ \t]*\n[ \t]*\n/, "\n\n")
          erbout.gsub!(/\n[ \t]*\n[ \t]*\n/, "\n\n")
          erbout.gsub!(/\s*\Z/m, "")
          erbout.gsub!(/\A\s*\n/m, "\n")
          results = erbout.split("\n")
          results.each do |result|
            result =~ /([ ]{#{undent}}?)(.*)/
            undented = $2
            out << undented
          end
          out << "#{comment_indent}//!end"
          state = :looking_for_end
        end
      when :looking_for_end
        state = :default if line =~ FIN
      end
    end
    
    File.open(file, "w") {|h| h.print(out.join("\n"))}
  end
end