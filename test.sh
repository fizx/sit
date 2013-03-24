#!/bin/bash
cd `dirname $0`
command -v bundle || gem install bundler
bundle
make install
cd rb
ruby extconf.rb
make
cd ..
rspec