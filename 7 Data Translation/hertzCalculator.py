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

while True:
    driveLetter = input("Please type the drive letter for your MechE network drive.\n")
os.chdir(driveLetter+":")
directory = os.getcwd()
while True:
    directory = os.getcwd()
    print(os.listdir())  # List files in the directory
    newFolder = input("Please type the name of the folder you would like to navigate to. Type 	STOP when you've found the data file you're looking for.\n")
    if newFolder == "STOP":
        break
    else:
        try:
            full_path = os.path.join(directory, newFolder)
            os.chdir(full_path)
        except:
            print("INPROPER FILENAME UR BAD KID")

    fileName = input("What file would you like to read from?\n")
    fileToRead = open(fileName,"r")
    mode1(fileToRead)
    
