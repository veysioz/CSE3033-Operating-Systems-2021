#!/bin/bash

# Project-1

# Abbas Göktuğ Yılmaz - 150115061
# Nurefşan YÜcel - 150119802
# Veysi Öz - 150116005

# My Program - 3
# A script that takes a filename and two words as arguments.
# Then it searches for the first word in that file and substitute all occurences of it with the second word and print how many substitutions was made.
# For example, if the program executes with a file including the word "apple" 3 times and the second word is "orange", then it changes all 3 "apple" to "orange".
# This program is case sensitive.

filename=$1
firstWord=$2
secondWord=$3

echo "All $(grep -o -w "$firstWord" "$filename" | wc -w) occurrences of \"$firstWord\" in \"$filename\" has changed with \"$secondWord\""

sed -i "s/$firstWord/$secondWord/g" "$filename"
