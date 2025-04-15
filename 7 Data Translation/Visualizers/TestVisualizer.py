import warnings
import pandas as pd
import matplotlib.pyplot as plt
import tkinter as tk
from tkinter import ttk
import csv

def testVisualizer(filePath, columnIndices, customWindow, useDefaultConfig):
    # Determine config file path
    if useDefaultConfig == 1:
        configFilePath = "Configs/defaultConfig.txt"
    else:
        configFilePathList = filePath.split("/")
        configFilePathList[-1] = "Configs/" + configFilePathList[-1] + "Config.txt"
        configFilePath = "/".join(configFilePathList)

    # Read config file (skip headers)
    with open(configFilePath, 'r') as file:
        reader = csv.reader(file)
        configLines = list(reader)[2:]

    # Create UI indicator
    label1 = tk.Label(customWindow, text="Creating your graph...")
    label1.pack()

    progressBar = ttk.Progressbar(customWindow, mode="indeterminate", maximum=100)
    progressBar.pack(padx=20, pady=20, fill="x")
    progressBar.start()

    # Read CSV data
    df = pd.read_csv(filePath, delimiter=',')

    # Optional filter
   # df = df[~df.iloc[:, 13].between(60000, 70000)]

    # Time in seconds
    time = df.iloc[:, 1] / (10 ** 6)

    # Plotting
    plt.figure()
    for i, colIndex in enumerate(columnIndices):
        if colIndex < len(configLines):
            label = configLines[colIndex][2]  # Use index 2 (label)
        else:
            label = f"Column {colIndex}"
        plt.plot(time, df.iloc[:, colIndex], label=label)

    plt.title('Custom Visualizer')
    plt.ylabel('Sensor Value')
    plt.xlabel('Time (Seconds)')
    plt.grid(True)
    plt.legend()

    # Cleanup
    customWindow.destroy()
    plt.show()
