import os


class Sensor:
    def __init__(self, index, dataType, name, pollingRate):
        self.index = int(index)
        self.dataType = dataType
        self.name = name
        self.pollingRate = int(pollingRate)

    def __repr__(self):
        return f"Sensor(index={self.index}, dataType='{self.dataType}', name='{self.name}', pollingRate={self.pollingRate})"


sensorList = []
maxPr = 0
maxIn = 0
currTime = 0
tempTime = 0
numTeeth = 1 #NEED A A WAY TO FIND NUM TEETH
findAvg = False
anaStart = 0
anaEnd = 0
avg_value = 0

def process_analog_sensor(sensor, dataBuffer, start, end):

    sum_values = sum(float(dataBuffer[i][sensor.index + 1]) for i in range(start, end))
    average_value = sum_values / (end-start)
    print(f"New Analog Average: {average_value}.")
    return average_value

def process_digital_sensor(sensor, current_value, last_value, last_time, current_time):
    time_diff = current_time - last_time  # Calculate time difference using timestamps

    if last_value == 0 and current_value == 1:
        if time_diff > 0:
            rpm = ((60 / numTeeth) / time_diff)  # RPM calculation based on Rotations Per Minute !!!
            return rpm, current_time  # Return the current timestamp instead of fileCount
    return None, None

# Read config file
file = open("Config/config.txt")
header = file.readline()  # Read header
data = file.readlines()  # Read sensor data



for line in data:
    lineList = line.strip().split(",")
    sensorList.append(Sensor(lineList[0], lineList[1], lineList[2], lineList[3]))
    if int(lineList[3]) > maxPr:
        maxPr = int(lineList[3])
        maxIn = lineList[0]
file.close()

last_values = [0] * len(sensorList)  # Initialize to match the number of sensors
last_times = [0] * len(sensorList)

print(len(sensorList))  # Print number of sensors
dataBuffer = [[0 for _ in range(len(sensorList) + 1)] for _ in range(maxPr)]  # Fixed dataBuffer initialization

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

inFile = open(fileName)
initTimestamp = inFile.readline()  # Read first line (timestamp)
newData = inFile.readlines()  # Read remaining lines (data)

# Create counters for each sensor based on their polling rate
counterList = []
for sensor in sensorList:
    counterList.append([0, sensor.pollingRate])

print(counterList)

outfile = open(fileName + "output", "w")
fileCount = 0
andrewsBool = False

# Process each data entry


for entry in newData:
    entryList = entry.strip().split(",")
    dataBuffer[fileCount][0] = float(float(fileCount) / float(maxPr))  # Timestamp based on buffer

    for i, sensor in enumerate(sensorList):
        #Setting time
        timeSensor = 1/sensor.pollingRate
        if len(entryList) > i + 2:  # Ensure there are enough elements in entryList
            current_value = entryList[i + 2].strip()  # The current value from the input file, with whitespace removed
        else:
            print(f"Warning: Not enough data for sensor {sensor.name}. Skipping entry.")
            continue  # Skip this iteration if there's not enough data

        if sensor.pollingRate < maxPr:  #sensor's polling rate is less than max
            if (currTime + timeSensor) < dataBuffer[fileCount][0]: #if the current time + next increment of the sensor < buffer
                currTime += timeSensor #update current time by the next polling rate's increment
                print(f"The current time for the sensor is {currTime}")
        else: #sensor is max polling rate so it reads or slower sensor is being updated
            currTime += timeSensor
            print(f"The current time for the {sensor.name} sensor is {currTime}")
            findAvg = True
            anaEnd = fileCount #shows where the sensor gets updated so avg can be found


        if sensor.dataType == 'analog':
            # Process analog sensor
            if findAvg:
                avg_value = process_analog_sensor(sensor, dataBuffer, anaStart, anaEnd)
                findAvg = False
            dataBuffer[fileCount][i + 1] = avg_value
        elif sensor.dataType == 'digital':
            try:
                # Convert current_value to an integer after stripping spaces
                rpm, time_diff = process_digital_sensor(sensor, int(current_value), last_values[i], last_times[i],
                                                        currTime)
                if rpm is not None:
                    dataBuffer[fileCount][i + 1] = rpm
                last_values[i] = int(current_value)  # Update the last value for this sensor
                last_times[i] = currTime  # Update the last time for this sensor
            except ValueError as e:
                print(f"Error processing digital sensor {sensor.name} at index {i}: {e}")
                print(f"Current value: {current_value}")
                continue  # Skip this iteration if there's an error with the data

        counterList[i][0] += 1  # Increment the polling counter for each sensor

    print(dataBuffer[fileCount])  # Print current buffer line
    print(fileCount)

    fileCount += 1


print(dataBuffer[0:100])  # Print first 100 rows of data buffer

# Write data buffer to file
test = open("test.txt", "w")
for i in range(len(dataBuffer)):
    test.write(str(dataBuffer[i]) + "\n")
test.close()
