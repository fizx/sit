#!/bin/bash
for FILE in `ls spec/*spec.rb`
do
  echo $FILE
  rspec $FILE
done