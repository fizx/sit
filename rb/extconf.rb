require 'mkmf'
# CONFIG["CC"] = "gcc"
CONFIG['optflags'] = "-Os"
CONFIG['CFLAGS'] << ' -std=c99 -D_GNU_SOURCE '
ROOT = File.expand_path(File.dirname(__FILE__) + "/..")

dir_config("sit", "#{ROOT}/src", "#{ROOT}/.libs")
dir_config("pcre", "#{ROOT}/contrib/pcre", "#{ROOT}/contrib/pcre/.libs")
dir_config("ev", "#{ROOT}/contrib/libev", "#{ROOT}/contrib/libev/.libs")

have_library("sit", "white_tokenizer_new", "sit.h")
have_library("ev", "ev_default_loop", "ev.h")
have_library("pcre", "pcre_compile", "pcre.h")
             
create_makefile('sit') or abort("error")