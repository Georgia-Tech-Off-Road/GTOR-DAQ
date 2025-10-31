import warnings
import pandas as pd
import matplotlib.pyplot as plt
import tkinter as tk
from tkinter import ttk
import csv
import numpy as np
import json
import plotly.express as px

def testVisualizer(filePath, columnIndices, customWindow, useDefaultConfig, plotlyCheckVar,scale):
    # Create UI indicator
    label1 = tk.Label(customWindow, text="Creating your graph...")
    label1.pack()

    # Create progress bar
    progressBar = ttk.Progressbar(customWindow, mode="indeterminate", maximum=100)
    progressBar.pack(padx=20, pady=20, fill="x")
    progressBar.start()

    # Read file
    if '.xlsx' in filePath:
        df = pd.read_excel(filePath, engine='openpyxl')
    else:
        with open(filePath, "r") as infile:
            text = infile.read()
        data = json.loads(text)
        df = pd.json_normalize(data)

    # Convert microsec to relative time
    startTime = df['microsec'].iloc[0]
    df['microsec'] = df['microsec'] - startTime
    time = df['microsec'] / (1 * 10**6)

    # Correct brake pressure
    for col in df.columns:
        print(scale)
        print(type(scale))
        if 'brake' in col.lower() or 'break' in col.lower():
            df[col] = (.5 + (df[col] / (2000 - 50)) * (4.5 - .5)) #(TEMPORARY UNITL ANDREW FIXES)
            df[col] = 500*(df[col]-0.5) #(TEMPORARY UNITL ANDREW FIXES)
            if scale != 1:
                df[col] = df[col]*scale
                df.rename(columns={col: f"{col} (SCALED BY {scale})"}, inplace=True)

    # Set outliers to NaN (values beyond 6 std devs)
    '''
    for col in df.columns:
        if np.issubdtype(df[col].dtype, np.number):  # only numeric columns
            mean = df[col].mean()
            std = df[col].std()
            df.loc[(df[col] < mean - 6*std) | (df[col] > mean + 6*std), col] = np.nan
    '''

    # Plotting
    if plotlyCheckVar == 1:
        # Use Plotly
        plot_df = pd.DataFrame({'Time': time})
        for colIndex in columnIndices:
            if colIndex < df.shape[1]:
                label = df.columns[colIndex]
                plot_df[label] = df.iloc[:, colIndex]
        fig = px.line(plot_df, x='Time', y=plot_df.columns[1:], labels={'value':'Sensor Value','Time':'Time (Seconds)'}, title='Data Visualizer')
        fig.show()
    else:
        # Use Matplotlib
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
