import statistics
import os
import shutil
from DataDownloader import DataDownloader
import tkinter as tk
from tkinter import ttk


def mode1(fileToRead, hertzCalculatorScreen, progressBar, label):
    minRefresh = 999999999
    timeStamps = []
    currentTimeStamp = ""
    currentTimeStampIndex = -1
    for line in fileToRead:
        line = line.split(",")[0]
        if len(line) > 5:
            if line != currentTimeStamp:
                timeStamps.append([line, 1])
                currentTimeStamp = line
                currentTimeStampIndex += 1
            else:
                timeStamps[currentTimeStampIndex][1] += 1
    #print(timeStamps)
    timeStampDataPoints =[]
    timeStamps.pop(0)
    timeStamps.pop(0)
    timeStamps.pop(len(timeStamps) - 1)
    timeStamps.pop(len(timeStamps) - 1)
    for timeStamp in timeStamps:
        timeStampDataPoints.append(timeStamp[1])
        if timeStamp[1] < minRefresh:
            minRefresh = timeStamp[1]
    progressBar.pack_forget()
    label.pack_forget()
    label2 = tk.Label(hertzCalculatorScreen, text=str(str(statistics.pvariance(timeStampDataPoints)) + " : pVariance"))
    label3 = tk.Label(hertzCalculatorScreen, text=str(str(statistics.mean(timeStampDataPoints)) + " : mean"))
    label4 = tk.Label(hertzCalculatorScreen, text=str(str(len(timeStampDataPoints))+ " : numSeconds"))
    label5 = tk.Label(hertzCalculatorScreen, text=str(str(minRefresh) + ": minRefreshRate"))
    label2.pack()
    label3.pack()
    label4.pack()
    label5.pack()
def calculateHertz(fileName, hertzCalculatorScreen):
    inputFile = open(fileName)
    label = tk.Label(hertzCalculatorScreen, text="Analyzing File")
    label.pack()
    progressBar = ttk.Progressbar(hertzCalculatorScreen, mode = "indeterminate", maximum=100)
    progressBar.pack(padx=20, pady=20, fill="x")
    progressBar.start()
    mode1(inputFile, hertzCalculatorScreen, progressBar, label)
    inputFile.close()

        
