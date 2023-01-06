#!/bin/python3
from os import close, getpid, read
import sys
from time import sleep

name = sys.argv[0]
try:
    pid = getpid()
except Exception as e:
    print(name+": ", end = "")
    print(e)
    exit(-1)
filename = "wy_"+str(pid)+".txt"
# print("Konsument: " + str(pid))

# try:
#     with open("result/"+filename, 'a') as file:
#         pass
# except Exception as e:
# print(name+": ", end = "")    
# print(e)
#     exit(-1)

des = 4
while True:
    try:
        x = read(des,1)
    except Exception as e:
        print(name+": ", end = "")
        print(e)
        exit(-1)
    if len(x) == 0:
        break
    try:
        with open("result/"+filename, 'a') as file:
            file.write(x.decode("utf-8"))
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