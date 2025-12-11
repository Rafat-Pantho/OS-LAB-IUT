#!/usr/bin/bash

read number

if [[ $number -gt 0 ]] 
then        echo "positive"
elif [[ $number -lt 0 ]] 
then
        echo "Negative"
else
    echo "zero"
fi
    