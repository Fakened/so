#!/bin/python3

from os import O_RDONLY, O_WRONLY, close, mkfifo, path, read, unlink
import os
import signal
from time import sleep
MAX = 100
def closeFifo(FifoName):
    unlink(FifoName)

def interruption(signum, frame):
    closeFifo(serverName)
    exit(0)

signal.signal(signal.SIGINT, interruption)


serverName = "server"
if not path.exists(serverName):
    mkfifo(serverName, mode=0o0606)

while True:
    serverFifo = os.open(serverName, O_RDONLY)
    sleep(2)
    temp = read(serverFifo, MAX)
    if len(temp) == 0:
        break
    temp = temp.decode("utf-8")
    temp = (temp.replace('[]', '')).split()
    clientName = temp[0]
    message = ''
    for i in range (1, len(temp)):
        message += temp[i] + ' '
    message = message.upper()
    message += ' '*(MAX-(len(message)))
    fd = os.open(clientName,  O_WRONLY)
    os.write(fd, message.encode())
    close(serverFifo)
closeFifo(serverName)
