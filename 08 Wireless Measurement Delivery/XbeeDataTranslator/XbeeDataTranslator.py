import os
import importlib


#(index, name)
analogValueLocations =[]


#imports all data libraries from ConversionLibrary folder
os.chdir("./")
for fileName in os.listdir("ConversionLibraries"):
        if fileName.endswith('.py') and not fileName.startswith('__'):
            moduleName = fileName[:-3]  # Remove .py extension
            module = importlib.import_module(f'ConversionLibraries.{moduleName}')
            globals()[moduleName] = module

def initialSetup(configFileName):
    global analogValueLocations
    
    config = open(configFileName)
    #get rid of the header line
    config.readline()
    for line in config:
        line = line.strip().split(",")
        if line[1] == "analog":
            analogValueLocations.append([int(line[0]), line[2]])

def computeNewValues(dataString):
    global analogValueLocations

    lastAnalogValues =[]
    
    data = dataString.strip().split(",")
    for indexTuple in analogValueLocations:
        if "brake pressure" in indexTuple[1]:
             data[indexTuple[0]] = str(BrakePressureSensor.convertBrakePressure(int(data[indexTuple[0]])))
    stringData = ",".join(data)
    return stringData
                
    
            
