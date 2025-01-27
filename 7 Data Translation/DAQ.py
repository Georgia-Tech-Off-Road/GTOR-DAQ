import tkinter as tk
from tkinter import filedialog
from tkinter import ttk
import os
import importlib
import threading
import time
import matplotlib

#libraries used for program
#NOTE: NEVER PUT ANYTHING IN FRONT OF IMPORTS AND ALWAYS KEEP IMPORTANTS IN THIS NOTATION, OTHERWISE THE UPDATER WILL LIKELY BREAK

#file imports
from DataDownloader import DataDownloader
from Updater import DataTranslatorUpdater
from Visualizers import AccelerationVisualizer
from Visualizers import BrakePressureVisualizer
from Visualizers import RPMVisualizer
from ProcessingPrograms import BinFileTranslator

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

# Function to go to Data Processing Tool
def dataProcessingTool():
    root.withdraw()
    # Create a new window for the data processing tool
    dataProcessingToolPage = tk.Toplevel(root)
    dataProcessingToolPage.title("Data Downloader Tool")
    dataProcessingToolPage.geometry("800x400")
    def openHowTo():
        #find the DataProcessingTool.txt file
        howToFilePath = "Guides/DataProcessingTool.txt"
        #open the howToFile file in notepade (See if this works on mac.....)
        os.system(f'notepad.exe {howToFilePath}')
        # Function to handle file upload
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
                if ".bin" in filePath:
                    binButton.grid(row=0, column=1,padx=20)
                if ".csv" in filePath:
                    accelButton.grid(row=1, column=0, padx=20)
                    brakeButton.grid(row=1, column=1, padx=20)
                    rpmButton.grid(row=1, column=2, padx=20)
                #otherwise display everything but the download button
                else: 
                    processButton.grid(row=0, column=0, padx=20)
                    configEditButton.grid(row=0, column=1, padx=20)
                    herztCalculatorButton.grid(row=0, column=2, padx=20)
    def downloadData():
        #create a new page for the progress bar
        progressBarPage = tk.Toplevel(dataProcessingToolPage)
        progressBarPage.title("Download Progress")
        progressBarPage.geometry("400x200")
        destinationFilePath = str(os.getcwd()+ "\\"+ os.path.basename(filePath))
        #open the file to create it to prevent any problems with later code being unable to find the target
        file = open(os.path.basename(destinationFilePath), "a")
        file.close()
        #use multithreading to allow the download to run seperately from the progress bar updater (more zoom zoom)
        downloadThread = threading.Thread(target = DataDownloader.downloadData, args = (filePath, destinationFilePath))
        progressBarThread = threading.Thread(target = DataDownloader.updateProgressBar, args = (filePath, destinationFilePath, progressBarPage, dataProcessingToolPage))
        #start threads
        downloadThread.start()
        progressBarThread.start()
        #hide main data processing page to prevent people from breaking things
        dataProcessingToolPage.withdraw()
        #run update buttons to make sure everything's in the right place once the page comes back (it'll be unhidden by progress bar thread once it sees that the file has finished being downloaded
        updateButtons()
        #get the file path for the config file included with the data
        configDST = os.getcwd() + "\\Configs\\" + os.path.basename(filePath)+ "Config.txt"
        #create a target file
        file = open(configDST, "w")
        file.close()
        #build out the source filepath
        configSRCLIST = filePath.split("/")
        configSRCLIST[-1] = "Config.txt"
        configSRC = "/".join(configSRCLIST)
        sourceFileSize = os.path.getsize(configSRC)
        #create the thread and download the config file (this isn't tracked since it's such a short download)
        configDownloadThread = threading.Thread(target = DataDownloader.downloadData, args = (configSRC, configDST))
        configDownloadThread.start()

    def binConvert():
        #update the buttons to allow the file to be operated on
        binThread = threading.Thread(target = BinFileTranslator.binConverter, args = (filePath,))
        #start the thread
        binThread.start()

    def processData():
        #create a page for the progress bar
        progressBarPage = tk.Toplevel(dataProcessingToolPage)
        progressBarPage.title("Translation Progress")
        progressBarPage.geometry("400x200")
        #create the thread
        dataProcessingThread = threading.Thread(target = DataTranslator.translateData, args = (filePath, progressBarPage, dataProcessingToolPage)) 
        #start the thread
        dataProcessingThread.start()
        #hide the main data processor page
        dataProcessingToolPage.withdraw()
        

    def calculateHertz():
        #open a hertz calculator page
        hertzCalculationPage = tk.Toplevel(dataProcessingToolPage)
        hertzCalculationPage.title("Hertz Calculator")
        hertzCalculationPage.geometry("400x200")
        #create the thread for hertz calculator
        hertzCalculatorThread = threading.Thread(target = hertzCalculator.calculateHertz, args = (filePath, hertzCalculationPage))
        #start the thread
        hertzCalculatorThread.start()

    def editConfig():
        #find the config file based on filePath variable
        configFilePathList = filePath.split("/")
        configFilePathList[-1] = "Configs/" + configFilePathList[-1] + "Config.txt"
        configFilePath = "/".join(configFilePathList)
        #open the config file in notepade (See if this works on mac.....)
        os.system(f'notepad.exe {configFilePath}')

    #howToButton
    howToButton = tk.Button(dataProcessingToolPage, text="How To", command=lambda: openHowTo())
    howToButton.pack()
    
    #create a button to select the file to be processed
    fileSelectButton = tk.Button(dataProcessingToolPage, text="Choose File", command=chooseFile)
    fileSelectButton.pack(pady=10)
  
    #create a label to show the file selected
    fileSelectLabel = tk.Label(dataProcessingToolPage, text="No file selected")
    fileSelectLabel.pack(pady=5)
    
    #Create a frame for the buttons
    buttonFrame = tk.Frame(dataProcessingToolPage)
    buttonFrame.pack(pady=20)

    #Create and place the buttons in a single row on the second page
    downloadButton = tk.Button(buttonFrame, text="Download Data File", command=lambda: downloadData())
    binButton = tk.Button(buttonFrame, text = "Convert .bin to .txt", command=lambda: binConvert())
    accelButton = tk.Button(buttonFrame, text="Acceleration", command=lambda: acceleration())
    brakeButton = tk.Button(buttonFrame, text="Brake Pressure", command=lambda: brakepressure())
    rpmButton = tk.Button(buttonFrame, text="RPM", command=lambda: rpm())
    processButton = tk.Button(buttonFrame, text="Process Data", command=lambda: processData())
    configEditButton = tk.Button(buttonFrame, text="Edit Config", command=lambda: editConfig())
    herztCalculatorButton = tk.Button(buttonFrame, text="Calculate Hertz Info", command=lambda: calculateHertz())
    updateButtons()

def runUpdater():
    #create update thread
    updateThread = threading.Thread(target = DataTranslatorUpdater.runUpdater, args = ())
    #destroy the root page
    root.destroy()
    #start the thread
    updateThread.start()

def acceleration():
    accelThread = threading.Thread(target = AccelerationVisualizer.accel, args = (filePath,))
    accelThread.start()
def brakepressure():
    brakeThread = threading.Thread(target = BrakePressureVisualizer.brake, args = (filePath,))
    brakeThread.start()
def rpm():
    rpmThread = threading.Thread(target = RPMVisualizer.rpm, args = (filePath,))
    rpmThread.start()

def openHowTo():
    #find the HomeScreen.txt file
    howToFilePath = "Guides/HomeScreen.txt"
    #open the howToFile file in notepade (See if this works on mac.....)
    os.system(f'notepad.exe {howToFilePath}')

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
dataProcessingToolButton = tk.Button(buttonFrame, text="Data Tool", command=lambda: dataProcessingTool())
dataProcessingToolButton.grid(row=1, column=0, padx=20)

#Button for visuals
"""
accelButton = tk.Button(buttonFrame, text="Acceleration", command=lambda: acceleration())
accelButton.grid(row=1, column=1, padx=20)
brakeButton = tk.Button(buttonFrame, text="Brake Pressure", command=lambda: brakepressure())
brakeButton.grid(row=1, column=2, padx=20)
rpmButton = tk.Button(buttonFrame, text="RPM", command=lambda: rpm())
rpmButton.grid(row=1, column=3, padx=20)
"""

# Create button 2
updaterButton = tk.Button(buttonFrame, text="Update Program", command=lambda: runUpdater())
updaterButton.grid(row=1, column=4, padx=20)

# Create Button 3
howToButton = tk.Button(buttonFrame, text="How To", command=lambda: openHowTo())
howToButton.grid(row=1, column=5, padx=20)

# Run the application
root.mainloop()
