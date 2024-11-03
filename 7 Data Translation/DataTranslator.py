import os
from DataDownloader import DataDownloader


class Sensor:
    def __init__(self, index, dataType, name, pollingRate, numTeeth):
        self.index = int(index)
        self.dataType = dataType
        self.name = name
        self.pollingRate = int(pollingRate)
        self.numTeeth = int(numTeeth)
        #sum, num data points, last time they were averaged, timesAvged for analog sensors
        self.sumList = [0, 0, 0, 0]
        #last value, time of last value
        self.digitalList = [0, 0]
        #list of rpm values with their correpsonding time stamps
        self.rpmValueList = []

    def __repr__(self):
        return f"Sensor(dataType='{self.dataType}', name='{self.name}', pollingRate={self.pollingRate})"


sensorList = []
maxPr = 0
timeController = ""


# Read config file
file = open("Config/config.txt")
header = file.readline()  # Read header
data = file.readlines()  # Read sensor data



for line in data:
    lineList = line.strip().split(",")
    try:
        sensorList.append(Sensor(lineList[0], lineList[1], lineList[2], lineList[3], lineList[4]))
    except:
        print("Inproper config! Please find and fix the following line\n" + str(line))
    if int(lineList[3]) >= maxPr and lineList[1] == "analog":
        maxPr = int(lineList[3])
        timeController = lineList[2]
file.close()

dataBuffer = [[0 for _ in range(len(sensorList) + 1)] for _ in range(maxPr)]  # Fixed dataBuffer initialization
#inFile = DataDownloader.DownloadDataFile()
inFile = open("temp.txt")
# Create counters for each sensor based on their polling rate
outfile = open("output.txt", "w")
# Process each data entry
baseTime = 0
firstRun = True
timeScalar = 1
progressBarCounter = 0
for line in inFile:
    lineList = line.strip().split(",")
    if firstRun:
        baseTime = int(lineList[1])
        for sensor in sensorList:
            sensor.sumList[2] = baseTime
        firstRun = False
    currentTime = lineList[1]
    for i in range(0,len(lineList[2:])):
        currentSensor = sensorList[i]
        for sensor in sensorList:
            if sensor.index == i + 2:
                currentSensor = sensor
        if currentSensor.dataType == "analog":
            if int(currentTime) > int(currentSensor.sumList[2]) + float(1/(currentSensor.pollingRate)*10**6):
                avg = float(currentSensor.sumList[0]/currentSensor.sumList[1])
                for j in range(int(currentSensor.sumList[3]*(maxPr/currentSensor.pollingRate)), int(currentSensor.sumList[3]*(maxPr/currentSensor.pollingRate) + maxPr/currentSensor.pollingRate)):
                    #have to subtract one since there's only one seconds column in the output file
                    dataBuffer[j][currentSensor.index - 1] =  avg
                    if currentSensor.pollingRate == maxPr and currentSensor.name == timeController:
                        dataBuffer[j][0] = round(1/currentSensor.pollingRate * timeScalar, 10)
                        timeScalar += 1
                    if i == len(lineList[2:]) - 1 and j == maxPr - 1:
                        for row in dataBuffer:
                            outfile.write(str(row).replace('[','').replace(']','')+"\n")
                        for sensor in sensorList:
                            sensor.sumList[3] = 0
                currentSensor.sumList[0] = 0
                currentSensor.sumList[1] = 0
                currentSensor.sumList[2] = currentTime
                currentSensor.sumList[3] += 1
            currentSensor.sumList[0] = currentSensor.sumList[0] + float(lineList[i + 2])
            currentSensor.sumList[1] += 1
        if currentSensor.dataType == "digital":
            if int(lineList[i + 2]) != currentSensor.digitalList[0]:
                if int(lineList[i + 2]) == 1:
                    #print(lineList[i + 2])
                    #print(currentSensor.digitalList[0])
                    numTeeth = currentSensor.numTeeth
                    timeDif = int(currentTime) - int(currentSensor.digitalList[1])
                    RPM = ((1/timeDif) * (10**6)) * (1/numTeeth) * 60
                    #print(numTeeth)
                    #print(timeDif)
                    #print(RPM)
                    currentSensor.digitalList[0] = int(lineList[i + 2])
                    currentSensor.digitalList[1] = int(currentTime)
                    currentSensor.rpmValueList.append([RPM, timeDif])
                else:
                    currentSensor.digitalList[0] = int(lineList[i + 2])
    if progressBarCounter == 1000000:
        #replace 25000 with average polling rate found by hz calculator in future
        print(str(round(((os.path.getsize(outfile.name))/(os.path.getsize(inFile.name) * maxPr/30000) * 100), 2)) + "% done with analog averaging!")
        progressBarCounter = 0
    progressBarCounter +=1
inFile.close()
os.remove("temp.txt")
outfile.close()
print("Finished analog averaging!")
outFile = open("output.txt", "r")
finalOutFile = open("finalOutput.txt", "w")
progressBarCounter = 0
for line in outFile:
    lineList = line.strip().split(",")
    for sensor in sensorList:
        if sensor.dataType == "digital":
            if len(sensor.rpmValueList) == 0:
                lineList[sensor.index - 1] = '0'
                continue
            rpmValue = sensor.rpmValueList[0]
            if float(lineList[0]) * (10 ** 6) > rpmValue[1]:
                sensor.rpmValueList.pop(0)
                #have to subtract one since only one time stamp  
            lineList[sensor.index - 1] = str(rpmValue[0])
    finalOutFile.write(",".join(lineList) + "\n")
    if progressBarCounter == 1000000:
        print(str(round((os.path.getsize(finalOutFile.name)/os.path.getsize(outFile.name) * 100), 8)) + "% done with rpm propagation!")
        progressBarCounter = 0
    progressBarCounter +=1
finalOutFile.close()
outFile.close()
os.remove("output.txt")

                    
            
    
            
            
