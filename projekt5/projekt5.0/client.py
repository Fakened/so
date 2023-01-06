#!/bin/python3
from os import O_RDONLY, O_WRONLY, getpid, mkfifo, path, close, read, unlink, write
from time import sleep
import os
import signal
MAX = 100

serverFd = os.open('server',  O_WRONLY)
clientName = 'clientFifo'
client_id = getpid()
clientFifoName = clientName + str(client_id)

if not path.exists(clientFifoName):
        mkfifo(clientFifoName, 0o666)

def closeFifo(FifoName):
    unlink(FifoName)

def interruption(signum, frame):
    closeFifo(clientFifoName)
    exit(0)
signal.signal(signal.SIGINT, interruption)

while True:
    message = clientFifoName
    temp = input()
    message +=' '+temp
    message += ' '*(MAX-(len(message)))
    asd = write(serverFd, message.encode())
    clientFd = os.open(clientFifoName, O_RDONLY)
    temp = read(clientFd, 100000)
    print(temp.decode("utf-8"))
    close(clientFd)
