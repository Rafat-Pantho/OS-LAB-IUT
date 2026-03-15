#!/bin/bash
# CSE4502 Operating Systems Lab
# Lab (Bash Scripting) 1B - Part A: File Size Monitor

if [ -z "$1" ]; then
    echo "Usage: $0 <filename>"
    exit 1
fi

filename="$1"

if [ ! -e "$filename" ]; then
    echo "Error: '$filename' does not exist."
    exit 1
fi

size=$(stat -c%s "$filename")

echo "File: $filename"
echo "Size: $size bytes"

if [ "$size" -lt 1000 ]; then
    echo "Category: Small file"
elif [ "$size" -le 10000 ]; then
    echo "Category: Medium file"
else
    echo "Category: Large file"
fi
