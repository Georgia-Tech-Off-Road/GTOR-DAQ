import statistics
import os
import shutil
from DataDownloader import DataDownloader
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
    print(str(statistics.pvariance(timeStampDataPoints)) + " : pVariance")
    print(str(statistics.mean(timeStampDataPoints)) + " : mean")
    print(str(len(timeStampDataPoints))+ " : numSeconds")
    print(str(minRefresh) + ": minRefreshRate")

fileToRead = DataDownloader.DownloadDataFile()
print("Parsing local file.")
mode1(fileToRead)
print("Deleting local file.")
fileToRead.close()
os.remove("temp.txt")
input("Hit enter to close window")
        
