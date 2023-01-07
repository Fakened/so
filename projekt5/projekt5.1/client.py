#!/bin/python3
from os import O_RDONLY, O_WRONLY, getpid, mkfifo, path, close, read, unlink, write
from time import sleep
import os
import signal
MAX = 100
clientName = 'clientFifo'
try:
    client_id = getpid()
except Exception as e:
    print(e)
    exit(-1)
clientFifoName = clientName + str(client_id)

try:
    def closeFifo(FifoName):
        unlink(FifoName)
    serverFd = os.open('server',  O_WRONLY)
except Exception as e:
    print(e)
    exit(-1)
try:
    if not path.exists(clientFifoName):
            mkfifo(clientFifoName, 0o666)
except Exception as e:
    print(e)
    exit(-1)
try:
    def interruption(signum, frame):
        closeFifo(clientFifoName)
        exit(0)
    signal.signal(signal.SIGINT, interruption)
except Exception as e:
    print(e)
    closeFifo(clientFifoName)
    exit(-1)
try:
    message = ['', '']
    message[0] = clientFifoName
    print("maksymalna dlugosc wiadomosci:",MAX-len(message[0])-8)
    for i in range(5):
        temp = ''
        while len(temp) == 0 or len(temp)+len(message[0])+8 > MAX:
            temp = input()
            if len(temp) == 0 or len(temp)+len(message[0])+8 > MAX:
                print("Wiadmosc jest zbyt krotka lub dluga, wprowadz poprawna wiadomosc")
        message[1] = temp+' '*(MAX-len(message[0])-len(temp)-8)
        write(serverFd, str(message).encode())
        clientFd = os.open(clientFifoName, O_RDONLY)
        temp = read(clientFd, MAX)
        print(temp.decode("utf-8"))
        close(clientFd)
    closeFifo(clientFifoName)
    exit(1)
except Exception as e:
    print(e)
    closeFifo(clientFifoName)
    exit(-1)