#!/bin/bash
i=0
while [ $i -lt 1 ]
do
    echo -e "ala\nma\nkota\ni psa \n       malego" | ./client.py&
    i=$((i+1))
    
done