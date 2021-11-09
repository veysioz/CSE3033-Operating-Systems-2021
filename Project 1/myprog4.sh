#!/bin/bash

# Project-1

# Abbas Göktuğ Yılmaz - 150115061
# Nurefşan Yücel - 150119802
# Veysi Öz - 150116005

# My Program - 4
# A script to organize directory by first creating two sub directories named smallest and largest.
# Then the script moves the largest file in the current directory to largest directory and the smallest file to smallest directory.

directory=$1/*

mkdir -p $1/smallest $1/largest

largestSize=0
smallestSize=-1
largestFile=0
smallestFile=0

for file in $directory
do
	if [[ -f $file ]]; then
		if [[ $(find $file -printf "%s") -gt $largestSize ]]; then
			largestSize=$(find $file -printf "%s")
			largestFile=$file
		fi
		if [[ $(find $file -printf "%s") -lt $smallestSize || $smallestSize == -1 ]]; then
			smallestSize=$(find $file -printf "%s")
			smallestFile=$file
		fi
	fi
done

if [ -f $smallestFile ]; then
	mv -i $smallestFile $1/smallest
fi

if [ -f $largestFile ]; then
	mv -i $largestFile $1/largest
fi
