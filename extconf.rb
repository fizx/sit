require 'mkmf'
# CONFIG['CC'] = "gcc"
CONFIG['optflags'] = "-O0"
$CFLAGS << ' -g -std=c99
             -Wall -Wextra
             -D_GNU_SOURCE
             -DCOMPILE_WITH_DEBUG 
             -Wno-newline-eof 
             -Wno-declaration-after-statement 
             -Wno-comment 
             -Wno-sign-compare 
             -Wno-shorten-64-to-32 
             -Wno-switch-enum 
             -Wno-switch
						 -Wno-variadic-macros
             -o sit'.gsub(/\s+/, " ")
             
headers = %w[/usr/local/include /usr/include]
libs = %w[/usr/local/lib /usr/lib]
out = `pcre-config --libs`[/-L(\S+)/, 1]
libs.unshift(out) if out

find_library('ev', 'ev_default_loop', *libs)   &&
find_header("ev.h", *headers)                  &&
find_header("aio.h", *headers)                 &&
find_library('pcre', 'pcre_free_study', *libs) &&
find_header("pcre.h", *headers)                &&
find_header("sys/queue.h", *headers)           &&
create_makefile('sit')                         ||
abort("error")

footer = <<-STR
$(TARGET): $(OBJS) Makefile
	$(ECHO) linking executable $(TARGET)
	@-$(RM) $(@)
	$(Q) $(CC) -dynamic -o $@ $(OBJS) $(LIBPATH) $(DLDFLAGS) $(LOCAL_LIBS) $(LIBS)
STR

existing = File.read("Makefile")
existing.sub!(/^all.*/, "all:\t$(DLLIB) $(TARGET)")
File.open("Makefile", "w") {|f| f.puts(existing + footer) }