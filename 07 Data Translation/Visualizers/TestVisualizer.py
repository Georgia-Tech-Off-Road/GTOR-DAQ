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

def testVisualizer(df, filePath, columnIndices, customWindow, useDefaultConfig,
                   plotlyCheckVar, scale, makeHeatMap,
                   smoothingWindow=11, polyorder=2,
                   normalVis=False, smoothVis=False, overlayVis=False):

    if not df:
        with open(filePath, "r") as infile:
            text = infile.read()
        data = json.loads(text)
        df = pd.json_normalize(data)

    # Create UI indicator
    label1 = tk.Label(customWindow, text="Creating your graph...")
    label1.pack()

    progressBar = ttk.Progressbar(customWindow, mode="indeterminate", maximum=100)
    progressBar.pack(padx=20, pady=20, fill="x")
    progressBar.start()

    # Convert microsec to relative time
    startTime = df['microsec'].iloc[0]
    df['microsec'] = df['microsec'] - startTime
    time = df['microsec'] / 1e6

    # Correct brake pressure
    for col in df.columns:
        if 'brake' in col.lower() or 'break' in col.lower():
            df[col] = (.5 + (df[col] / (2000 - 50)) * (4.5 - .5))
            df[col] = 500*(df[col]-0.5)
            if scale != 1:
                df[col] = df[col]*scale
                df.rename(columns={col: f"{col} (SCALED BY {scale})"}, inplace=True)

    # Build plot DF
    plot_df = pd.DataFrame({'Time': time})

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
                plot_df[label] = series
            elif smoothVis:
                plot_df[label] = smoothed
            elif overlayVis:
                plot_df[label + " (Original)"] = series
                plot_df[label + " (Smoothed)"] = smoothed
        else:
            plot_df[f"Column_{colIndex}"] = np.nan

    ycols = [c for c in plot_df.columns if c != 'Time']

    if plotlyCheckVar == 1:
        fig = px.line(plot_df, x='Time', y=ycols,
                      labels={'value': 'Sensor Value', 'Time': 'Time (Seconds)'},
                      title='Data Visualizer')
        progressBar.stop()
        fig.show()
    else:
        plt.figure()
        for colName in ycols:
            plt.plot(plot_df['Time'], plot_df[colName], label=colName,
                     alpha=(0.7 if 'Original' in colName else 1.0))
        plt.title('Data Visualizer')
        plt.ylabel('Sensor Value')
        plt.xlabel('Time (Seconds)')
        plt.grid(True)
        plt.legend()
        progressBar.stop()
        plt.show()

    customWindow.destroy()

