import warnings
import pandas as pd
import matplotlib.pyplot as plt
import tkinter as tk
from tkinter import ttk
import csv
import json
import os

def convertExcel(filePath,excelWindow,outputPath):
    label1 = tk.Label(excelWindow, text="Creating your spreadsheet...")
    label1.pack()
    #Create progress bar
    progressBar = ttk.Progressbar(excelWindow, mode="indeterminate", maximum=100)
    progressBar.pack(padx=20, pady=20, fill="x")
    progressBar.start()

    with open(filePath,"r") as infile:
        text = infile.read()
    data = json.loads(text)
    df = pd.json_normalize(data)

    if outputPath:
        output = os.path.splitext(os.path.basename(filePath))[0] + ".xlsx"
        outfile = os.path.normpath(os.path.join(outputPath, output))
        df.to_excel(str(outfile), index=False)
        print(f"Created Excel file, saved to {outfile}")
    else:
        df.to_excel(str(filePath)+".xlsx", index=False)
        filedest = str(filePath)+".xlsx"
        print(f"Created Excel file, saved to {filedest}")

    excelWindow.destroy()
