#!/bin/python3
# type: ignore
import multiprocessing
from os import getpid, popen, wait
import random
import sys
import signal
from time import sleep
space = 0
processesR = []
processesW = []
w1 = multiprocessing.Semaphore(1)
w2 = multiprocessing.Semaphore(1)
w3 = multiprocessing.Semaphore(1)
w4 = multiprocessing.Semaphore(1)
sp = multiprocessing.Semaphore(1)
sc = multiprocessing.Semaphore(1)
mem = multiprocessing.Value('c')
lc = multiprocessing.Value('i', 0)
lp = multiprocessing.Value('i', 0)
def writer():
    while(1):
            w4.acquire()
            w2.acquire()
            if lp.value == 0 and lc.value == 0:
                lp.value += 1
                if lp.value == 1: sc.acquire()
                w2.release()
                sp.acquire()
                print("Writer "+str(getpid())+" come to library")
                mem.value = random.randint(97,122)
                sleep(0.1)
                sp.release()
                w2.acquire()
                lp.value -= 1
                if lp.value == 0: sc.release()
            w2.release()
            w4.release()

def reader():
    while(1):
        w4.acquire()
        w3.acquire()
        sc.acquire()
        w1.acquire()
        if lp.value == 0 and lc.value < space:
            lc.value +=1
            if lc.value == 1 : sp.acquire()
            w1.release()
            sc.release()
            w3.release()
            x = mem.value
            print("Reader "+str(getpid())+" "+x.decode("utf-8")+ " "+str(lc.value))
            sleep(0.1)
            w1.acquire
            lc.value -= 1
            if lc.value == 0: sp.release()
        w1.release()
        w4.release()

#------------------------------------------------------------------------------------------------------------
#------------------------------------------------------------------------------------------------------------
def interruption(signum, frame):
    for proces in processesR:
        proces.terminate()
    for proces in processesW:
        proces.terminate()
    exit(0)

signal.signal(signal.SIGINT, interruption)

# name = sys.argv[0]
# # print(str(getpid()))
# if len(sys.argv) != 4:
#     print("Nieodpowiednia liczba argumentów")
#     exit(-1)

# if int(sys.argv[1]) <= 0 or int(sys.argv[2]) <= 0 or int(sys.argv[3]) < 0:
#     print("Wartosci argumentow sa nieodpowiednie")
#     exit(-1)

# numOfWriters = int(sys.argv[1])
# numOfReaders = int(sys.argv[2])
# space = int(sys.argv[3])
# try:
#     process = popen('ps ux | wc -l', 'r')
#     processLimit = popen('ulimit -p', 'r')
# except Exception as e:
#     print(name+": ", end = "")
#     print(e)
#     exit(-1)

# if int(process.readline())-4 + numOfWriters + numOfReaders> int(processLimit.readline()):
#     print("Przekroczono limit procesow")
#     exit(-1)
# try:
#     process.close()
#     processLimit.close()
# except Exception as e:
#     print(name+": ", end = "")
#     print(e)
#     exit(-1)


numOfReaders = 1
numOfWriters = 1
space = 1

processesR = [multiprocessing.Process(target=reader) for i in range(numOfReaders)]
processesW = [multiprocessing.Process(target=writer) for i in range(numOfWriters)]

print(processesW)
print(processesR)

for proces in processesR:
    proces.start()
for proces in processesW:
    proces.start()

wait()




