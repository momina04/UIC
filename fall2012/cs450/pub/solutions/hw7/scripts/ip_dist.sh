#!/bin/bash
A=`cat data/IP_locations.txt | grep $1 | awk '{$1=""; print}'`
B=`cat data/IP_locations.txt | grep $2 | awk '{$1=""; print}'`
perl scripts/gcdist.pl $A $B
