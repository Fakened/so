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
    if not path.exists(clientFifoName):
            mkfifo(clientFifoName, 0o666)

    def interruption(signum, frame):
        closeFifo(clientFifoName)
        exit(0)
    signal.signal(signal.SIGINT, interruption)
    message = clientFifoName
    temp = ''
    while len(temp) == 0 or len(temp) > MAX:
        temp = input()
    print(temp)
    message +=' '+temp
    message += ' '*(MAX-(len(message)))
    write(serverFd, message.encode())
except Exception as e:
    print(e)
    exit(-1)
try:
    clientFd = os.open(clientFifoName, O_RDONLY)
    temp = read(clientFd, MAX)
    print(temp.decode("utf-8"))
    close(clientFd)
    closeFifo(clientFifoName)
except Exception as e:
    print(e)
    closeFifo(clientFifoName)
    exit(-1)
