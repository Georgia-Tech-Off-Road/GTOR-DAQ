import tkinter as tk
from tkinter import filedialog
from tkinter import ttk
import os
import importlib
import threading
import time
import matplotlib
import sys
import csv
import json
import subprocess

#libraries used for program
#NOTE: NEVER PUT ANYTHING IN FRONT OF IMPORTS AND ALWAYS KEEP IMPORTANTS IN THIS NOTATION, OTHERWISE THE UPDATER WILL LIKELY BREAK

#file imports
from DataDownloader import DataDownloader
from Updater import DataTranslatorUpdater
from Visualizers import TestVisualizer
from ProcessingPrograms import FileSplitter

#imports the processing programs (hertz calculator, data processor, etc etc) 
os.chdir("./")
for fileName in os.listdir("ProcessingPrograms"):
        if fileName.endswith('.py') and not fileName.startswith('__'):
            moduleName = fileName[:-3]  # Remove .py extension
            module = importlib.import_module(f'ProcessingPrograms.{moduleName}')
            globals()[moduleName] = module

# Creates the main window
root = tk.Tk()
root.title("Main Page")
root.geometry("600x200")

with open("Settings/Settings.csv") as settingsf:
    reader = csv.reader(settingsf)
    settingsData = list(reader)

# Function to go to Data Processing Tool
def dataProcessingTool():
    root.withdraw()
    # Create a new window for the data processing tool
    dataProcessingToolPage = tk.Toplevel(root)
    dataProcessingToolPage.title("Data Downloader Tool")
    dataProcessingToolPage.geometry("800x400")
    global useDefaultConfig
    if settingsData[1][3] == "False":
        useDefaultConfig = tk.IntVar(value=0)
    else:
        useDefaultConfig = tk.IntVar(value=1)
    global outputPath
    outputPath = None
    global chosePath
    chosePath = False
    def openHowTo():
        #find the DataProcessingTool.txt file
        howToFilePath = "Guides/DataProcessingTool.txt"
        #open the howToFile file in notepade                    (See if this works on mac.....)
        if sys.platform.startswith("win"):  # Windows
            os.system(f'notepad.exe {howToFilePath}')
        elif sys.platform.startswith("darwin"):  # Mac
            os.system(f'open {howToFilePath}')
    def chooseFile():
        # Open file dialog and get the file path
        global filePath
        filePath = filedialog.askopenfilename()
        # Display the file path in the label
        fileSelectLabel.config(text=f"Selected file: {filePath}")
        #update the buttons to allow the file to be operated on
        updateButtons()

    def updateButtons():
        #loop through every frame in the data processing page
        for frame in dataProcessingToolPage.winfo_children():
            if isinstance(frame, tk.Frame):
                #loop through every widget in the frame
                for widget in frame.winfo_children():
                    #if the widget is a button and isn't the chooseFile button then hide it
                    if widget["text"] != "Choose File" and widget["text"] != "How To"  and isinstance(widget, tk.Button):
                            widget.grid_forget()
        #if a filePath has been chosen
        if ('filePath' in globals()):
                #if the filepath isn't on the main OS drive only display the download button
                if "C:/" not in filePath:
                    downloadButton.grid(row=0, column=1, padx=20)
                    outputButton.grid(row=0,column=2,padx=20)
                elif ".bin" in filePath:
                    outputButton.grid(row=0,column=1,padx=20)
                elif ".csv" in filePath:
                    customButton.grid(row=1,column=1,padx=20)
                    configCheckbox.grid(row=2, column=3, padx=20)
                #otherwise display everything but the download button
                else: 
                    processButton.grid(row=0, column=0, padx=20)
                    configEditButton.grid(row=0, column=1, padx=20)
                    indexButton.grid(row=1,column=0,padx=20)
                    customButton.grid(row=1,column=1,padx=20)
                    splitButton.grid(row=1, column=2, padx=20)
                    outputButton.grid(row=2, column=0, padx=20)
                    configCheckbox.grid(row=2, column=1, padx=20)

    def downloadData():
        #create a new page for the progress bar
        global outputPath, chosePath
        progressBarPage = tk.Toplevel(dataProcessingToolPage)
        progressBarPage.title("Download Progress")
        progressBarPage.geometry("400x200")
        destinationFilePath = str(os.getcwd()+ "\\"+ os.path.basename(filePath))
        #open the file to create it to prevent any problems with later code being unable to find the target
        file = open(os.path.basename(destinationFilePath), "a")
        file.close()
        #if the user does not select a file path, default to current CWD
        if not chosePath:
            destinationFilePath = os.getcwd() + "\\Configs\\" + os.path.basename(filePath)
        else:
            destinationFilePath = outputPath + "\\" + os.path.basename(filePath)
        #use multithreading to allow the download to run seperately from the progress bar updater (more zoom zoom)
        downloadThread = threading.Thread(target = DataDownloader.downloadData, args = (filePath, destinationFilePath, outputPath, chosePath))
        progressBarThread = threading.Thread(target = DataDownloader.updateProgressBar, args = (filePath, destinationFilePath, progressBarPage, dataProcessingToolPage, outputPath, chosePath))
        #start threads
        downloadThread.start()
        progressBarThread.start()
        #hide main data processing page to prevent people from breaking things
        dataProcessingToolPage.withdraw()
        #run update buttons to make sure everything's in the right place once the page comes back (it'll be unhidden by progress bar thread once it sees that the file has finished being downloaded
        updateButtons()
        #if no Configs folder then create one

        #get the file path for the config file included with the data
        configDST = os.getcwd() + "\\Configs\\" + os.path.basename(filePath).split('.')[0] + "Config.txt"
        #create a target file
        file = open(configDST, "w")
        file.close()
        #build out the source filepath
        configSRCLIST = filePath.split("/")
        configSRCLIST[-1] = "Config.txt"
        configSRC = "/".join(configSRCLIST)
        sourceFileSize = os.path.getsize(configSRC)
        #create the thread and download the config file (this isn't tracked since it's such a short download)
        configDownloadThread = threading.Thread(target = DataDownloader.downloadData, args = (configSRC, configDST, outputPath, chosePath))
        configDownloadThread.start()

    def outputDestination():
        global chosePath
        global outputPath
        outputPath = filedialog.askdirectory(title="Select Output Folder")
        #lets the user choose a different file path
        if outputPath:
            chosePath = True
        else:
            chosePath = False
        outputSelectLabel.config(text=f"Selected output path: {outputPath}")
    """ DEPRECATED
    def binConvert():
        #update the buttons to allow the file to be operated on
        global chosePath
        global outputPath
        binThread = threading.Thread(target = BinFileTranslator.binConverter, args = (filePath,chosePath,outputPath,settingsData))
        #start the thread
        binThread.start()
    """
    def processData():
        global chosePath
        global outputPath
        #create a page for the progress bar
        progressBarPage = tk.Toplevel(dataProcessingToolPage)
        progressBarPage.title("Translation Progress")
        progressBarPage.geometry("400x200")
        #create the thread
        dataProcessingThread = threading.Thread(target = DataTranslator.translateData, args = (filePath, progressBarPage, dataProcessingToolPage, int(useDefaultConfig.get()),outputPath,chosePath,settingsData))
        #start the thread
        dataProcessingThread.start()
        #hide the main data processor page
        dataProcessingToolPage.withdraw()
    """ DEPRECATED
    def calculateHertz():
        #open a hertz calculator page
        hertzCalculationPage = tk.Toplevel(dataProcessingToolPage)
        hertzCalculationPage.title("Hertz Calculator")
        hertzCalculationPage.geometry("400x200")
        #create the thread for hertz calculator
        hertzCalculatorThread = threading.Thread(target = hertzCalculator.calculateHertz, args = (filePath, hertzCalculationPage))
        #start the thread
        hertzCalculatorThread.start()
    """
    def editConfig():
        #find the config file based on filePath variable
        if useDefaultConfig.get() == 1:
            configFilePath = "Configs/defaultConfig.txt"
        else:
            configFilePathList = filePath.split("/")
            configFilePathList[-1] = "Configs/" + configFilePathList[-1] + "Config.txt"
            configFilePath = "/".join(configFilePathList)
        #open the config file in notepade (See if this works on mac.....)
        if sys.platform.startswith("win"):  # Windows
            os.system(f'notepad.exe {configFilePath}')
        elif sys.platform.startswith("darwin"):  # Mac
            os.system(f'open {configFilePath}')

    def custom():
        customWindow = tk.Toplevel()
        customWindow.title("Visualizer")
        customWindow.geometry("800x300")
        # Label and Entry for column number
        label = tk.Label(customWindow, text="Enter 1+ indeces separated by commas (i.e. 2,3,4 or just 2)\nNote that index 0 = column #1.\nSensor data starts at 1.")
        label.pack(pady=5)
        columnEntry = tk.Entry(customWindow)
        columnEntry.pack(pady=5)
        def runVisualizer():
            columnNumber = columnEntry.get()
            columnNumber = list(map(int, columnNumber.split(",")))
            visualizerThread = threading.Thread(target = TestVisualizer.testVisualizer, args = (filePath, columnNumber,customWindow,int(useDefaultConfig.get())))
            visualizerThread.start()
        createGraphButton = tk.Button(customWindow, text="Create Graph",font=("Helvetica", 12, "bold"), command=runVisualizer)
        createGraphButton.pack(pady=10)

    def indices(filePath):      #used for showing a legend of data types & their indeces
        #imports
        import pandas as pd
        import json
        import tkinter as tk
        from tkinter import ttk

        #reads json, converts to dataframe
        with open(filePath, "r") as f:
            dayta = f.read()
            data = json.loads(dayta)
        df = pd.json_normalize(data)

        #formats variables next to their index
        variables = {}
        for idx, col in enumerate(df.columns):
            varName = f"Index {idx}"
            variables[varName] = col

        #gui
        indexWindow = tk.Toplevel()
        indexWindow.title(f"Indices for {filePath}")
        indexWindow.geometry("500x400")
        canvas = tk.Canvas(indexWindow)
        scrollbar = ttk.Scrollbar(indexWindow, orient="vertical", command=canvas.yview)
        scrollFrame = ttk.Frame(canvas)
        scrollFrame.bind("<Configure>", lambda e: canvas.configure(scrollregion=canvas.bbox("all")))
        canvas.create_window((0,0), window=scrollFrame, anchor="nw")
        canvas.configure(yscrollcommand=scrollbar.set)
        canvas.pack(side="left", fill="both", expand=True)
        scrollbar.pack(side="right", fill="y")

        #writes data type and index onto gui
        for varName, value in variables.items():
            tk.Label(scrollFrame, text=f"{varName}: {value}").pack(anchor="w", padx=10, pady=2)


    def splitFile():
        fileSplitPage = tk.Toplevel()
        fileSplitPage.title("File Splitter")
        fileSplitPage.geometry("800x300")
        label = tk.Label(fileSplitPage, text="Enter number of data sets to keep per file or write 'd' for default (10,000,000)")
        label.pack(pady=5)
        splitEntry = tk.Entry(fileSplitPage)
        splitEntry.pack(pady=5)
        def split():
            splitNum = splitEntry.get()
            if str(splitNum) == 'd':
                splitNum = 10000000
            else:
                splitNum = int(splitNum)
            splitThread = threading.Thread(target = FileSplitter.split, args = (filePath, splitNum, fileSplitPage))
            splitThread.start()
        splitterButton = tk.Button(fileSplitPage, text="Split Files",font=("Helvetica", 12, "bold"), command=split)
        splitterButton.pack(pady=10)

    #howToButton
    howToButton = tk.Button(dataProcessingToolPage, text="How To", command=lambda: openHowTo())
    howToButton.pack()
    
    #create a button to select the file to be processed
    fileSelectButton = tk.Button(dataProcessingToolPage, text="Choose File",font=("Helvetica", 14, "bold"), command=chooseFile)
    fileSelectButton.pack(pady=10)
  
    #create a label to show the file selected
    fileSelectLabel = tk.Label(dataProcessingToolPage, text="No file selected")
    fileSelectLabel.pack(pady=5)

    outputSelectLabel = tk.Label(dataProcessingToolPage, text = "No output path selected")
    outputSelectLabel.pack(pady=5)

    #Create a frame for the buttons
    buttonFrame = tk.Frame(dataProcessingToolPage)
    buttonFrame.pack(pady=20)

    #Create and place the buttons in a single row on the second page
    downloadButton = tk.Button(buttonFrame, text="Download Data File", command=lambda: downloadData())
    splitButton = tk.Button(buttonFrame, text = "Split large txt file into smaller files", command = lambda: splitFile())

    customButton = tk.Button(buttonFrame, text="Data Visualizer", font=("Helvetica", 12, "bold"),fg="navy",bg="gold", command=lambda: custom())
    indexButton = tk.Button(buttonFrame, text= "Show File Indeces", command=lambda: indices(filePath))
    processButton = tk.Button(buttonFrame, text="Process Data", command=lambda: processData())
    configCheckbox = tk.Checkbutton(buttonFrame, text="Use default config", variable=useDefaultConfig)
    configEditButton = tk.Button(buttonFrame, text="Edit Config", command=lambda: editConfig())
    outputButton = tk.Button(buttonFrame, text = "Choose output destination...", command=lambda: outputDestination())
    updateButtons()

def runUpdater():
    #create update thread
    updateThread = threading.Thread(target = DataTranslatorUpdater.runUpdater, args = ())
    #destroy the root page
    root.destroy()
    #start the thread
    updateThread.start()


def openHowTo():
    #find the HomeScreen.txt file]
    howToFilePath = "Guides/HomeScreen.txt"
    #open the howToFile file in notepade (See if this works on mac.....)
    if sys.platform.startswith("win"):  # Windows
        os.system(f'notepad.exe {howToFilePath}')
    elif sys.platform.startswith("darwin"):  # Mac
        os.system(f'open {howToFilePath}')

def settings():
    settingsPath = "Settings/Settings.csv"
    #open the howToFile file in notepade (See if this works on mac.....)
    if sys.platform.startswith("win"):  # Windows
        os.system(f'notepad.exe {settingsPath}')
    elif sys.platform.startswith("darwin"):  # Mac
        os.system(f'open {settingsPath}')

#pack the main page
frame = tk.Frame(root)
frame.pack(pady=20)

# Create main label
programSelectLabel = tk.Label(frame, text="Please choose a program/tool")
programSelectLabel.pack()

# Create frame for buttons
buttonFrame = tk.Frame(root)
buttonFrame.pack(pady=20)

#create the dataProcessingToolButton
dataProcessingToolButton = tk.Button(buttonFrame, text="    Data Tool    ",font=("Helvetica", 14, "bold"), command=lambda: dataProcessingTool())
dataProcessingToolButton.grid(row=1, column=0, padx=20)

# Create button 2
updaterButton = tk.Button(buttonFrame, text="Update Program", command=lambda: runUpdater())
updaterButton.grid(row=1, column=4, padx=20)

# Create Button 3
howToButton = tk.Button(buttonFrame, text="How To", command=lambda: openHowTo())
howToButton.grid(row=1, column=5, padx=20)

#Create button 4
settingsButton = tk.Button(buttonFrame, text="View/Edit Settings", command=lambda: settings())
settingsButton.grid(row=1, column=6,padx=20)

# Run the application
root.mainloop()
