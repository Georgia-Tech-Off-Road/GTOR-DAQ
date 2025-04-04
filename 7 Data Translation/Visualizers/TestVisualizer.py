import warnings
import pandas as pd
import matplotlib.pyplot as plt
import tkinter as tk
from tkinter import ttk

def testVisualizer(filePath, columnName, customWindow):
    #create a label to say Analyzing File
    label1 = tk.Label(customWindow, text="Creating your graph...")
    label1.pack()
    #create a progress bar that just bounces back and forth
    progressBar = ttk.Progressbar(customWindow, mode = "indeterminate", maximum=100)
    progressBar.pack(padx=20, pady=20, fill="x")
    #start the bouncing
    progressBar.start()

    df = pd.read_csv(filePath, delimiter=',')
    #df = df[~df.iloc[:, columnName].between(65534.99999999999999999999, 65535.1)]
    time = df.iloc[:, 1]/(10**6)

    if len(columnName) == 1:
        val = df.iloc[:, columnName[0]]
        plt.figure()
        plt.plot(time, val, label='Value')
        plt.title('Graph!')
        plt.ylabel('Value')
        plt.xlabel('Time (Seconds)')
        plt.grid(True)
        plt.legend()

        customWindow.destroy()
        plt.show()
    elif len(columnName) == 2:
        val1 = df.iloc[:, columnName[0]]
        val2 = df.iloc[:, columnName[1]]
        plt.figure()
        plt.plot(time, val1, label = 'Value 1')
        plt.plot(time, val2, label = 'Value 2')
        plt.title('DOUBLE GRAPH!')
        plt.ylabel('Values')
        plt.xlabel('Time (Seconds)')
        plt.grid(True)
        plt.legend()

        customWindow.destroy()
        plt.show()
    else:
        print("Do a max of 2 columns only...")
