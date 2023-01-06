#!/bin/python3
from os import close, getpid, write
import sys
import random
from time import sleep

name = sys.argv[0]

if len(sys.argv) != 2:
    print("Nieodpowiednia liczba argumentów")
    exit(-1)

try:
    pid = getpid()
except Exception as e:
    print(name+": ", end = "")
    print(e)
    exit(-1)
fileName = "we_"+str(pid)+".txt"
# print("producent: " + str(pid))
des = 3
i = 0
for i in range(int(sys.argv[1])):
    randchar = chr(random.randint(97,122))
    try:
        with open("result/"+fileName, 'a') as file:
            file.write(str(randchar))
    except Exception as e:
        print(name+": ", end = "")
        print(e)
        exit(-1)
    randchar = str.encode(randchar)
    try:
        write(des, randchar)
    except Exception as e:
        print(name+": ", end = "")
        print(e)
        exit(-1)

try:
    close(des)
except Exception as e:
    print(name+": ", end = "")
    print(e)
    exit(-1)

exit(0)
