MAINTAINERCLEANFILES = Makefile.in aclocal.m4 configure

SUBDIRS = contrib/libev contrib/pcre src

# EXTRA_DIST = README.md NOTICE LICENSE ChangeLog conf scripts notes

ruby: all
	cd rb && ./configure && make

check: ruby
	rspec
	
valgrind: sit
	glibtool --mode=execute valgrind --track-origins=yes --leak-check=full --dsymutil=yes ./sit --port 4000 --test-mode
