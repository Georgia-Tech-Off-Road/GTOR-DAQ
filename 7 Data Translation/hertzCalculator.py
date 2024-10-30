import statistics
import os
import shutil 
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

originalDirectory = os.getcwd()
driveLetter = input("Please type the drive letter for your MechE network drive.\n")
os.chdir(driveLetter+":")
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
            print("IMPROPER FILENAME UR BAD KID")
while True:
    fileName = input("What file would you like to read from?\n")
    if os.path.isfile(os.getcwd() + "/"+ fileName):
        break
    else: 
        print("IMPROPER FILENAME UR BAD KID")
print("Copying data to temporary local file.")
shutil.copyfile(os.getcwd() + "/" + fileName, originalDirectory + "/temp.txt")
print("Opening local temp file.")
os.chdir(originalDirectory)
fileToRead = open("temp.txt","r")
print("Parsing local file.")
mode1(fileToRead)
print("Deleting local file.")
fileToRead.close()
os.remove(originalDirectory + "/temp.txt")
input("Hit enter to close window")
        
