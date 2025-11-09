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

def testVisualizer(df, columnIndices, customWindow, useDefaultConfig,
                   plotlyCheckVar, scale,
                   smoothingWindow=11, polyorder=2,
                   normalVis=False, smoothVis=False, overlayVis=False):

    # Create UI indicator
    label1 = tk.Label(customWindow, text="Creating your graph...")
    label1.pack()

    # Create progress bar
    progressBar = ttk.Progressbar(customWindow, mode="indeterminate", maximum=100)
    progressBar.pack(padx=20, pady=20, fill="x")
    progressBar.start()

    # Read file
    # if '.xlsx' in filePath:
    #     df = pd.read_excel(filePath, engine='openpyxl')
    # else:
    #     with open(filePath, "r") as infile:
    #         text = infile.read()
    #     data = json.loads(text)
    #     df = pd.json_normalize(data)

    # Convert microsec to relative time
    startTime = df['microsec'].iloc[0]
    df['microsec'] = df['microsec'] - startTime
    time = df['microsec'] / 1e6

    # Correct brake pressure
    for col in df.columns:
        if 'brake' in col.lower() or 'break' in col.lower():
            df[col] = (.5 + (df[col] / (2000 - 50)) * (4.5 - .5))  # (TEMPORARY UNTIL ANDREW FIXES)
            df[col] = 500*(df[col]-0.5)  # (TEMPORARY UNTIL ANDREW FIXES)
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

    # Build plot DF & apply Savitzky-Golay smoothing
    plot_df = pd.DataFrame({'Time': time})
    for colIndex in columnIndices:
        if colIndex < df.shape[1]:
            label = df.columns[colIndex]
            series = df[label].copy().fillna(method='ffill').fillna(method='bfill')

            # Savitzky-Golay requires window <= series length
            win = min(smoothingWindow, len(series))
            if win % 2 == 0:
                win -= 1
            if win < 3:
                win = 3

            smoothed = pd.Series(
                savgol_filter(series, window_length=win, polyorder=polyorder),
                index=series.index
            )

            # Decide what to store based on visualization mode
            if normalVis:
                plot_df[label] = series  # Only original data
            elif smoothVis:
                plot_df[label] = smoothed  # Only smoothed data
            elif overlayVis:
                plot_df[label + " (Original)"] = series  # Original
                plot_df[label + " (Smoothed)"] = smoothed  # Smoothed
        else:
            plot_df[f"Column_{colIndex}"] = np.nan

    # Plotting
    if plotlyCheckVar == 1:
        fig = px.line(plot_df, x='Time', y=plot_df.columns[1:],
                      labels={'value':'Sensor Value','Time':'Time (Seconds)'},
                      title='Data Visualizer')
        fig.show()
    else:
        plt.figure()
        for colIndex in columnIndices:
            if colIndex < df.shape[1]:
                if overlayVis:
                    # Plot overlay: original semi-transparent, smoothed bold
                    orig_label = df.columns[colIndex] + " (Original)"
                    smooth_label = df.columns[colIndex] + " (Smoothed)"
                    plt.plot(plot_df['Time'], plot_df[orig_label], label=orig_label, alpha=0.5)
                    plt.plot(plot_df['Time'], plot_df[smooth_label], label=smooth_label, linewidth=2)
                else:
                    # Plot only normal or smoothed
                    label = df.columns[colIndex]
                    plt.plot(plot_df['Time'], plot_df[label], label=label)
            else:
                label = f"Column {colIndex}"
                plt.plot(plot_df['Time'], plot_df[label], label=label)
        plt.title('Data Visualizer')
        plt.ylabel('Sensor Value')
        plt.xlabel('Time (Seconds)')
        plt.grid(True)
        plt.legend()
        plt.show()

    customWindow.destroy()
