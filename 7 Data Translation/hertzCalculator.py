import statistics
import os
def mode1(fileToRead):
    minRefresh = 999999999
    timeStamps = []
    currentTimeStamp = ""
    currentTimeStampIndex = -1
    for line in fileToRead:
        line = line.split(",")[0]
        if len(line) > 5:
            if line != currentTimeStamp:
                timeStamps.append([line, 1])
                currentTimeStamp = line
                currentTimeStampIndex += 1
            else:
                timeStamps[currentTimeStampIndex][1] += 1
    print(timeStamps)
    timeStampDataPoints =[]
    timeStamps.pop(0)
    timeStamps.pop(0)
    timeStamps.pop(len(timeStamps) - 1)
    timeStamps.pop(len(timeStamps) - 1)
    for timeStamp in timeStamps:
        timeStampDataPoints.append(timeStamp[1])
        if timeStamp[1] < minRefresh:
            minRefresh = timeStamp[1]
    print(statistics.pvariance(timeStampDataPoints))
    print(statistics.mean(timeStampDataPoints))
    print(len(timeStampDataPoints))
    print(minRefresh)

def mode2(fileToRead):
    print("penis")
while True:
    print(os.listdir())
    fileName = input("Please type a file name\n")
    fileToRead = open(fileName,"r")
    mode = input("What mode would you like to use? (1 or 2)\n")
    if mode == "1":
        mode1(fileToRead)
    else:
        mode2(fileToRead)
    
