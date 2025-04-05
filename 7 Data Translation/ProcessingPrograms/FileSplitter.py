import warnings
import pandas as pd
import matplotlib.pyplot as plt
import tkinter as tk
from tkinter import ttk

def split(filePath, fileSplitPage):
    #create a label to say Analyzing File
    label1 = tk.Label(fileSplitPage, text="Splitting files...")
    label1.pack()
    #create a progress bar that just bounces back and forth
    progressBar = ttk.Progressbar(fileSplitPage, mode = "indeterminate", maximum=100)
    progressBar.pack(padx=20, pady=20, fill="x")
    #start the bouncing
    progressBar.start()

    lines_per_file = 10000000 #1,000,000
    file_count = 1
    line_count = 0
    output = None

    with open(filePath, 'r', encoding='utf-8', errors='ignore') as f:
        header = f.readline()
        for i, line in enumerate(f, start=1):
            if (i - 1) % lines_per_file == 0:
                if output:
                    output.close()
                output = open(f"{filePath}_part{file_count}.txt", 'w', encoding='utf-8')
                output.write(header)
                file_count += 1
            output.write(line)
    if output:
        output.close()
    fileSplitPage.destroy()
