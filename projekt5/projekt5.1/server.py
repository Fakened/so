#!/bin/python3

from os import O_RDONLY, O_WRONLY, close, mkfifo, path, read, unlink
import os
import signal
from time import sleep
MAX = 100
serverName = "server"
try:
    def closeFifo(FifoName):
        unlink(FifoName)

except Exception as e:
    print(e)
    exit(-1)
try:
    
    def interruption(signum, frame):
        closeFifo(serverName)
        exit(0)

    signal.signal(signal.SIGINT, interruption)

    if not path.exists(serverName):
        mkfifo(serverName, mode=0o0606)

    serverFifo = os.open(serverName, O_RDONLY)
    # sleep(3)
    while True:
        temp = read(serverFifo, MAX)
        if len(temp) == 0:
            break
        temp = temp.decode("utf-8")
        temp = (temp.replace("[", '', 1))
        temp = (temp.replace("]", '', -1)).split(', ')
        temp[0] = temp[0].replace("'", '')
        temp[1] = temp[1].replace("'", '', 1)
        temp[1] = temp[1].replace("'", '', -1)
        clientName = temp[0]
        message = temp[1]
        message = message.upper()
        message += ' '*(MAX-(len(message)))
        print("Odsylam wiadomosc do:",clientName,"\nO tresci:", message)
        fd = os.open(clientName,  O_WRONLY)
        os.write(fd, message.encode())
    closeFifo(serverName)
    exit(1)
except Exception as e:
    print(e)
    closeFifo(serverName)
    exit(-1)
