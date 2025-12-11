#!/usr/bin/bash

# name="Vegapunk"
# echo "Hello $name"
# echo "Your path: $PATH"

num=10
if [ $num -gt 5 ]
then echo "$num is greater than 5"
else echo "$num is not greater than 5"
fi

num2=20
if [ $num2 -lt 15 ]
then echo greater
else echo less
fi

if [[ $num2 -lt 15 ]]
then echo greater
else echo less
fi