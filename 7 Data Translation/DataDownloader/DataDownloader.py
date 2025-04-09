import shutil
import os
import time
import tkinter as tk
from tkinter import ttk


#handles data download and includes basic progress bar
def downloadData(src, dst, outputPath, chosePath):
    global filePath
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
            #increments numMegs to try and find optimum download rate
            if not wait:
                numMegs += 1
    print(f"Successfully downloaded file as {dst}")

def updateProgressBar(src, dst, progressBarPage, parentPage, outputPath, chosePath):
    #get the basePath
    basePath = os.getcwd()
    #get the size of the source file
    sourceFileSize = os.path.getsize(src)
    #create a label to say how far along the download is
    downloadProgressLabel = tk.Label(progressBarPage, text="Download Progress")
    downloadProgressLabel.pack()
    #create the progress bar itself
    progressBar = ttk.Progressbar(progressBarPage, mode = "determinate", maximum=100)
    progressBar.pack(padx=20, pady=20, fill="x")
    #get the destination file path by appending the file name to the current file path
    #while the sizes aren't identical
    while os.path.getsize(dst) != sourceFileSize:
        #compute the percentage downloaded
        percentage = os.path.getsize(dst)/sourceFileSize * 100
        #update the progress bar to match the percentage
        progressBar["value"] = percentage
        #update the label to display the percentage downloaded
        downloadProgressLabel.config(text = str("Download Progress " + str(round(percentage,2)) + "%"))
        #pack both elements
        downloadProgressLabel.pack()
        progressBar.pack()
        #update the progressBar page to show the label and progress bar
        progressBarPage.update()
    #destry the progress bar page
    progressBarPage.destroy()
    #unhide the main data processor page
    parentPage.deiconify()
    
    
    
            
