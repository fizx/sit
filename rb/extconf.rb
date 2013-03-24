require 'mkmf'
CONFIG["CC"] = "gcc"
CONFIG['optflags'] = "-Os"
$CFLAGS << ' -std=c99 -D_GNU_SOURCE '
ROOT = File.expand_path(File.dirname(__FILE__) + "/..")

dir_config("sit", "#{ROOT}/src", "#{ROOT}/.libs")
have_library("sit", "white_tokenizer_new", "sit.h")
             
create_makefile('sit') or abort("error")