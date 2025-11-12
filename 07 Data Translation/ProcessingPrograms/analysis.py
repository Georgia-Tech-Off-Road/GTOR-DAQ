import warnings
import pandas as pd
import matplotlib.pyplot as plt
import tkinter as tk
from tkinter import ttk
import csv
import numpy as np
import json
import plotly.express as px
import ctypes
import threading
import os
import ctypes
import struct
import sys
import ctypes
import os
from tkinter import filedialog


dfRAM = None
df_lock = threading.Lock()

def ram(filePath, anaPage):

    progressBar = ttk.Progressbar(anaPage, mode="indeterminate", maximum=100)
    progressBar.pack(padx=20, pady=20, fill="x")
    progressBar.start()

    with open(filePath, "r") as infile:
        text = infile.read()
    data = json.loads(text)
    df = pd.json_normalize(data)

    global dfRAM
    with df_lock:
        dfRAM = df

    progressBar.destroy()
    return df

def kernel(df, anaPage, cuda):
    convolutionPage = tk.Toplevel()
    convolutionPage.title("Convolution Tool")
    convolutionPage.geometry("500x600")
    #define kernel options
    kernelOptions = ["Gaussian/Normal", "Equal Weight"]
    #define checkBox variables
    checkBoxValues=[]
    label = tk.Label(convolutionPage, text="Enter 1 column index\nNote that index 0 = column #1, sensor data usually starts at 2.")
    label.pack(pady=5)
    colEntry = tk.Entry(convolutionPage)
    colEntry.pack(pady=5)
    kerLab = tk.Label(convolutionPage, text="Enter kernel length")
    kerLab.pack(pady=5)
    kernelEntry = tk.Entry(convolutionPage)
    kernelEntry.pack(pady=5)
    outputLabel = tk.Label(convolutionPage, text="Output Name: Leave blank to overwrite original column data")
    outputLabel.pack(pady=5)
    outputEntry = tk.Entry(convolutionPage)
    outputEntry.pack(pady=5)
    #make kernel stuff checkboxes
    for kernelOption in kernelOptions:
        checkBoxValue = tk.BooleanVar()
        checkbox = tk.Checkbutton(convolutionPage, text=kernelOption, variable = checkBoxValue)
        checkbox.pack(anchor="w", padx = 20, pady = 2)
        checkBoxValues.append(checkBoxValue)
    def performConvolution():
        colNum = int(colEntry.get())
        kernelSize = int(kernelEntry.get())
        if kernelSize % 2 == 0:
            kernelSize += 1  # make sure it's odd
        kernel = np.zeros(kernelSize, dtype=np.float32)
        # Convert column to numpy array (float32)
        if (checkBoxValues[0].get() == True):
            #little chatgpt thing to create ze kernel
            # Parameters
            n = kernelSize        # length of the kernel
            mu = n // 2    # center of the Gaussian
            sigma = n / 10 # standard deviation

            # Generate Gaussian values
            x = np.arange(n)
            kernel = np.exp(-0.5 * ((x - mu) / sigma) ** 2).astype(np.float32)

            # Normalize to sum to 1
            kernel /= kernel.sum()
        elif (checkBoxValues[1].get() == True):
            kernel = np.ones(kernelSize, dtype = np.float32)
            kernel /= kernel.sum()
        label1 = tk.Label(convolutionPage, text="Convolution in progress...")
        label1.pack()
        progressBar = ttk.Progressbar(convolutionPage, mode="indeterminate", maximum=100)
        progressBar.pack(padx=20, pady=20, fill="x")
        progressBar.start()
        if not cuda:
            # Perform convolution
            columnArray = df.iloc[:, int(colNum)].to_numpy(dtype=np.float32)
            convolvedArray = np.convolve(columnArray, kernel, mode="same")
            if outputEntry.get() == "":
                df.iloc[:, int(colNum)] = convolvedArray
            else:
                df[outputEntry.get()] = convolvedArray
        else:
            from .fancyCudaStuff import cudaConvolver
            columnArray = np.ascontiguousarray(df.iloc[:, int(colNum)].to_numpy(dtype=np.float32))
            cudaConvolver.run(columnArray, kernel)
            if outputEntry.get() == "":
                df.iloc[:, int(colNum)] = columnArray
            else:
                df[outputEntry.get()] = columnArray


        convolutionPage.destroy()
        return
    startConvolutionButton = tk.Button(convolutionPage, text="StartConvolution!",command=performConvolution)
    startConvolutionButton.pack(pady=5)

def rpmSpikeFilter(df, anaPage, cuda):
    rpmSpikeFilterPage = tk.Toplevel()
    rpmSpikeFilterPage.title("RPM Spike Filter")
    rpmSpikeFilterPage.geometry("900x800")
    #define kernel options
    kernelOptions = ["Gaussian/Normal", "Equal Weight"]
    #define checkBox variables
    checkBoxValues=[]
    label = tk.Label(rpmSpikeFilterPage, text="Enter 1 column index\nNote that index 0 = column #1, sensor data usually starts at 2.")
    label.pack(pady=5)
    colEntry = tk.Entry(rpmSpikeFilterPage)
    colEntry.pack(pady=5)
    kerLab = tk.Label(rpmSpikeFilterPage, text="Enter kernel length (max 99)")
    kerLab.pack(pady=5)
    kernelEntry = tk.Entry(rpmSpikeFilterPage)
    kernelEntry.pack(pady=5)
    percentDiff = tk.Label(rpmSpikeFilterPage, text="Enter percent diff (1.05 translates to if slope greater than 5 percent greater)")
    percentDiff.pack(pady=5)
    percentDiffEntry = tk.Entry(rpmSpikeFilterPage)
    percentDiffEntry.pack(pady=5)
    timesToRun = tk.Label(rpmSpikeFilterPage, text="Enter times to run")
    timesToRun.pack(pady=5)
    timesToRunEntry = tk.Entry(rpmSpikeFilterPage)
    timesToRunEntry.pack(pady=5)
    timesToRunEntry.insert(0, "1")
    outputLabel = tk.Label(rpmSpikeFilterPage, text="Output Name: Leave blank to overwrite original column data")
    outputLabel.pack(pady=5)
    outputEntry = tk.Entry(rpmSpikeFilterPage)
    outputEntry.pack(pady=5)
    #make kernel stuff checkboxes
    for kernelOption in kernelOptions:
        checkBoxValue = tk.BooleanVar()
        checkbox = tk.Checkbutton(rpmSpikeFilterPage, text=kernelOption, variable = checkBoxValue)
        checkbox.pack(anchor="w", padx = 20, pady = 2)
        checkBoxValues.append(checkBoxValue)
    def filterRPMSpikes():
        colNum = int(colEntry.get())
        kernelSize = int(kernelEntry.get())
        timesToRunValue = np.int32(timesToRunEntry.get())
        percentDiffValue = np.float32(percentDiffEntry.get())
        if kernelSize % 2 == 0:
            kernelSize += 1  # make sure it's odd
        kernel = np.zeros(kernelSize, dtype=np.float32)
        # Convert column to numpy array (float32)
        if (checkBoxValues[0].get() == True):
            #little chatgpt thing to create ze kernel
            # Parameters
            n = kernelSize        # length of the kernel
            mu = n // 2    # center of the Gaussian
            sigma = n / 10 # standard deviation

            # Generate Gaussian values
            x = np.arange(n)
            kernel = np.exp(-0.5 * ((x - mu) / sigma) ** 2).astype(np.float32)

            # Normalize to sum to 1
            kernel /= kernel.sum()
        elif (checkBoxValues[1].get() == True):
            kernel = np.ones(kernelSize, dtype = np.float32)
            kernel /= kernel.sum()
        label1 = tk.Label(rpmSpikeFilterPage, text="Convolution in progress...")
        label1.pack()
        progressBar = ttk.Progressbar(rpmSpikeFilterPage, mode="indeterminate", maximum=100)
        progressBar.pack(padx=20, pady=20, fill="x")
        progressBar.start()
        if not cuda:
            # Perform convolution
            label2 = tk.Label(rpmSpikeFilterPage, text="Sorry! This currently isn't supported without cuda.")
            label2.pack()
        else:
            from .fancyCudaStuff import RPMSpikeFilter
            columnArray = np.ascontiguousarray(df.iloc[:, int(colNum)].to_numpy(dtype=np.float32))
            RPMSpikeFilter.run(columnArray, kernel, percentDiffValue, timesToRunValue)
            if outputEntry.get() == "":
                df.iloc[:, int(colNum)] = columnArray
            else:
                df[outputEntry.get()] = columnArray


        rpmSpikeFilterPage.destroy()
        return
    startConvolutionButton = tk.Button(rpmSpikeFilterPage, text="Filter RPM!",command=filterRPMSpikes)
    startConvolutionButton.pack(pady=5)

def save(df, anaPage):
    savePage = tk.Toplevel()
    savePage.title("Save File")
    savePage.geometry("500x600")

    #define variables
    filePath = os.getcwd()
    fileName = "output"

    fileSelectLabel = tk.Label(savePage, text = f"Selected filePath: {filePath}")
    fileSelectLabel.pack(pady = 20)
    def saveFile():
        df.to_json(filePath + fileName + ".txt", orient='records')
        savePage.destroy()
        anaPage.destroy()
    def chooseFilePath():
        filePath = filedialog.askdirectory()
        # Display the file path in the label
        fileSelectLabel.config(text=f"Selected filePath: {filePath}")
    fileSelectButton = tk.Button(savePage, text="Choose File Path", command=chooseFilePath)
    fileSelectButton.pack(pady = 5)
    label = tk.Label(savePage, text="Enter File Name.")
    label.pack(pady=5)
    colEntry = tk.Entry(savePage)
    colEntry.pack(pady=5)
    saveButton = tk.Button(savePage, text="Save File", command = saveFile)
    saveButton.pack()
    

    
    
    


   
