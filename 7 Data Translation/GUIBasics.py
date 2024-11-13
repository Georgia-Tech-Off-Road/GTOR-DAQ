import tkinter as tk
from tkinter import filedialog
from tkinter import ttk
import os
import importlib
import threading
import time


os.chdir("./")
for fileName in os.listdir("ProcessingPrograms"):
        if fileName.endswith('.py') and not fileName.startswith('__'):
            moduleName = fileName[:-3]  # Remove .py extension
            module = importlib.import_module(f'ProcessingPrograms.{moduleName}')
            globals()[moduleName] = module
from DataDownloader import DataDownloader

# Create the main window
root = tk.Tk()
root.title("Main Page")
root.geometry("400x200")


# Function to go to the second page
def openDataDownloaderTool():
    root.withdraw()
    # Create a new window for the second page
    second_page = tk.Toplevel(root)
    second_page.title("Data Downloader Tool")
    second_page.geometry("800x400")
    # Function to handle file upload
    def chooseFile():
        # Open file dialog and get the file path
        global filePath
        filePath = filedialog.askopenfilename()
        # Display the file path in the label
        file_upload_label.config(text=f"Selected file: {filePath}")
        updateButtons()

    def updateButtons():
        for frame in second_page.winfo_children():
            if isinstance(frame, tk.Frame):
                for widget in frame.winfo_children():
                    if widget["text"] != "Choose File":
                            widget.grid_forget()
        if ('filePath' in globals()):
                if "C:/" not in filePath:
                        button4.grid(row=0, column=1, padx=20)
                        button5.grid(row=1, column=0, padx=20)
                        button6.grid(row=1, column=1, padx=20)
                        button7.grid(row=1, column=2, padx=20)
                else: 
                        button5.grid(row=0, column=0, padx=20)
                        button6.grid(row=0, column=1, padx=20)
                        button7.grid(row=0, column=2, padx=20)
    def downloadData():
        #create a progress bar
        progressBarPage = tk.Toplevel(second_page)
        progressBarPage.title("Download Progress")
        progressBarPage.geometry("400x200")
        label = tk.Label(progressBarPage, text="Download Progress")
        label.pack()
        progressBar = ttk.Progressbar(progressBarPage, mode = "determinate", maximum=100)
        progressBar.pack(padx=20, pady=20, fill="x")
        destinationFilePath = str(os.getcwd()+ "\\"+ os.path.basename(filePath))
        file = open(os.path.basename(destinationFilePath), "a")
        file.close()
        #use multithreading to have download run in background while gui still updates in foreground
        downloadThread = threading.Thread(target = DataDownloader.downloadData, args = (filePath, destinationFilePath))
        progressBarThread = threading.Thread(target = DataDownloader.updateProgressBar, args = (filePath, destinationFilePath, progressBar, progressBarPage, label, second_page))
        downloadThread.start()
        progressBarThread.start()
        second_page.withdraw()
        updateButtons()
        configDST = os.getcwd() + "\\Configs\\" + os.path.basename(filePath)+ "Config.txt"
        file = open(configDST, "w")
        file.close()
        configSRCLIST = filePath.split("/")
        configSRCLIST[-1] = "Config.txt"
        configSRC = "/".join(configSRCLIST)
        sourceFileSize = os.path.getsize(configSRC)
        configDownloadThread = threading.Thread(target = DataDownloader.downloadData, args = (configSRC, configDST))
        configDownloadThread.start()

    def processData():
        progressBarPage = tk.Toplevel(second_page)
        progressBarPage.title("Translation Progress")
        progressBarPage.geometry("400x200")
        label = tk.Label(progressBarPage, text="Data Translation Progress")
        label.pack()
        progressBar = ttk.Progressbar(progressBarPage, mode = "determinate", maximum=100)
        progressBar.pack(padx=20, pady=20, fill="x")
        dataProcessingThread = threading.Thread(target = DataTranslator.translateData, args = (filePath, progressBar, progressBarPage, label, second_page)) 
        dataProcessingThread.start()
        second_page.withdraw()
        

    def calculateHertz():
        hertzCalculationPage = tk.Toplevel(second_page)
        hertzCalculationPage.title("Hertz Calculator")
        hertzCalculationPage.geometry("400x200")
        hertzCalculatorThread = threading.Thread(target = hertzCalculator.calculateHertz, args = (filePath, hertzCalculationPage)) 
        hertzCalculatorThread.start()

    # Function to run when each button on the second page is clicked
    def editConfig():
        configFilePathList = filePath.split("/")
        configFilePathList[-1] = "Configs/" + configFilePathList[-1] + "Config.txt"
        configFilePath = "/".join(configFilePathList)
        os.system(f'notepad.exe {configFilePath}')

    # File upload section
    file_upload_button = tk.Button(second_page, text="Choose File", command=chooseFile)
    file_upload_button.pack(pady=10)

    file_upload_label = tk.Label(second_page, text="No file selected")
    file_upload_label.pack(pady=5)
    # Create a frame for the buttons on the second page
    second_frame = tk.Frame(second_page)
    second_frame.pack(pady=20)

    # Create and place the buttons in a single row on the second page
    button4 = tk.Button(second_frame, text="Download Data File (Recommended as it should make data processing faster)", command=lambda: downloadData())
    button5 = tk.Button(second_frame, text="Process Data", command=lambda: processData())
    button6 = tk.Button(second_frame, text="Edit Config", command=lambda: editConfig())
    button7 = tk.Button(second_frame, text="Calculate Hertz Info", command=lambda: calculateHertz())
    updateButtons()

# Create labels and buttons on the main page in a single row
frame = tk.Frame(root)
frame.pack(pady=20)

# Create label and button for Button 1
label1 = tk.Label(frame, text="Label 1")
label1.grid(row=0, column=0, padx=20)
button1 = tk.Button(frame, text="Data Tool", command=lambda: openDataDownloaderTool())
button1.grid(row=1, column=0, padx=20)

# Create label and button for Button 2
label2 = tk.Label(frame, text="Label 2")
label2.grid(row=0, column=1, padx=20)
button2 = tk.Button(frame, text="Button 2", command=lambda: on_main_button_click(2))
button2.grid(row=1, column=1, padx=20)

# Create label and button for Button 3
label3 = tk.Label(frame, text="Label 3")
label3.grid(row=0, column=2, padx=20)
button3 = tk.Button(frame, text="Button 3", command=lambda: on_main_button_click(3))
button3.grid(row=1, column=2, padx=20)

# Run the application
root.mainloop()
