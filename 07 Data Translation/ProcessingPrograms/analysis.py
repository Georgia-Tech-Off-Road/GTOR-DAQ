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
from .fancyCudaStuff import cudaConvolver

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
                df[outputEntry.get()] = convovledArray
        else:
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

def save(df, anaPage):
    df.to_json('output.json', orient='records')
    anaPage.destroy()

    
    
    


   
