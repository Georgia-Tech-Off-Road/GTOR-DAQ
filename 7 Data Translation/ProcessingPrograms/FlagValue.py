import warnings
import pandas as pd
import matplotlib.pyplot as plt
import tkinter as tk
from tkinter import ttk

def flag(filePath, flagPage):
    #create a label to say Analyzing File
    label1 = tk.Label(flagPage, text="Analyzing RPM values...")
    label1.pack()
    #create a progress bar that just bounces back and forth
    progressBar = ttk.Progressbar(flagPage, mode = "indeterminate", maximum=100)
    progressBar.pack(padx=20, pady=20, fill="x")
    #start the bouncing
    progressBar.start()

    flagPage.update()  # ensure GUI updates before file reading starts

    found3 = None
    found4 = None

    with open(filePath, 'r', encoding='utf-8', errors='ignore') as f:
        header = f.readline()
        for i, line in enumerate(f, start=1):
            parts = line.strip().split(',')
            if len(parts) > 4:
                if found3 is None and float(parts[3]) != 0:
                    found3 = i
                if found4 is None and float(parts[4]) != 0:
                    found4 = i
                if found3 is not None and found4 is not None:
                    break

    progressBar.stop()
    progressBar.destroy()
    label1.config(text="Done analyzing.")

    result1 = tk.Label(flagPage, text=f"First nonzero at index 3 on row: {found3}")
    result1.pack()

    result2 = tk.Label(flagPage, text=f"First nonzero at index 4 on row: {found4}")
    result2.pack()
