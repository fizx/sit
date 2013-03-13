require 'mkmf'
# CONFIG['CC'] = "gcc"
CONFIG['optflags'] = "-O0"
$CFLAGS << ' -g -std=c99
             -Wall -Wextra 
             -Wno-newline-eof 
             -Wno-declaration-after-statement 
             -Wno-comment 
             -Wno-sign-compare 
             -Wno-shorten-64-to-32 
             -Wno-switch-enum 
             -Wno-switch
						 -Wno-variadic-macros
             -o sit'.gsub(/\s+/, " ")

find_library('ev', 'ev_default_loop') && have_header("ev.h")

create_makefile('sit')

footer = <<-STR
$(TARGET): $(OBJS) Makefile
	$(ECHO) linking executable $(TARGET)
	@-$(RM) $(@)
	$(Q) $(CC) -dynamic -o $@ $(OBJS) $(LIBPATH) $(DLDFLAGS) $(LOCAL_LIBS) $(LIBS)
STR


existing = File.read("Makefile")
existing.sub!(/^all.*/, "all:\t$(DLLIB) $(TARGET)")
File.open("Makefile", "w") {|f| f.puts(existing + footer) }