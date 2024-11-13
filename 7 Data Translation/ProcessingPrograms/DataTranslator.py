#I still lose the last final bit of an average I think (like a hundredth of a second)kinda a good thing tho so you don't pretend you have proepr averages when you don't
import os
from DataDownloader import DataDownloader
import math
import importlib

os.chdir("./")

#imports all data libraries from ConversionLibrary folder
for fileName in os.listdir("ConversionLibraries"):
        if fileName.endswith('.py') and not fileName.startswith('__'):
            moduleName = fileName[:-3]  # Remove .py extension
            module = importlib.import_module(f'ConversionLibraries.{moduleName}')
            globals()[moduleName] = module

class Sensor:
    def __init__(self, index, dataType, name, pollingRate, numTeeth):
        #index of sensor in data file
        self.index = int(index)
        #datatype of sensor options are analog or digital
        self.dataType = dataType
        #name of sensor
        self.name = name
        #sensor polling rate
        self.pollingRate = int(pollingRate)
        #number of teeth of sensor (only applies to halleffect sensors but is still present for all sensors)
        self.numTeeth = int(numTeeth)
        #sum, num data points, last time they were averaged, timesAvged for analog sensors
        self.sumList = [0, 0, 0, 0]
        #last value, time of last value
        self.digitalList = [0, 0]
        #list of rpm values with their correpsonding time stamps
        self.rpmValueList = []

    def __repr__(self):
        return f"Sensor(dataType='{self.dataType}', name='{self.name}', pollingRate={self.pollingRate})"
def translateData (inputFilePath, progressBar, progressBarPage, label, parentPage):
        #list of sensors
        sensorList = []
        #maximum polling rate of all analog sensors
        prLCM = 0
        #name of last sensor with highest polling rate
        timeController = ""
        #index of last analog sensor
        lastAnalogIndex = 0

        # Read config file
        configFileName = os.path.basename(inputFilePath)+"Config.txt"
        file = open("Configs/" + configFileName)
        # Read header
        header = file.readline()
        # Read sensor data
        data = file.readlines()

        #loops through config file and creates sensor list
        for line in data:
            #strips and splits line
            lineList = line.strip().split(",")
            try:
                #tries to create a new sensor
                sensorList.append(Sensor(lineList[0], lineList[1], lineList[2], lineList[3], lineList[4]))
            except:
                #throws an error if sensor creation fails
                print("Inproper config! Please find and fix the following line\n" + str(line))
            if lineList[1] == "analog":
                #make prLCM start at the first polling rate value
                if prLCM == 0:
                    prLCM = int(lineList[3])
                prLCM = math.lcm(int(lineList[3]), prLCM)
                timeController = lineList[2]
                lastAnalogIndex = int(lineList[0])
        file.close()
        #create a data buffer the width of the sensor list plus a time stamp and the length of prLCM
        dataBuffer = [[0 for _ in range(len(sensorList) + 1)] for _ in range(prLCM)]
        #opens input data file
        inFile = open(inputFilePath)
        #opens output file
        outfile = open("output.txt", "w")
        #whether this is the first run
        firstRun = True
        #time scalar (multiplies the period of the largest PR by num dataBuffers written)
        timeScalar = 1
        #counter to handle final partial buffer flush
        currentRowForBuffer = 0
        # Process each data entry
        #progress bar counter
        counter = 0
        for line in inFile:
            lineList = line.strip().split(",")
            #if first run set baseTime to microsecond time
            if firstRun:
                baseTime = int(lineList[1])
                for sensor in sensorList:
                    sensor.sumList[2] = baseTime
                firstRun = False
            #get current time
            currentTime = lineList[1]
            #for i in range num sensors (could just be sensor list but still)
            for i in range(0,len(lineList[2:])):
                #get current sensor
                currentSensor = sensorList[i]
                #set current sensor based on sensor index in config file (IF THIS FAILS THEN THINGS BREAK)
                for sensor in sensorList:
                    if sensor.index == i + 2:
                        currentSensor = sensor
                if currentSensor.dataType == "analog":
                    #if current time is greater than last time recorded + period in microseconds
                    if float(currentTime) > float(currentSensor.sumList[2]) + float((1/currentSensor.pollingRate)*10**6):
                        #set entry to average of values in time frame
                        avg = float(currentSensor.sumList[0]/currentSensor.sumList[1])
                        #propogate average value from last entry to last entry + num entries of lower polling rate sensor to max polling rate sensor
                        for j in range(int(currentSensor.sumList[3]*(prLCM/currentSensor.pollingRate)), int((currentSensor.sumList[3] + 1)*(prLCM/currentSensor.pollingRate))):
                            #have to subtract one since there's only one seconds column in the output file
                            dataBuffer[j][currentSensor.index - 1] = avg
                            if currentSensor.name == timeController:
                                #propagate time along first column
                                dataBuffer[j][0] = round(1/currentSensor.pollingRate * timeScalar, 10)
                                timeScalar += 1
                            #if last analog sensor and dataBuffer is full, write data buffer to file and reset all num times averaged to 0
                            if i + 2 == lastAnalogIndex:
                                currentRowForBuffer = j
                                if j == prLCM - 1:
                                    for row in dataBuffer:
                                        outfile.write(str(row).replace('[','').replace(']','')+"\n")
                                    for sensor in sensorList:
                                        sensor.sumList[3] = 0
                        #update/reset all sumList values
                        currentSensor.sumList[0] = 0
                        currentSensor.sumList[1] = 0
                        currentSensor.sumList[2] = currentSensor.sumList[2] + float((1/currentSensor.pollingRate)*10**6)
                        currentSensor.sumList[3] += 1
                    #add entry to sumLists (prior logic runs on data before this)
                    currentSensor.sumList[0] =  currentSensor.sumList[0] + float(lineList[i + 2])
                    currentSensor.sumList[1] += 1
                #if digital compute current RPM and save it with the time stamp to the sensors RPMlist
                if currentSensor.dataType == "digital":
                    #if line list value has changed
                    if int(lineList[i + 2]) != currentSensor.digitalList[0]:
                        #if line list value is 1
                        if int(lineList[i + 2]) == 1:
                            #compute the stuff
                            numTeeth = currentSensor.numTeeth
                            timeDif = int(currentTime) - int(currentSensor.digitalList[1])
                            RPM = ((1/timeDif) * (10**6)) * (1/numTeeth) * 60
                            currentSensor.digitalList[0] = int(lineList[i + 2])
                            currentSensor.digitalList[1] = int(currentTime)
                            #subtract 
                            currentSensor.rpmValueList.append([RPM, int(currentTime) - baseTime])
                        #otherwise just update last sensor binary value entry
                        else:
                            currentSensor.digitalList[0] = int(lineList[i + 2])
                if counter == 1000000:
                        percentage = round((os.path.getsize(outfile.name)/(os.path.getsize(inFile.name) * (prLCM/20000)) * 75), 2)
                        progressBar["value"] = percentage
                        label.config(text = str("Analog Averaging Progress " + str(round(percentage,2)) + "%"))
                        label.pack()
                        progressBar.pack()
                        progressBarPage.update()
                        counter = 0
                counter += 1
        #write any lines remaining in file
        for i in range(0, len(dataBuffer)):
            if i == currentRowForBuffer:
                break
            outfile.write(str(dataBuffer[i]).replace('[','').replace(']','')+"\n")
        inFile.close()
        outfile.close()
        outFile = open("output.txt", "r")
        finalOutFile = open(os.path.basename(str(ininputFilePath + ".csv"), "w")
        counter = 0
        #for line in output.txt plug in RPM value and save to final Output file
        for line in outFile:
            lineList = line.strip().split(",")
            for sensor in sensorList:
                if sensor.dataType == "digital":
                    #if 1 or 0 entries just type zero (since data collection is only stopped when car is stationary we can assume end rpm is 0)
                    if len(sensor.rpmValueList) <= 1:
                        lineList[sensor.index - 1] = '0'
                        continue
                    rpmValue = sensor.rpmValueList[0]
                    #if next time stamp less than current time then use that one instead
                    if float(lineList[0]) * (10 ** 6) > sensor.rpmValueList[1][1]:
                        sensor.rpmValueList.pop(0)
                        rpmValue = sensor.rpmValueList[0]
                    #have to subtract one since only one time stamp
                    lineList[sensor.index - 1] = str(rpmValue[0])
                #finds sensor type by name, may not be the best way to do this but itll be fineeeeee
                elif "brake pressure" in sensor.name:
                        lineList[sensor.index - 1] = str(BrakePressureSensor.convertBrakePressure(float(lineList[sensor.index - 1])))
            finalOutFile.write(",".join(lineList) + "\n")
            if counter == 1000000:
                    percentage = round((os.path.getsize(finalOutFile.name)/(os.path.getsize(outFile.name)*1.25) * 25 + 75), 2)
                    progressBar["value"] = percentage
                    label.config(text = str("RPM propogation and sensory library application " + str(round(percentage,2)) + "%"))
                    label.pack()
                    progressBar.pack()
                    progressBarPage.update()
                    counter = 0
            counter += 1
        finalOutFile.close()
        outFile.close()
        try:
            os.remove("output.txt")
        except:
            print("File deletion failed.")
        progressBarPage.destroy()
        parentPage.deiconify()

                    
            
                    
                    

