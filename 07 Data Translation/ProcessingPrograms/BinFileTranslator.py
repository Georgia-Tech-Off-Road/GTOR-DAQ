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
    import subprocess

    #save initial directory
    initialDirectory = os.getcwd()

    targetFolderPath = "/".join(targetFilePath.split("/")[:-1])
    os.chdir(targetFolderPath)
    targetFileName = targetFilePath.split("/")[-1]
    configFileName = targetFilePath.split("/")[-1].replace(".bin","Config.txt")
    names =[]
    configFile = open(configFileName, "r")
    configString = ""
    numElements = 0

    for line in configFile:
        if line.strip() == "[-]":
            break
        lineList = line.strip().split(",")
        dataType = lineList[0]
        names.append(lineList[1])
        if dataType == "unsigned long long" or dataType == "unsigned long long int":
            configString = configString + "7"
        elif dataType == "unsigned long" or dataType == "unsigned long int":
            configString = configString + "6"
        elif dataType == "float":
            configString = configString + "2"
        elif dataType == "int":
            configString = configString + "1"
        numElements += 1

    arduinoStructSize = int(configFile.readline().strip())

    configString=str(arduinoStructSize) + "-" + str(numElements) + "-" + configString
    
    outfile = targetFileName.replace(".bin","") + ".txt"

    if chosePath:
        outfile = os.path.normpath(os.path.join(outputPath, outfile))
    else:
        outfile = targetFilePath.replace(".bin","") + ".txt"

    numNameChars = 0
    for name in names:
        numNameChars += len(name)
    nameString = "-".join(names)
    nameString = str(numNameChars) + "-" + nameString
    processorPath =  initialDirectory + "/ProcessingPrograms/DAQBinFileProcessor.exe"
    print("Processing File!")
    subprocess.run([processorPath, targetFilePath, outfile, configString, nameString])
    print("Finished!")
