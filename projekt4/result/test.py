#!/bin/python3

from os import popen, system


howManyOut = popen('ls  | grep wy | wc -l').readline()
outName = popen('ls  | grep wy ').readlines()

i = 0 
outSize = 0
for i in range(int(howManyOut)):
    accName = outName[i].strip()
    cmd = "stat -c %s "+accName
    outSize += int(popen(cmd).readline())


howManyIn = popen('ls  | grep we | wc -l').readline()
inName = popen('ls  | grep we ').readlines()

i = 0 
inSize = 0
for i in range(int(howManyIn)):
    accName = inName[i].strip()
    cmd = "stat -c %s "+accName
    inSize += int(popen(cmd).readline())

print("Plikow wejsciowych: " + howManyIn.strip() + " suma wielkosci: " + str(inSize))
print("Plikow wyjsciowych: " + howManyOut.strip() + " suma wielkosci: " + str(outSize))
system('rm -f *.txt')