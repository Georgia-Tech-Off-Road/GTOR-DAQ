import warnings
import pandas as pd
import matplotlib.pyplot as plt
import tkinter as tk
from tkinter import ttk
import csv
import numpy as np
import json
import plotly.express as px
from scipy.signal import savgol_filter

def testVisualizer(filePath, columnIndices, customWindow, useDefaultConfig,
                   plotlyCheckVar, scale,
                   smoothingWindow=11, polyorder=2,
                   normalVis=False, smoothVis=False, overlayVis=False, wftFilePath=None):

    # Create UI indicator
    label1 = tk.Label(customWindow, text="Creating your graph...")
    label1.pack()

    # Create progress bar
    progressBar = ttk.Progressbar(customWindow, mode="indeterminate", maximum=100)
    progressBar.pack(padx=20, pady=20, fill="x")
    progressBar.start()

    # Read main file
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
    time = df['microsec'] / 1e6
    time = time.round(2)  # rounding to hundreds place for merge

    # Correct brake pressure
    for col in df.columns:
        if 'brake' in col.lower() or 'break' in col.lower():
            df[col] = (.5 + (df[col] / (2000 - 50)) * (4.5 - .5))
            df[col] = 500*(df[col]-0.5)
            if scale != 1:
                df[col] = df[col]*scale
                df.rename(columns={col: f"{col} (SCALED BY {scale})"}, inplace=True)

    # Build initial plot df
    plotDf = pd.DataFrame({'Time': time})

    # Merge wftFilePath if provided
    if wftFilePath is not None:
        df2 = pd.read_csv(wftFilePath, sep='\t')
        df2['Time'] = df2['Time'].round(2)
        plotDf = pd.merge(plotDf, df2, on='Time', how='left')

    # Apply Savitzky-Golay smoothing to selected columns
    for colIndex in columnIndices:
        if colIndex < df.shape[1]:
            label = df.columns[colIndex]
            series = df[label].copy().fillna(method='ffill').fillna(method='bfill')

            win = min(smoothingWindow, len(series))
            if win % 2 == 0:
                win -= 1
            if win < 3:
                win = 3

            smoothed = pd.Series(
                savgol_filter(series, window_length=win, polyorder=polyorder),
                index=series.index
            )

            if normalVis:
                plotDf[label] = series
            elif smoothVis:
                plotDf[label] = smoothed
            elif overlayVis:
                plotDf[label + " (Original)"] = series
                plotDf[label + " (Smoothed)"] = smoothed
        else:
            plotDf[f"Column{colIndex}"] = np.nan

    # Plotting
    if plotlyCheckVar == 1:
        fig = px.line(plotDf, x='Time', y=plotDf.columns[1:],
                      labels={'value':'Sensor Value','Time':'Time (Seconds)'},
                      title='Data Visualizer')
        fig.show()
    else:
        plt.figure()
        for colIndex in columnIndices:
            if colIndex < df.shape[1]:
                if overlayVis:
                    origLabel = df.columns[colIndex] + " (Original)"
                    smoothLabel = df.columns[colIndex] + " (Smoothed)"
                    plt.plot(plotDf['Time'], plotDf[origLabel], label=origLabel, alpha=0.5)
                    plt.plot(plotDf['Time'], plotDf[smoothLabel], label=smoothLabel, linewidth=2)
                else:
                    label = df.columns[colIndex]
                    plt.plot(plotDf['Time'], plotDf[label], label=label)
            else:
                label = f"Column{colIndex}"
                plt.plot(plotDf['Time'], plotDf[label], label=label)

        # Plot all df2 columns if wftFilePath was used
        if wftFilePath is not None:
            for col in df2.columns:
                if col != 'Time':
                    plt.plot(plotDf['Time'], plotDf[col], label=col)

        plt.title('Data Visualizer')
        plt.ylabel('Sensor Value')
        plt.xlabel('Time (Seconds)')
        plt.grid(True)
        plt.legend()
        plt.show()

    customWindow.destroy()
