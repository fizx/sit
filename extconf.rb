require 'mkmf'

CONFIG['optflags'] = "-O0"
$CFLAGS << ' -g -std=c99 -pedantic -Wall -Wextra -Wno-newline-eof -Wno-declaration-after-statement -Wno-comment -Wno-sign-compare -Wno-shorten-64-to-32 -Wno-switch-enum -Wno-switch'
# find_library('ev', 'ev_default_loop')
create_makefile('sit')