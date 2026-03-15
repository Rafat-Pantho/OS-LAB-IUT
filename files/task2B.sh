#!/bin/bash
# Lab 3 Task 2B - Part 1: Check Whether a Year is a Leap Year

echo "=== Check Leap Year ==="
echo -n "Enter a year: "
read y

if [ $((y % 400)) -eq 0 ]; then
    echo "$y is a Leap Year."
elif [ $((y % 100)) -eq 0 ]; then
    echo "$y is NOT a Leap Year."
elif [ $((y % 4)) -eq 0 ]; then
    echo "$y is a Leap Year."
else
    echo "$y is NOT a Leap Year."
fi

echo ""
echo "=== Find the Factorial of a Number ==="
echo -n "Enter a number: "
read n

if [ $n -lt 0 ]; then
    echo "Factorial is not defined for negative numbers."
else
    fact=1
    while [ $n -gt 0 ]; do
        fact=$((fact * n))
        n=$((n - 1))
    done
    echo "Factorial = $fact"
fi
