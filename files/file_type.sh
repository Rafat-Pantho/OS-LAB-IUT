#!/bin/bash
# CSE4502 Operating Systems Lab
# Lab (Bash Scripting) 2B - Part A: File Type Identifier

if [ -z "$1" ]; then
    echo "Usage: $0 <filename>"
    exit 1
fi

filename="$1"
ext="${filename##*.}"
ext_lower=$(echo "$ext" | tr '[:upper:]' '[:lower:]')

case "$ext_lower" in
    txt|text)
        echo "File type: Text File"
        ;;
    sh|bash)
        echo "File type: Shell Script"
        ;;
    c)
        echo "File type: C Source File"
        ;;
    cpp|cc|cxx)
        echo "File type: C++ Source File"
        ;;
    h|hpp)
        echo "File type: Header File"
        ;;
    py)
        echo "File type: Python Script"
        ;;
    java)
        echo "File type: Java Source File"
        ;;
    jpg|jpeg|png|gif|bmp|webp)
        echo "File type: Image File"
        ;;
    mp3|wav|ogg|flac)
        echo "File type: Audio File"
        ;;
    mp4|avi|mkv|mov)
        echo "File type: Video File"
        ;;
    pdf)
        echo "File type: PDF Document"
        ;;
    zip|tar|gz|bz2|xz)
        echo "File type: Archive File"
        ;;
    html|htm)
        echo "File type: HTML File"
        ;;
    css)
        echo "File type: CSS File"
        ;;
    js)
        echo "File type: JavaScript File"
        ;;
    *)
        echo "File type: Unknown File Type (.${ext})"
        ;;
esac
