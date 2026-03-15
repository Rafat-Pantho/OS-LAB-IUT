#!/bin/bash
# Lab 3 Task 1B - Part 1: Check Positive, Negative, or Zero

echo "=== Check Positive, Negative, or Zero ==="
echo -n "Enter a number: "
read n

if [ $n -gt 0 ]; then
    echo "The number is positive."
elif [ $n -lt 0 ]; then
    echo "The number is negative."
else
    echo "The number is zero."
fi

echo ""
echo "=== Sum of First N Natural Numbers ==="
echo -n "Enter a number N: "
read n

sum=0
i=1
while [ $i -le $n ]; do
    sum=$((sum + i))
    i=$((i + 1))
done

echo "Sum of first $n natural numbers = $sum"
