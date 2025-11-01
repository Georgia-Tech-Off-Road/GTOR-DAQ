fileOne = open("C:/Users/andre/Documents/GitHub/GTOR-DAQ/07 Data Translation/2025-10-31 19_27_28/2025-10-31 19_27_28.txt")
fileTwo = open("C:/Users/andre/Documents/GitHub/GTOR-DAQ/07 Data Translation/2025-10-31 19_27_28/CorrectForComparison.txt")

for line in fileOne:
    if line != fileTwo.readline():
        print(line)

fileOne.close()
fileTwo.close()
