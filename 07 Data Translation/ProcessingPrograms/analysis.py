import warnings
import pandas as pd
import matplotlib.pyplot as plt
import tkinter as tk
from tkinter import ttk
import csv
import numpy as np
import json
import plotly.express as px

def analyzeData(filePath, colNum, anaPage):
    # Create UI indicator
    label1 = tk.Label(anaPage, text="Creating your graph...")
    label1.pack()

    # Create progress bar
    progressBar = ttk.Progressbar(anaPage, mode="indeterminate", maximum=100)
    progressBar.pack(padx=20, pady=20, fill="x")
    progressBar.start()

    with open(filePath, "r") as infile:
        text = infile.read()
    data = json.loads(text)
    df = pd.json_normalize(data)
    print(df.iloc[:,colNum].values.tolist())
    anaPage.destroy()

