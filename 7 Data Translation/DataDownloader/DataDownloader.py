import shutil
import os
import time
#handles data download and includes basic progress bar
def copy_with_progress(src, dst):
    #finds file size
    fileSize = os.path.getsize(src)
    #sets oldTime
    oldTime = time.time()
    #initializes a variable to keep track of last download speed
    previousDownloadSpeed = 0
    #initializes variable to keep track of num megs to have in buffer
    numMegs = 10
    #creates boolean to tell program if it should wait to reduce buffer size
    wait = True
    #opens the source and destination files    
    with open(src, 'rb') as fsrc, open(dst, 'wb') as fdst:
        #creates a variable to keep track of data copied so far
        copied = 0
        #creates a variable to hold buffer size
        bufferSize = numMegs * 1024 * 1024  # Copy in numMegsMB chunks
        while True:
            #reads data from file into buffer (I think this uses shutil but not sure)
            buf = fsrc.read(bufferSize)
            #if buffer is empty
            if not buf:
                break
            #write buffer to file
            fdst.write(buf)
            #add buffer length to copied variable
            copied += len(buf)
            #calculate download speed (if it downloades numMegs in less than a second than call download speed infinite)
            try:
                downloadSpeed = round(((numMegs)/(time.time() - oldTime)),2)
                if (downloadSpeed < previousDownloadSpeed) and not wait:
                    numMegs -= 2
                    #prevent numMegs from going below 1
                    if numMegs <= 0:
                        numMegs = 1
                    previousNumMegs = numMegs
                    wait = True
                elif wait:
                    wait = False
                previousDownloadSpeed = downloadSpeed
            except:
                downloadSpeed = float(inf)
            #calculate download percentage
            percentage = round((copied/fileSize * 100), 2)
            #print progress bar
            print("Downloading: " + str(percentage) + "% at " + str(downloadSpeed) + "MB/s")
            #update old time
            oldTime = time.time()
            #increments numMegs to try and find optimum download rate
            if not wait:
                numMegs += 1
            
    
def DownloadDataFile():
    originalDirectory = os.getcwd()
    driveLetter = ""
    while True:
            driveLetter = input("Please type the drive letter for your MechE network drive.\n")
            try:
                    os.chdir(driveLetter+":")
                    break
            except:
                    print("Something went wrong. Please make sure you're signed into the mechE drive.")
    while True:
        directory = os.getcwd()
        print(os.listdir())  # List files in the directory
        newFolder = input("Please type the name of the folder you would like to navigate to or type DEFAULT to skip to the current season's data folder. Type STOP when you've found the data file you're looking for.\n")
        if newFolder == "STOP":
            break
        elif newFolder == "DEFAULT":
            try:
                os.chdir(originalDirectory + "/DataDownloader/Config")
                config = open("config.txt")
                lines = config.readlines()
                filePath = lines[1].replace("~?~", driveLetter)
                os.chdir(filePath)
            except:
                print("Something went wrong. Please try manually navigating to the file.")
                os.chdir(directory)
        else:
            try:
                full_path = os.path.join(directory, newFolder)
                os.chdir(full_path)
            except:
                print("IMPROPER FILENAME UR BAD KID")
    while True:
        fileName = input("What file would you like to read from?\n")
        if os.path.isfile(os.getcwd() + "/"+ fileName):
            break
        else: 
            print("IMPROPER FILENAME UR BAD KID")
    print("Copying data to temporary local file.")
    copy_with_progress(os.getcwd() + "/" + fileName, originalDirectory + "/temp.txt")
    print("Opening local temp file.")
    os.chdir(originalDirectory)
    fileToRead = open("temp.txt","r")
    return fileToRead
