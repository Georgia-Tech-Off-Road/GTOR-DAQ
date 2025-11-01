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
    import numpy as np
    import struct
    import os
    import json

    print("Time to go make coffee!")
    targetFolderPath = "/".join(targetFilePath.split("/")[:-1])
    os.chdir(targetFolderPath)
    targetFileName = targetFilePath.split("/")[-1]
    configFileName = targetFilePath.split("/")[-1].replace(".bin","Config.txt")

    configFile = open(configFileName, "r")
    configString = ""
    totalStructSize = 0
    jsonEntryListForParsingDataNames = []

    numpyFieldList = []
    
    #type map generously provided by chatgpt
    type_map = {
    "unsigned long long": "uint64",
    "unsigned long long int": "uint64",
    "unsigned long": "uint32",
    "unsigned long int": "uint32",
    "float": "float32",
    "int": "int32",
    }

    for line in configFile:
        if line.strip() == "[-]":
            break
        lineList = line.strip().split(",")
        dataType = type_map[lineList[0]]
        name = lineList[1]
        numpyFieldList.append((name, dataType))
        totalStructSize += np.dtype(dataType).itemsize
        jsonEntryListForParsingDataNames.append(lineList[1])

    arduinoStructSize = int(configFile.readline().strip())
    if arduinoStructSize != totalStructSize:
        numPadding = int((arduinoStructSize - totalStructSize))
        numpyFieldList.append(("pad", f"V{numPadding}"))

    fancyNumpyTranslatorThing = np.dtype(numpyFieldList)
    
    outfile = targetFileName.replace(".bin","") + ".txt"

    if chosePath:
        outfile = os.path.normpath(os.path.join(outputPath, outfile))


    #make a numpy memmap whatever that is
    data = np.memmap(targetFileName, dtype=fancyNumpyTranslatorThing, mode="r")
    totalData = len(data)
    names = list(fancyNumpyTranslatorThing.names)
    currentActualPacket = tuple(0 for _ in range(len(names)))
    jsonFile = open(outfile, "w")
    jsonFile.write("[\n")
    firstTime = True
    #loop through fifty thousand items at a time
    for i in range(0, len(data), 5000000):
        jsonCacheList =[]
        #grab part to work on
        chunk = data[i:i+5000000]
        for datum in chunk:
            smallDatum = tuple(datum[name] for name in names[2:])
            if smallDatum != currentActualPacket:
                jsonEntry = {name: datum[name].item() for name in jsonEntryListForParsingDataNames}
                jsonCacheList.append(jsonEntry)
                currentActualPacket = smallDatum
        if jsonCacheList:
            if not firstTime:
                jsonFile.write(",\n")
            firstTime = False
            jsonFile.write(",\n".join(json.dumps(entry) for entry in jsonCacheList))
        print(str(((i+5000000)/totalData)*100) +"% done!")
    jsonFile.write("]")
    jsonFile.close()
    print("Done!")
