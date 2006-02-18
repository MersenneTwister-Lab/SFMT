#!/bin/sh
for i in result*txt;do
  ./sep2.rb separated $i
  mv $i backup
done
