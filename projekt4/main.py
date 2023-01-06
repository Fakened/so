#!/bin/python3
from os import close, dup, dup2, execl, fork, getpid, pipe, popen, system, waitpid
import sys
from time import sleep

name = sys.argv[0]
# print(str(getpid()))
if len(sys.argv) != 4:
    print("Nieodpowiednia liczba argumentów")
    exit(-1)

if int(sys.argv[1]) <= 0 or int(sys.argv[2]) <= 0 or int(sys.argv[3]) < 0:
    print("Wartosci argumentow sa nieodpowiednie")
    exit(-1)

consumer = int(sys.argv[1])
producer = int(sys.argv[2])
chars = sys.argv[3]
try:
    system('rm -f result/*.txt')
    process = popen('ps ux | wc -l', 'r')
    processLimit = popen('ulimit -p', 'r')
except Exception as e:
    print(name+": ", end = "")
    print(e)
    exit(-1)

if int(process.readline())-4 + consumer + producer> int(processLimit.readline()):
    print("Przekroczono limit procesow")
    exit(-1)
try:
    process.close()
    processLimit.close()
except Exception as e:
    print(name+": ", end = "")
    print(e)
    exit(-1)
try:
    r,w = pipe()
except Exception as e:
    print(name+": ", end = "")
    print(e)
    exit(-1)
i = 0

for i in range(producer):
    try:
        pid = fork()
    except Exception as e:
        print(name+": ", end = "")
        print(e)
        exit(-1)
    if pid == 0:
        try:
            close(r)
            dup2(w, 3)
            close(w)
            execl('producer.py', ' ', chars)
        except Exception as e:
            print(name+": ", end = "")
            print(e)
            exit(-1)
try:
    close(w)
except Exception as e:
    print(name+": ", end = "")
    print(e)
    exit(-1)

lastPid = 0
i = 0

for i in range(consumer):
    try:
        pid = fork()
    except Exception as e:
        print(name+": ", end = "")
        print(e)
        exit(-1)
    if pid == 0:
        try:
            dup2(r, 4)
            close(r)
            execl('consumer.py', ' ')
        except Exception as e:
            print(name+": ", end = "")
            print(e)
            exit(-1)
    else:
        lastPid = pid

try:
    waitpid(lastPid, 0)
    close(r)
except Exception as e:
    print(name+": ", end = "")
    print(e)
    exit(-1)
exit(0)