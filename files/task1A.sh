#!/bin/bash
# Lab 3 Task 1A - Part 1: Check Even or Odd

echo "=== Check Even or Odd ==="
echo -n "Enter a number: "
read n

r=$((n % 2))
if [ $r -eq 0 ]; then
    echo "$n is Even"
else
    echo "$n is Odd"
fi

echo ""
echo "=== Check Prime Number ==="
echo -n "Enter a number: "
read n

if [ $n -le 1 ]; then
    echo "$n is NOT a prime number"
else
    is_prime=1
    i=2
    half=$((n / 2))
    while [ $i -le $half ]; do
        r=$((n % i))
        if [ $r -eq 0 ]; then
            is_prime=0
            break
        fi
        i=$((i + 1))
    done

    if [ $is_prime -eq 1 ]; then
        echo "$n is a Prime number"
    else
        echo "$n is NOT a prime number"
    fi
fi
