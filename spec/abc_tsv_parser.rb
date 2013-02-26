require File.dirname(__FILE__) + "/../sit"

# A pretty terrible TSV parser for testing purposes
class AbcTsvParser < Sit::Parser 
	
	SEP = /\t|\n| /
	
	def initialize
	  super()
		@id = rand(1<<20)
		@buffer = ""
		@field = "a"
		@offset = 0
		@doc_start = 0
		@first = true
		@field_offset = 0
	end
	
	def consume(string)
		@buffer += string
		while sep = @buffer.index(SEP, @offset)
			case @buffer.getbyte(sep)
			when 10, "\n"
				size = sep - @offset
				doc_size = sep - @doc_start + 1 # include the newline
				term_found(@buffer[@offset, size], @field_offset) if size > 0
			  field_found("columns")
			  int_found(@field.getbyte(0) - "a".getbyte(0) + 1)
				document_found(@buffer[@doc_start, doc_size])
				@doc_start = @doc_start + doc_size
				@field = "a"
				@field_offset = 0
			when 9,  "\t"
				if @first
					@first = false
					field_found(@field.dup)
				end
				size = sep - @offset
				if size > 0
					term_found(@buffer[@offset, size], @field_offset)
				end
				@field.succ!
				field_found(@field.dup)
				@field_offset = 0
			when 32, " "
				if @first
					@first = false
					field_found(@field.dup)
				end
				size = sep - @offset
				if size > 0
					term_found(@buffer[@offset, size], @field_offset)
					@field_offset += 1 
				end
			end
			@offset = sep + 1
		end
	end
end

module TestEvents
	def self.new
		r = Sit::Receiver.new
		r.extend(TestEvents)
		r
	end
	
  $events = []
  def term_found(*args)
		$events << [:term] + args
	end
	
	def document_found(*args)
		$events << [:doc] + args
	end
	
	def field_found(*args)
		$events << [:field] + args
	end
	
	def int_found(*args)
		$events << [:int] + args
	end
end