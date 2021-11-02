#!/bin/bash

# Project-1

# Abbas Göktuğ Yılmaz - 1501115061
# Nurefşan Yücel - 150119802
# Veysi Öz - 150116005

# My Program - 1
# A script that takes a single command line argument which is a file containing one integer per   line.
# Our program print out a row of stars of the given length for each integer.

filename=$1

while read line;
do
	for (( i=0; i<$line; i++ ))
	do
		echo -n "*"
	done
	echo ""
done < $filename
