#!/bin/python3

import multiprocessing
from os import getpid, popen, wait
import random
import sys
from time import sleep
space = 0
writerSem = multiprocessing.Semaphore(1)
readerSem = multiprocessing.Semaphore(1)
mem = multiprocessing.Value('c')
lc = multiprocessing.Value('i', 0)
sp = multiprocessing.Value('i', 0)
def writer():
    while(1):
        writerSem.acquire()
        if sp.value - space == 0:
            print("Writer "+str(getpid())+" come to library")
            mem.value = random.randint(97,122)
            # print("Writer "+str(getpid())+" out of library")
            sleep(0.1)
        writerSem.release()

def reader():
    while(1):
        readerSem.acquire()
        if sp.value - 1 >= 0:
            sp.value -= 1
            lc.value += 1
            if lc.value == 1:
                writerSem.acquire()
            # print("Reader "+str(getpid())+" come to library")
            readerSem.release()
            x = mem.value
            print("Reader "+str(getpid())+" "+x.decode("utf-8")+ " "+str(sp.value))
            readerSem.acquire()
            # print("Reader "+str(getpid())+" out of library")
            lc.value -= 1
            if lc.value == 0:
                writerSem.release()
            sp.value += 1
            sleep(0.1)
        readerSem.release()




name = sys.argv[0]
# print(str(getpid()))
if len(sys.argv) != 4:
    print("Nieodpowiednia liczba argumentów")
    exit(-1)

if int(sys.argv[1]) <= 0 or int(sys.argv[2]) <= 0 or int(sys.argv[3]) < 0:
    print("Wartosci argumentow sa nieodpowiednie")
    exit(-1)

numOfWriters = int(sys.argv[1])
numOfReaders = int(sys.argv[2])
space = int(sys.argv[3])
try:
    process = popen('ps ux | wc -l', 'r')
    processLimit = popen('ulimit -p', 'r')
except Exception as e:
    print(name+": ", end = "")
    print(e)
    exit(-1)

if int(process.readline())-4 + numOfWriters + numOfReaders> int(processLimit.readline()):
    print("Przekroczono limit procesow")
    exit(-1)
try:
    process.close()
    processLimit.close()
except Exception as e:
    print(name+": ", end = "")
    print(e)
    exit(-1)

# numOfReaders = 1
# numOfWriters = 1
sp.value = space
for i in range(0, numOfReaders):
    p1 = multiprocessing.Process(target=reader)
    p1.start()

for i in range(0, numOfWriters):
    p2 = multiprocessing.Process(target=writer)
    p2.start()

wait()




