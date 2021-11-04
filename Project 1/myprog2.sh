#!/bin/bash

# Project-1

# Abbas GÖktuğ Yılmaz - 150115061
# Nurefşan Yücel - 150119802
# Veysi Öz - 150116005

# My Program - 2
# A script that takes directory name as an argument, and it deletes every file in the directory that is not:
#	a c file (i.e. a file matching *.c) or
#	a header file (i.e. a file matching *.h) or
#	a file named "Makefile" or "makefile"
# This command is not attempt to remove subdirectories of the given directory.

directory=$1/*

for file in $directory
do
	if [[ $file != *.c && $file != *.h && $file != Makefile && $(basename $file) != makefile && $(basename $file) != Makefile && -f $file ]]; then
		rm -f $file
	fi
done
