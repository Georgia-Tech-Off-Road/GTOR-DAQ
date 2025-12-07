filename = "2025-10-30 23_53_42"
outputFilename = filename + "output"
file = open(filename + ".txt")
outputFile = open(outputFilename + ".txt", "w")
currentActualPacket = ""
currentEvaluatedPacket =""
firstTime = True
for line in file:
    if line.strip() == "},{":
        packetToBeEvaluatedOne = "".join(currentActualPacket.split('\n')[2:])
        packetToBeEvaluatedTwo = "".join(currentEvaluatedPacket.split('\n')[2:])
        if packetToBeEvaluatedOne != packetToBeEvaluatedTwo:
            if firstTime == False:
                outputString = "},{\n" + currentEvaluatedPacket
            else:
                outputString = currentEvaluatedPacket
                firstTime = False
            outputFile.write(outputString)
            currentActualPacket = currentEvaluatedPacket
        currentEvaluatedPacket = ""
    else:
        if line.strip == '[':
            outputFile.write(line)
        elif line.strip == '{':
            outputFile.write(line)
        else:
            currentEvaluatedPacket += line
outputFile.write("}]\n")

file.close()
outputFile.close()

    
