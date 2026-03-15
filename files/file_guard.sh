#!/bin/bash
# CSE4502 Operating Systems Lab
# Lab (Bash Scripting) 1A - Part A: File Guard (Conditionals)

if [ -z "$1" ]; then
    echo "Usage: $0 <filename>"
    exit 1
fi

filename="$1"

if [ -e "$filename" ]; then
    if [ -d "$filename" ]; then
        echo "'$filename' is a directory."
    elif [ -f "$filename" ]; then
        echo "'$filename' is a regular file."
    else
        echo "'$filename' exists but is neither a regular file nor a directory."
    fi
else
    echo "Error: '$filename' does not exist."
fi
