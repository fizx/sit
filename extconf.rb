require 'mkmf'

CONFIG['optflags'] = "-O0"
$CFLAGS << ' -g -std=c99 
             -pedantic -Wall -Wextra 
             -Wno-newline-eof 
             -Wno-declaration-after-statement 
             -Wno-comment 
             -Wno-sign-compare 
             -Wno-shorten-64-to-32 
             -Wno-switch-enum 
             -Wno-switch
             -o sit'.gsub(/\s+/, " ")
if have_library('ev', 'ev_default_loop')
  have_header("ev.h")
  find_library('ev', 'ev_default_loop')
end

if have_library('', 'ev_default_loop')
  have_header("pcre.h")
  find_library('ev', 'ev_default_loop')
end

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