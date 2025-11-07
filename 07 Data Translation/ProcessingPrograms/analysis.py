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
    print("Successfully loaded to RAM!")
    return df

def kernel(df, colNum, kernelSize, anaPage):
    label1 = tk.Label(anaPage, text="Convolution in progress...")
    label1.pack()
    progressBar = ttk.Progressbar(anaPage, mode="indeterminate", maximum=100)
    progressBar.pack(padx=20, pady=20, fill="x")
    progressBar.start()


    # Convert column to numpy array (float64)
    columnArray = df.iloc[:, int(colNum)].to_numpy(dtype=np.float64)
    arrayLength = len(columnArray)

    # C-style pointer
    arrayPointer = columnArray.ctypes.data_as(ctypes.POINTER(ctypes.c_double))

    print("Array length:", arrayLength)
    print("Pointer address:", arrayPointer)

    # Create convolution kernel (9000-point moving average)
    if kernelSize % 2 == 0:
        kernelSize += 1  # make sure it's odd
    kernel = np.ones(kernelSize) / kernelSize

    # Perform convolution
    convolvedArray = np.convolve(columnArray, kernel, mode="same")

    print("Convolved array length:", len(convolvedArray))

    anaPage.destroy()
