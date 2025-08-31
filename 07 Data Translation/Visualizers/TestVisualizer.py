import warnings
import pandas as pd
import matplotlib.pyplot as plt
import tkinter as tk
from tkinter import ttk
import csv
import json

def testVisualizer(filePath,columnIndices,customWindow,useDefaultConfig):
    # Create UI indicator
    label1 = tk.Label(customWindow, text="Creating your graph...")
    label1.pack()

    progressBar = ttk.Progressbar(customWindow, mode="indeterminate", maximum=100)
    progressBar.pack(padx=20, pady=20, fill="x")
    progressBar.start()

    # Read file data
    with open(filePath,"r") as infile:
        text = infile.read()
    data = json.loads(text)
    df = pd.json_normalize(data)

    time = df["millisec"] #if the code errors, change this to ["millisec"]

    plt.figure()
    for colIndex in columnIndices:
        if colIndex < df.shape[1]:
            label = df.columns[colIndex]
        else:
            label = f"Column {colIndex}"

        plt.plot(time, df.iloc[:, colIndex], label=label)

    plt.title('Data Visualizer')
    plt.ylabel('Sensor Value')
    plt.xlabel('Time (Seconds)')
    plt.grid(True)
    plt.legend()

    plt.show()
    customWindow.destroy()


