#!/bin/bash
# Lab 3 Task 2A - Part 1: Find the Largest of Three Numbers

echo "=== Find the Largest of Three Numbers ==="
echo -n "Enter first number: "
read a
echo -n "Enter second number: "
read b
echo -n "Enter third number: "
read c

if [ $a -gt $b ] && [ $a -gt $c ]; then
    echo "The largest number is: $a"
elif [ $b -gt $c ]; then
    echo "The largest number is: $b"
else
    echo "The largest number is: $c"
fi

echo ""
echo "=== Display Fibonacci Series ==="
echo -n "Enter number of terms: "
read n

if [ $n -le 0 ]; then
    echo "Please enter a positive number."
elif [ $n -eq 1 ]; then
    echo "Fibonacci Series: 0"
else
    a=0
    b=1
    echo -n "Fibonacci Series: $a $b"
    count=2
    while [ $count -lt $n ]; do
        c=$((a + b))
        echo -n " $c"
        a=$b
        b=$c
        count=$((count + 1))
    done
    echo ""
fi
