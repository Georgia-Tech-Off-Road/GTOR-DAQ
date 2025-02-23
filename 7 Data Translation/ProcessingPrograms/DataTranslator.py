#I still lose the last final bit of an average I think (like a hundredth of a second) kinda a good thing tho so you don't pretend you have proepr (andrew you suck at spelling) averages when you don't
import os
from DataDownloader import DataDownloader
import math
import importlib
import tkinter as tk
from tkinter import ttk
from tkinter import messagebox
import subprocess
import shutil
import threading

#imports all data libraries from ConversionLibrary folder
os.chdir("./")
for fileName in os.listdir("ConversionLibraries"):
        if fileName.endswith('.py') and not fileName.startswith('__'):
            moduleName = fileName[:-3]  # Remove .py extension
            module = importlib.import_module(f'ConversionLibraries.{moduleName}')
            globals()[moduleName] = module

class Sensor:
    def __init__(self, index, dataType, name, pollingRate, numTeeth):
        #index of sensor in data file
        self.index = int(index)
        #datatype of sensor options are analog or digital
        self.dataType = dataType
        #name of sensor
        self.name = name
        #sensor polling rate
        self.pollingRate = int(pollingRate)
        #number of teeth of sensor (only applies to halleffect sensors but is still present for all sensors)
        self.numTeeth = int(numTeeth)
        #sum, num data points, last time they were averaged, timesAvged for analog sensors
        self.sumList = [0, 0, 0, 0]
        #last value, time of last value
        self.digitalList = [0, 0]
        #list of rpm values with their correpsonding time stamps
        self.rpmValueList = []

    def __repr__(self):
        return f"Sensor(dataType='{self.dataType}', name='{self.name}', pollingRate={self.pollingRate})"

def translateData (inputFilePath, progressBarPage, parentPage,useDefaultConfig,outputPath,chosePath,settingsData):
    if useDefaultConfig == 1:
        file = "Configs/defaultConfig.txt"
    else:
        configFileName = os.path.basename(inputFilePath).split('.')[0]+"Config.txt"
        file = "Configs/" + configFileName
    outputFileFolder = settingsData[0][2]
    #opens output file
    if outputFileFolder == "":
        outputFileBase = os.getcwd()
    else:
        outputFileBase = outputFileFolder
    print(outputFileBase)
    print(os.getcwd())
    if not chosePath:
        outfile = os.path.join(outputFileBase, os.path.basename(inputFilePath).replace(".txt", ".csv"))
        print(outfile)
    else:
        outfile = str(os.path.join(outputPath, os.path.basename(inputFilePath).replace(".txt", ".csv")))
        print(outfile)

    dataTranslationProgressLabel = tk.Label(progressBarPage, text="Data Translation Progress")
    dataTranslationProgressLabel.pack()
    progressBar = ttk.Progressbar(progressBarPage, mode = "determinate", length=100)
    progressBar.pack(padx=20, pady=20, fill="x")

    err = processData(file,inputFilePath,outfile, progressBar, dataTranslationProgressLabel, progressBarPage, progressBarPage)
    if err != None:
         messagebox.showerror("Error!", err)
    # destroy the progress bar page
    progressBarPage.destroy()
    #unhide the parent page
    parentPage.deiconify()

# Returns 0 if success, -1 otherwise
def processData(config_file, input_file, output_file, progressBar, dataTranslationProgressLabel, progressBarPage, verbose=True):
    #Gets complete path to executable
    executable_location = shutil.which(os.path.join('ProcessingPrograms','Build', 'Windows', 'dataprocess.exe'))
    print("Executable location: " + executable_location)
    cmd = [executable_location,"-c", config_file, "-i", input_file, "-o", output_file]
    if verbose:
        cmd.append("-v")
    print(cmd)
    processData = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, creationflags=subprocess.CREATE_NO_WINDOW)
    while True:
        if processData.poll() != None:
            break
        out = processData.stdout.readline()
        if out:
            if "Update: " in out:
                 percentage = round(float(out[8:-2].strip()),2)
                 progressBar["value"] = percentage
                 dataTranslationProgressLabel.config(text = str("Percentage " + str(percentage)) + "%")
                 dataTranslationProgressLabel.pack()
                 progressBar.pack()
                 progressBarPage.update()
    stderr = processData.stderr.read()
    if stderr:
         return stderr
    return None
