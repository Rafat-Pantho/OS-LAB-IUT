#!/bin/bash
# CSE4502 Operating Systems Lab
# Lab (Bash Scripting) 2A - Part A: File Permission Checker

if [ -z "$1" ]; then
    echo "Usage: $0 <filename>"
    exit 1
fi

filename="$1"

if [ ! -e "$filename" ]; then
    echo "Error: '$filename' does not exist."
    exit 1
fi

echo "File: $filename"

if [ -r "$filename" ]; then
    echo "Readable:   Yes"
else
    echo "Readable:   No"
fi

if [ -w "$filename" ]; then
    echo "Writable:   Yes"
else
    echo "Writable:   No"
fi

if [ -x "$filename" ]; then
    echo "Executable: Yes"
else
    echo "Executable: No"
fi
