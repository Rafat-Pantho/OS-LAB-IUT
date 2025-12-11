#!/bin/bash

read n

declare -a arr

for ((i=1; i<=n; i++));do
    read element
    arr+=($element)
done


read -p "Enter the element to search: " target

found=0
position=0

for ((i=0; i<n; i++))
do
    if [ "${arr[$i]}" -eq "$target" ]; then
        position=$((i + 1))
        echo "Element $target found at position $position"
        found=1
        break
    fi
done

# 5. If not found, display an appropriate message
if [ $found -eq 0 ]; then
    echo "Element $target not found in the array"
fi