lastDigitalValues = []
lastAnalogValues =[]
#(index, name)
analogValueLocations =[]
#(index, other) or (index, RPM, (lastRPMValue, microsecondsForLastValue, numTeeth))
digitalValueLocations =[]
firstTime = True

#imports all data libraries from ConversionLibrary folder
os.chdir("./")
for fileName in os.listdir("ConversionLibraries"):
        if fileName.endswith('.py') and not fileName.startswith('__'):
            moduleName = fileName[:-3]  # Remove .py extension
            module = importlib.import_module(f'ConversionLibraries.{moduleName}')
            globals()[moduleName] = module

def initialSetup(configFileName):
    config = open(configFileName)
    #get rid of the header line
    config.readline()
    for line in config:
        line = line.strip().split(",")
        if line[1] == "digital":
            #this allows for the distinction between RPM focused binary data and something else like data from an i2c bus
            if "(RPM)" in line[2]:
                digitalValueLocations.append((int(line[0]), "RPM", (0, 0, int(line[4])))
            else:
                digitalValueLocations.append((int(line[0]), "other"))
        elif line[1] == "analog":
            analogValueLocations.append((int(line[1]), line[2]))

def computeNewValues(dataString):
    data = dataString.strip().split(",")
    if firstTime:
        for indexTuple in digitalValueLocations:
            lastDigitalValues.append((indexTuple[0], data[indexTuple[0]]))
        firstTime = False
    for indexTuple in analogValueLocations:
        if "brake pressure" in indexTuple[1]:
             data[indexTuple[0]] = str(BrakePressureSensor.convertBrakePressure(int(data[indexTuple[0]])))
    for indexTuple in digitalValueLocations:
        if indexTuple[1] == "RPM":
            if data[indexTuple[0]] == lastDigitalValues[indexTuple[0]]:
                data[indexTuple[0]] = str(indexTuple[2][0])
            else:
                lastDigitalValues[indexTuple[0]] = data[indexTuple[0]]
                timeDif = int(data[1]) - indexTuple[2][1]
                RPM = ((1/timeDif) * (10**6)) * (1/indexTuple[2][2]) * 60
                data[indexTuple[0]] = RPM
                str(indexTuple[2][0]) = RPM
                str(indexTuple[2][1]) = int(data[1])
    return data
                
    
            
