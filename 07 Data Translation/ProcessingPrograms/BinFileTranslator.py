def ogBinConverter(input_file_name,chosePath,outputPath,settingsData):
    import struct
    import os

    # Define the format for the binary data (including padding to match Arduino's structure size)
    data_format = "Q L 4f 4i 1f"  # Add 4 bytes of padding (4x)
    data_size = struct.calcsize(data_format)
    output_file_name = os.path.splitext(os.path.basename(input_file_name))[0] + ".txt"
    # Check if the user has selected a directory
    print(settingsData)
    if chosePath:
        outfile = os.path.normpath(os.path.join(outputPath, output_file_name))
    else:
        outfile = output_file_name
    try:
        with open(input_file_name, "rb") as binary_file, open(outfile, "w") as text_file:
            print("Writing data...........")
            # Read and process binary data
            while chunk := binary_file.read(data_size):
                if len(chunk) != data_size:
                    print("Incomplete data chunk encountered. Skipping.")
                    continue

                # Unpack the binary data
                data = struct.unpack(data_format, chunk)

                # Write the unpacked data as a comma-separated line
                text_file.write(",".join(map(str, data)) + "\n")

        print(f"Data successfully written to {outfile}")

    except FileNotFoundError:
        print("File not found")
    except Exception as e:
        print(f"An error occurred: {e}")

def binConverter(targetFilePath,chosePath,outputPath):
    import struct
    import os
    import json

    jsonCacheList = [""] * 50000

    targetFolderPath = "/".join(targetFilePath.split("/")[:-1])
    os.chdir(targetFolderPath)
    targetFileName = targetFilePath.split("/")[-1]
    configFileName = targetFilePath.split("/")[-1].replace(".bin","Config.txt")

    configFile = open(configFileName, "r")
    configString = ""
    jsonEntry = {}
    jsonEntryListForParsingDataNames = []
    totalStructSize = 0


    for line in configFile:
        if line.strip() == "[-]":
            break
        lineList = line.strip().split(",")
        jsonEntry[lineList[1]]=""
        dataType = lineList[0]
        jsonEntryListForParsingDataNames.append(lineList[1])
        if dataType == "unsigned long long" or dataType == "unsigned long long int":
            configString = configString + "Q "
            totalStructSize += 8
        elif dataType == "unsigned long" or dataType == "unsigned long int":
            configString = configString + "L "
            totalStructSize += 4
        elif dataType == "float":
            configString = configString + "f "
            totalStructSize += 4
        elif dataType == "int":
            configString = configString + "i "
            totalStructSize += 4

    arduinoStructSize = int(configFile.readline().strip())
    if arduinoStructSize != totalStructSize:
        numPadding = (arduinoStructSize - totalStructSize)/4
        configString = configString + str(count) + "X "

    configString = "".join(configString[:-1])

    outfile = targetFileName.replace(".bin","") + ".txt"

    if chosePath:
        outfile = os.path.normpath(os.path.join(outputPath, outfile))
    
    currentActualPacket = ""
    currentEvaluatedPacket =""
    binaryFile = open(targetFileName, "rb")
    jsonFile = open(outfile, "w")
    jsonFile.write("[\n")
    firstTime = True
    #start at 1 to make modulus math easier
    counter = 0
    try:
        print("Writing data...........")
        # Read and process binary data
        while chunk := binaryFile.read(arduinoStructSize):
            if len(chunk) != arduinoStructSize:
                print("Incomplete data chunk encountered. Skipping.")
                continue

            # Unpack the binary data
            data = struct.unpack(configString, chunk)
            dataString = ",".join(map(str, data))
            dataStringList = dataString.split(",")
            smallerDataStringList = dataStringList[2:]
            if "".join(smallerDataStringList) != currentActualPacket:
                for i in range(0, len(jsonEntryListForParsingDataNames)):
                    jsonEntry[jsonEntryListForParsingDataNames[i]] = data[i]
                if not firstTime:
                    jsonCacheList[counter] = ","
                jsonCacheList[counter] += json.dumps(jsonEntry)
                currentActualPacket = "".join(smallerDataStringList)
                firstTime = False
                counter += 1
                if counter == 10000:
                    jsonFile.write("".join(jsonCacheList))
                    jsonCacheList = [""] * 10000
                    counter = 0

        print(f"Data successfully written to {outfile}")

    except FileNotFoundError:
        print("File not found")
    except Exception as e:
        print(f"An error occurred: {e}")
    jsonFile.write("".join(jsonCacheList))
    jsonFile.write("]")
    binaryFile.close()
    jsonFile.close()
