import tkinter as tk
import time
from datetime import datetime
from tkinter import ttk
import threading
from EngineDynoSupporterCode import Globals

calibrationInProgress = True

def createCSVFile(timeToKill):
    file = open(datetime.now().strftime("%Y-%m-%d_%H-%M-%S") + ".csv", "w")
    assemblyString = ""
    startTime = time.perf_counter()
    while True:
        if Globals.engineDyno.in_waiting > 0:
            assemblyString += Globals.engineDyno.read().decode('utf-8')
            #print(assemblyString)
            if "\n" in assemblyString:
                file.write(assemblyString.split("\n")[0] + "\n")
                assemblyString = assemblyString.split("\n")[1]
            if startTime + 20 < time.perf_counter():
                file.close()
                break

def getConfigSerialOut(textBox):
    assemblyString = ""
    while True:
        if Globals.engineDyno.in_waiting > 0:
            assemblyString += Globals.engineDyno.read().decode('utf-8')
            if "\n" in assemblyString:
                textBox.insert(tk.END, assemblyString.split("\n")[0] + "\n")
                assemblyString = assemblyString.split("\n")[1]
                textBox.see(tk.END)
        if not calibrationInProgress:
            break

def increaseCalibrationFactor():
    Globals.engineDyno.write(b'+')

def decreaseCalibrationFactor():
    Globals.engineDyno.write(b'-')

def endCalibration():
    global calibrationInProgress
    Globals.engineDyno.write(b'~')
    calibrationInProgress = False
                
            
def startDynoRun(parentPage):
    #hide parent page
    parentPage.withdraw()
    #create page for intial scale calibration
    calibrationPage = tk.Toplevel(parentPage)
    calibrationPage.title("Load Cell Calibration")
    calibrationPage.geometry("900x1000")
    #create a text box for the serial output
    serialOutput = tk.Text(calibrationPage, height =30, width = 100)
    serialOutput.see(tk.END)
    serialOutput.pack(padx = 10, pady = 10)
    #create a frame for the buttons
    buttonFrame = tk.Frame(calibrationPage)
    buttonFrame.pack(pady=20)
    #create increase and decrease buttons
    increaseButton = tk.Button(buttonFrame, text="Increase Calibration Factor", command = lambda : increaseCalibrationFactor())
    increaseButton.pack(padx = 5)
    decreaseButton = tk.Button(buttonFrame, text="Decrease Calibration Factor", command = lambda : decreaseCalibrationFactor())
    decreaseButton.pack(padx = 5)
    endCalibrationButton = tk.Button(buttonFrame, text="End Calibration", command = lambda : endCalibration())
    endCalibrationButton.pack(padx = 5)
    #create a thread to update the textbox
    serialOutThread = threading.Thread(target = getConfigSerialOut, args =(serialOutput,))
    serialOutThread.start()
    while calibrationInProgress:
        time.sleep(1)
        continue
    time.sleep(2)
    calibrationPage.destroy()
    #create thread for input taker/CSV file creator
    inputTakerThread = threading.Thread(target = createCSVFile, args =(20,))
    inputTakerThread.start()
    #create progress bar page
    progressBarPage = tk.Toplevel(parentPage)
    progressBarPage.title("Dyno Run Progress")
    progressBarPage.geometry("400x200")
    #create a label to say how far along the run is
    dynoProgressRunLabel = tk.Label(progressBarPage, text="Taking Data")
    dynoProgressRunLabel.pack()
    #create the progress bar itself
    progressBar = ttk.Progressbar(progressBarPage, mode = "determinate", maximum=100)
    progressBar.pack(padx=20, pady=20, fill="x")
    #get start time
    startTime = time.perf_counter()
    while startTime + 20 > time.perf_counter():
        #compute the percentage downloaded
        percentage = ((time.perf_counter() - startTime) / 20) * 100
        #update the progress bar to match the percentage
        progressBar["value"] = percentage
        #update the label to display the percentage downloaded
        dynoProgressRunLabel.config(text = str("Run Progress " + str(round(percentage,2)) + "%"))
        #pack both elements
        dynoProgressRunLabel.pack()
        progressBar.pack()
        #update the progressBar page to show the label and progress bar
        progressBarPage.update()
    #destroy the progress bar page
    progressBarPage.destroy()
    #unhide the main data processor page
    parentPage.deiconify()
    Globals.engineDyno.write(b'~')
    calibrationInProgress = True
