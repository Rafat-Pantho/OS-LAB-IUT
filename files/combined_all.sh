#!/bin/bash
# Lab 3 Combined Task - All 6 Algorithms
# Even/Odd, Prime Number, Factorial, Fibonacci, Largest of Three, Leap Year

echo "================================================"
echo " Lab 3 - All Algorithms Combined"
echo "================================================"

# 1. Check Even or Odd
echo ""
echo "--- 1. Check Even or Odd ---"
echo -n "Enter a number: "
read n
r=$((n % 2))
if [ $r -eq 0 ]; then
    echo "$n is Even"
else
    echo "$n is Odd"
fi

# 2. Check Prime Number
echo ""
echo "--- 2. Check Prime Number ---"
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

# 3. Find the Factorial of a Number
echo ""
echo "--- 3. Factorial of a Number ---"
echo -n "Enter a number: "
read n
if [ $n -lt 0 ]; then
    echo "Factorial not defined for negative numbers."
else
    fact=1
    temp=$n
    while [ $temp -gt 0 ]; do
        fact=$((fact * temp))
        temp=$((temp - 1))
    done
    echo "$n! = $fact"
fi

# 4. Display Fibonacci Series
echo ""
echo "--- 4. Fibonacci Series ---"
echo -n "Enter number of terms: "
read n
if [ $n -le 0 ]; then
    echo "Enter a positive number."
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

# 5. Find the Largest of Three Numbers
echo ""
echo "--- 5. Largest of Three Numbers ---"
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

# 6. Check Leap Year
echo ""
echo "--- 6. Check Leap Year ---"
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
