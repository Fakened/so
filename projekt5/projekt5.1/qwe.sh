#!/bin/bash
i=0
while [ $i -lt 6 ]
do
    echo "ala\nma\nkota\ni psa\nmalego\n" | ./client.py&
    i=$((i+1))
    
done