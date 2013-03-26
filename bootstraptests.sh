#!/bin/bash
cd `dirname $0`
command -v bundle || gem install bundler
bundle
make
cd rb
./configure
make
cd ..
rspec