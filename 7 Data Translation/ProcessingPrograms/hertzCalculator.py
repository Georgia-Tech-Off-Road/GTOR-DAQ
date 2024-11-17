import statistics
import os
import shutil
from DataDownloader import DataDownloader
import tkinter as tk
from tkinter import ttk


def calculateHertz(filePath, hertzCalculatorScreen):
    #create a label to say Analyzing File
    label1 = tk.Label(hertzCalculatorScreen, text="Analyzing File")
    label1.pack()
    #create a progress bar that just bounces back and forth
    progressBar = ttk.Progressbar(hertzCalculatorScreen, mode = "indeterminate", maximum=100)
    progressBar.pack(padx=20, pady=20, fill="x")
    #start the bouncing
    progressBar.start()
    #open data file for hertz calculation
    fileToRead = open(filePath)
    #set minimum refresh to a big number so the first instance will be smaller than it
    minRefresh = 999999999
    #create a list of time stamps
    timeStamps = []
    #create a string for the current time stamp
    currentTimeStamp = ""
    #create the current time stamp index (starts at -1 so the first add makes the index 0 
    currentTimeStampIndex = -1
    #loop over the file
    for line in fileToRead:
        #get the second timeStamp
        timeStamp = line.split(",")[0]
        #if the line is long enough to actually have data (this is mostly defunct now but is still in here in case someone has an old data file with a second mark before the daat)
        if len(timeStamp) > 5:
            #if time stamp is different the one currently being analyzed then add a new time stamp to the list and update current time stamp
            if timeStamp != currentTimeStamp:
                timeStamps.append([line, 1])
                currentTimeStamp = timeStamp
                currentTimeStampIndex += 1
            #if its the same then increment the count for the current time stamp
            else:
                timeStamps[currentTimeStampIndex][1] += 1
    #close the file
    fileToRead.close()
    #create a list for time stamp data points
    timeStampDataPoints =[]
    #remove the first and last couple seconds of data from the calculation as they likely misrepresent the actual polling rate
    timeStamps.pop(0)
    timeStamps.pop(0)
    timeStamps.pop(len(timeStamps) - 1)
    timeStamps.pop(len(timeStamps) - 1)
    #loop through the timeStamps and append them to the timeStamps list, finding the mimimum refresh rate while doing so
    for timeStamp in timeStamps:
        timeStampDataPoints.append(timeStamp[1])
        if timeStamp[1] < minRefresh:
            minRefresh = timeStamp[1]
    #hide the progress bar and initial label
    progressBar.pack_forget()
    label1.pack_forget()
    #display various statistics about the dataFile
    label2 = tk.Label(hertzCalculatorScreen,  text = str(str(statistics.pvariance(timeStampDataPoints)) + " : pVariance"))
    label3 = tk.Label(hertzCalculatorScreen, text = str(str(statistics.mean(timeStampDataPoints)) + " : mean"))
    label4 = tk.Label(hertzCalculatorScreen,  text = str(str(len(timeStampDataPoints))+ " : numSeconds"))
    label5 = tk.Label(hertzCalculatorScreen,  text = str(str(minRefresh) + ": minRefreshRate"))
    label2.pack()
    label3.pack()
    label4.pack()
    label5.pack()

        
