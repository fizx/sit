require 'mkmf'
CONFIG['optflags'] = "-Os"
$CFLAGS << '-std=c99 -D_GNU_SOURCE'

             #              -Wall -Wextra
             #              -Wno-newline-eof 
             #              -Wno-declaration-after-statement 
             #              -Wno-comment 
             #              -Wno-sign-compare 
             #              -Wno-shorten-64-to-32 
             #              -Wno-switch-enum 
             #              -Wno-switch
             # -Wno-variadic-macros

             
libs = headers = %w[libev pcre-8.32]

find_header("pcre.h", *headers)                &&
find_header("ev.h", *headers)                  &&
find_header("aio.h", *headers)                 &&
find_library('ev', 'ev_default_loop', *libs)   &&
find_library('pcre', 'pcre_compile', *libs)    &&
find_library('pcre', 'pcre_free_study', *libs) &&
create_makefile('sit')                         ||
abort("error")

footer = <<-STR
$(TARGET): $(OBJS) Makefile
	echo linking executable $(TARGET)
	@-$(RM) $(@)
	$(Q) $(CC) -dynamic -o $@ $(OBJS) $(LIBPATH) $(DLDFLAGS) $(LOCAL_LIBS) $(LIBS)
STR

existing = File.read("Makefile")
existing.sub!(/^all.*/, "all:\t$(DLLIB) $(TARGET)")
File.open("Makefile", "w") {|f| f.puts(existing + footer) }